/*==========================================================================
** File Name:  	router.c
**
** Title: 		UDP Server Application
**
** $Author:		Stephen Scott
** $Revision: 	1.0 $
** $Date:      	2020-07-11
**
** Purpose:  	This application is a UDP server which receives packets from
** 				multiple sockets and sends confirmation to clients.
**
** Functions Defined:
**    	initServAddrs 	- 	Initializes memory for addresses in address table 
** 							and sets server HOST/PORT addresses
**    	createSocket 	- 	Calls to socket() to create a new socket
**   	bindSocket		- 	Calls to bind() to bind a socket to a server
** 							address in the address table
**		processPacket	- 	Prints the data from the received packet
**		getMax			- 	Global utility function to get max integer from 
** 							array of integers
**
** Modification History:
**   Date | Author | Description
**   ---------------------------
**   2020-07-11 | Stephen Scott | Build #: Code Started
**
**==========================================================================*/


/*==========================================================================
** INCLUDE FILES
**==========================================================================*/
#include "../router.h"
#include "queue.h"

/*==========================================================================
** GLOBAL VARIABLES
**==========================================================================*/
/* Init input buffer for each client */
packetQueue *buffers[NUMSOCK];
/* Mutual exclusion for input buffers */
MUTEX buffers_mutex[NUMSOCK];
/* Counting semaphores for full buffer */
SEM buffers_sem[NUMSOCK];
/* Pthread structure to hold thread info */
DATA_pthread threads[NUMSOCK];

/*==========================================================================
** SOCKET READING THREAD
**==========================================================================*/
void *readSocket(void *args)
{
	int thread = -1;
	for (int i; i < NUMSOCK; i++){
		if (threads[i].tid == pthread_self())
		{
			thread = threads[i].threadnum;
		}
	}
	if (thread < 0)
	{
		printf("No matching thread\n");
		exit(EXIT_FAILURE);
	}
	DATA_stdPacket packet;
	int len = sizeof(struct sockaddr_in);
	while(1)
	{
		recvfrom(threads[thread].fd, &packet, 
				sizeof(DATA_stdPacket), MSG_WAITALL, 
				(struct sockaddr *) &threads[thread].clientAddr, &len);
		printf("Thread %d: received packet\n", thread);
		sem_wait(&buffers_sem[thread]);
		pthread_mutex_lock(&buffers_mutex[thread]);
		enqueue(threads[thread].buffer, packet);
		pthread_mutex_unlock(&buffers_mutex[thread]);
	}
	close(threads[thread].fd);
	pthread_exit(0);
}

/*==========================================================================
** MAIN PROCESS
**==========================================================================*/
int main(void)
{
	/* Init array to hold socket file descriptors */
	int fds[NUMSOCK];
	/* Create a buffer queue for each client */
	for (int buffer = 0; buffer < NUMSOCK; buffer++)
	{
		buffers[buffer] = createQueue(MAXBUFFER);
		printf("Buffer Pointer: %d ->%p\n", buffer, buffers[buffer]);
	}
	/* Init table to hold client and server addresses */
	struct sockaddr_in addrTbl[NUMADDR];
	/* Init server addresses */
	initServAddrs(addrTbl);
	/* Start binding the first server address */
	int servAddr = FIRST_SERVADDR;
	/* Create and bind socket for each file descriptor */
	for (int fd = 0; fd < NUMSOCK; fd++)
	{
		/* Create the socket */
		fds[fd] = createSocket();
		/* Bind socket to server address */
		bindSocket(fds[fd], servAddr, addrTbl);
		/* Move to next server address in address table */
		servAddr += NEXTADDR;
	}
	/* Int to store return from pthread_create */
	int createret;
	/* Int to store index in address table */
	int tblIndex = FIRST_CLIENTADDR;
	/* Create socket reading threads */
	for (int i = 0; i < NUMSOCK; i++)
	{
		if ((createret = pthread_create(&threads[i].tid, NULL, *readSocket, NULL)) != 0)
		{
			perror("thread failed");
			exit(EXIT_FAILURE);
		}
		printf("Created new thread.\n");
		threads[i].threadnum = i;
		threads[i].fd = fds[i];
		threads[i].buffer = buffers[i];
		threads[i].clientAddr = addrTbl[i+tblIndex];
		tblIndex += NEXTADDR;
		sem_init(&buffers_sem[i], 0, MAXBUFFER - 1);
	}
	DATA_stdPacket packet;
	int packetsProcessed = 0;
	/* Do work with incoming packets */
	while(1)
	{
		for (int i = 0; i < NUMSOCK; i++)
		{
			pthread_mutex_lock(&buffers_mutex[i]);
			if (isEmpty(threads[i].buffer))
			{
				pthread_mutex_unlock(&buffers_mutex[i]);
				continue;
			}
			else
			{
				packet = dequeue(threads[i].buffer);
				printf("Packet dequeued from thread %d\n", i);
				sem_post(&buffers_sem[i]);
				pthread_mutex_unlock(&buffers_mutex[i]);
				processPacket(&packet);
				packetsProcessed += 1;
				printf("%d Packets processed.\n", packetsProcessed);
			}
		}
		sleep(2);
	}
	/* Join threads when finished */
	for (int i = 0; i < NUMSOCK; i++)
	{
		pthread_join(threads[i].tid, NULL);
	}
	exit(EXIT_SUCCESS);
}

/*==========================================================================
** FUNCTION DEFINITIONS
**==========================================================================*/
void initServAddrs(struct sockaddr_in addrTbl[])
{
	/*
	** Init socket addresses - two for each two-way communication 
	** 		1. Server Address 1
	**		2. Client Address 1
	**		3. Server Address 2
	**		4. Client Address 2
	*/
	/* Init memory for all addresses */
	memset(addrTbl, 0, NUMADDR*sizeof(struct sockaddr_in));
	/*
	** Provide data for each address
	*/
	/* Address 1 */
	addrTbl[0].sin_port = htons(PORT1);
	addrTbl[0].sin_addr.s_addr = HOST;
	/* Address 2 */
	addrTbl[2].sin_port = htons(PORT2);
	addrTbl[2].sin_addr.s_addr = HOST;
}

int createSocket()
{
	/* Init fd to store output from socket() */
	int fd;
	/* Call socket() and handle errors */
	if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
	{
		perror("socket failed");
		exit(EXIT_FAILURE);
	}
	else
	{
		printf("New Socket: %d\n", fd);
		return fd;
	}
}

void bindSocket(int fd, int servAddr, struct sockaddr_in addrTbl[])
{
	if(bind(fd, (struct sockaddr *) &addrTbl[servAddr], 
		sizeof(struct sockaddr_in)) == -1)
	{
		perror("bind failed");
		exit(EXIT_FAILURE);
	}
}

void processPacket(DATA_stdPacket *packet)
{
	printf("Client says:\n\t\t%c\n\t\t%c\n\t\t%hhu\n\t\t%hhu\n",
			packet->firstChar,
			packet->secondChar,
			packet->firstNum,
			packet->secondNum);
}

int getMax(int array[])
{
	/* Init max it to store result */
	int max = 0;
	/* Get total numbers in array */
	int nums = sizeof(&array)/sizeof(array[0]);
	/* Loop through each num and compare to max */
	for (int i = 0; i < nums; i++)
	{
		if (array[i] > max)
		{
			max = array[i];
		}
		else
		{
			continue;
		}
	}
	return max;
}
