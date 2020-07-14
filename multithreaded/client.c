/*==========================================================================
** File Name:  	client.c
**
** Title:  	UDP Client Application
**
** $Author:    	Stephen Scott
** $Revision: 	1.0 $
** $Date:      	2020-07-11
**
** Purpose:  	This application is a UDP client which creates a forked 
** process, and sends packets to two sockets in the UDP server application.
**
** Functions Defined:
**    createChild 	- Creates child process using fork()
**    createSocket 	- Calls to socket() to create a new socket
**    initPacket	- Initializes a data packets with values
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

/*==========================================================================
** MAIN PROCESS
**==========================================================================*/
int main(void)
{
	/* Create data constants */
	char letters[2];
	UINT8 nums[2];
	/* Create child process */
	PID pid = createChild();
	/* Create a socket */
	int fd = createSocket();
	/* Create struct for storing packet data */
	DATA_stdPacket packet;
	/* Create struct for storing server address*/
	struct sockaddr_in servaddr;
	/* Allocate memory for server address */
	memset(&servaddr, 0, sizeof(servaddr));
	/* Set server address host - same host for each process */
	servaddr.sin_addr.s_addr = HOST;
	/* Check which process I am, and adjust PORT and data accordingly */
	if (pid == 0)
	{
		printf("I am the child process.\n");
		/* Set port of child process */
		servaddr.sin_port = htons(PORT1);
		/* Init child packet data with some values */
		letters[0] = 'C'; letters[1] = 'H'; nums[0] = 1; nums[1] = 2;
		initPacket(&packet, letters, nums);
	}
	else
	{
		printf("I am the parent process.\n");
		/* Set port of parent process */
		servaddr.sin_port = htons(PORT2);
		/* Init parent packet data with some values */
		letters[0] = 'P'; letters[1] = 'A'; nums[0] = 3; nums[1] = 4;
		initPacket(&packet, letters, nums);
	}
	/* Init len for sendto() function */
	int len = sizeof(servaddr);
	/* Loop sending packets at 1HZ*/
	while(1)
	{
		/* Send packet to server */
		sendto(fd, (const DATA_stdPacket *) &packet, sizeof(DATA_stdPacket), 
		MSG_CONFIRM, (const struct sockaddr *) &servaddr, len);
		printf("\nClient %d: Packet sent.\n\n", pid);
		sleep(1);
	}

	close(fd);

	exit(EXIT_SUCCESS);
}

/*==========================================================================
** FUNCTION DEFINITIONS
**==========================================================================*/
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

void initPacket(DATA_stdPacket *packet, char letters[], UINT8 nums[])
{
	packet->firstChar 	= letters[0];
	packet->secondChar 	= letters[1];
	packet->firstNum	= nums[0];
	packet->secondNum	= nums[1];
}
