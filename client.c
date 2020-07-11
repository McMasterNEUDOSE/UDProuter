/*=======================================================================================
** File Name:  	client.c
**
** Title:  		UDP Client Application
**
** $Author:    	Stephen Scott
** $Revision: 	1.0 $
** $Date:      	2020-07-11
**
** Purpose:  	This application is a UDP client which creates a forked process, and
**				sends packets to two different sockets in the UDP server application
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
	/* Create child process */
	PID pid = createChild();
	/* Create a socket */
	int fd = createSocket();
	/* Init buffer for receiving server confirmation packets */
	char buffer[MAXBUFFER];
	/* Create struct for storing packet data */
	DATA_stdPacket packet;
	/* Create struct for storing server address*/
	struct sockaddr_in servaddr;
	/* Allocate memory for server address */
	memset(&servaddr, 0, sizeof(servaddr));
	/* Set server address host - in this case the server is on the same host for each process */
	servaddr.sin_addr.s_addr = HOST;
	/* Check to see which process I am, and adjust PORT and data accordingly */
	if (pid == 0)
	{
		printf("I am the child process.\n");
		/* Set port of child process */
		servaddr.sin_port = htons(PORT1);
		/* Init child packet data with some values */
		initPacket(&packet, 'C', 'H', 1, 2);
	}
	else
	{
		printf("I am the parent process.\n");
		/* Set port of parent process */
		servaddr.sin_port = htons(PORT2);
		/* Init parent packet data with some values */
		initPacket(&packet, 'P', 'A', 3, 4);
	}

	int len, n;

	len = sizeof(servaddr);
	/* Send packet to server */
	sendto(fd, (const DATA_stdPacket *) &packet, sizeof(DATA_stdPacket), MSG_CONFIRM, (const struct sockaddr *) &servaddr, len);
	printf("\nClient: Packet sent.\n\n");
	/* If the server mode is two-way then receive confirmation packet from server */
	if (SERVMODE == 2)
	{
		n = recvfrom(fd, (char *)buffer, MAXBUFFER, MSG_WAITALL, (struct sockaddr *) &servaddr, &len);
		buffer[n] = '\0';
		printf("Server says: \n\t\t%s\n", buffer);
	}
	/* Close socket in each process */
	close(fd);

	exit(EXIT_SUCCESS);
}

/*=====================================================================================
** FUNCTION DEFINITIONS
**=====================================================================================*/

PID createChild()
{
	PID pid;
	if((pid = fork()) == -1)
	{
		perror("fork failed");
		exit(EXIT_FAILURE);
	}
	else
	{
		return pid;
	}
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

void initPacket(DATA_stdPacket *packet, char char1, char char2, int num1, int num2)
{
	packet->firstChar 	= char1;
	packet->secondChar 	= char2;
	packet->firstNum	= num1;
	packet->secondNum	= num2;
}