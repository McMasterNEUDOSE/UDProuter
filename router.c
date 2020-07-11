/*=======================================================================================
** File Name:  	router.c
**
** Title:  		UDP Server Application
**
** $Author:    	Stephen Scott
** $Revision: 	1.0 $
** $Date:      	2020-07-11
**
** Purpose:  	This application is a UDP server which receives packets from multiple
**				sockets and sends confirmation to clients of packets received
**
** Functions Defined:
**    initServAddrs - Initializes memory for addresses in address table and sets server HOST/PORT addresses
**    createSocket 	- Calls to socket() to create a new socket
**    bindSocket	- Calls to bind() to bind a socket to a server address in the address table
**	  processPacket	- Prints the data from the received packet
**	  getMax		- Global utility function to get max integer from array of integers
**
** Modification History:
**   Date | Author | Description
**   ---------------------------
**   2020-07-11 | Stephen Scott | Build #: Code Started
**
**=====================================================================================*/


/*=====================================================================================
** INCLUDE FILES
**=====================================================================================*/
#include "router.h"

/*=====================================================================================
** MAIN PROCESS
**=====================================================================================*/
int main(void)
{
	/* Init array to hold socket file descriptors */
	int fds[NUMSOCK];
	/* Init table which includes data buffer for each client */
	DATA_stdPacket streamTbl[NUMSOCK];
	/*
	** Init socket addresses - two for each two-way communication 
	** 		1. Server Address 1
	**		2. Client Address 1
	**		3. Server Address 2
	**		4. Client Address 2
	*/
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
	/* Various constants in loop */
	int len, n, selectret, check = 0;
	/* Stores length of client address for recvfrom/sendto() */
	len = sizeof(struct sockaddr_in);
	/* Init set of file descriptors for select() to monitor */
	fd_set readfds;
	/* Init timeout struct for select() */
	struct timeval timeout;
	/* Continue to wait for packets */
	while(1)
	{
		/* Reset bits for select() monitoring */
		FD_ZERO(&readfds);
		/* Set each file descriptor to be monitored by select() */
		for (int fd = 0; fd < NUMSOCK; fd++)
		{
			FD_SET(fds[fd], &readfds);
		}
		/* Reset timeout in case values were altered by select () */
		timeout.tv_sec 	= 5;
		timeout.tv_usec = 0;
		/* Calls select() for blocking-wait on readable sockets until timeout*/
		if ((selectret = select(getMax(fds) + 1, &readfds, NULL, NULL, &timeout)) == -1)
		{
			perror("select failed.");
			exit(EXIT_FAILURE);
		}
		/* Will be zero due to select timeout when no sockets are ready for read */
		else if (selectret == 0)
		{
			printf("Timeout. Continue.\n");
			continue;
		}
		else
		{
			/* Init index for client addresses */
			int clientAddr = FIRST_CLIENTADDR;
			/* Check which sockets are ready for reading, and read from those sockets */
			for (int fd = 0; fd < NUMSOCK; fd++)
			{
				/* If the current socket is ready to read then receive a packet and send confirmation*/
				if (FD_ISSET(fds[fd], &readfds))
				{
					/* Receive a data packet from the current socket */
					n = recvfrom(fds[fd], &streamTbl[fd], sizeof(DATA_stdPacket), MSG_WAITALL, (struct sockaddr *) &addrTbl[clientAddr], &len);
					/* Do something with the new packet */
					processPacket(&streamTbl[fd]);
					/* Send confirmation message if server is two-way */
					if (SERVMODE == 2)
					{
						sendto(fds[fd], MSG_RECVD, strlen(MSG_RECVD), MSG_CONFIRM, (struct sockaddr *) &addrTbl[clientAddr], len);
						printf("\nServer: Confirmation sent.\n\n");
					}
					check += 1;
				}
				/* Move to next client address in the table */
				clientAddr += NEXTADDR;
			}
			/* Check if all messages have been received */
			if (check == 2){
				break;
			}
		}
	}
	/* Close each open socket */
	for (int fd = 0; fd < NUMSOCK; fd++)
	{
		close(fds[fd]);
	}

	exit(EXIT_SUCCESS);
}

/*=====================================================================================
** FUNCTION DEFINITIONS
**=====================================================================================*/
void initServAddrs(struct sockaddr_in addrTbl[])
{
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
	if(bind(fd, (struct sockaddr *) &addrTbl[servAddr], sizeof(struct sockaddr_in)) == -1)
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