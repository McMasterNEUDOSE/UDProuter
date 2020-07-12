#ifndef ROUTER_H
#define ROUTER_H

/*==========================================================================
** INCLUDE FILES
**==========================================================================*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/udp.h>
#include "data_types.h"

/*==========================================================================
** MACRO DEFINITIONS
**==========================================================================*/
/* Port used to communicate with child client */
#define PORT1 				1234
/* Port used to communicate with parent client */
#define PORT2 				1235
/* Choose any available host */
#define HOST 				INADDR_ANY
/* Max string buffer for confirmation msg */			
#define MAXBUFFER 			1024				
/* Number of sockets required - one for each client */													
#define NUMSOCK				2		
/* Specifies two-way comms - server sends confirmation */			
#define SERVMODE 			2				
/* Number of addresses required - client/server for each socket*/	
#define NUMADDR				SERVMODE*NUMSOCK
/* Server confirmation msg for two-way comms */
#define MSG_RECVD			"Received data."
/* Client addresses start at 1in the address table */
#define FIRST_CLIENTADDR 	1	
/* Server addresses start at 0 in the address table */				
#define FIRST_SERVADDR 		0			
/* Skip one address to get to next entry in table */		
#define NEXTADDR 			2					

/*==========================================================================
** FUNCTION PROTOTYPES
**==========================================================================*/
/* Address functions */
void initServAddrs(struct sockaddr_in addrs[]);

/* Socket API functions */
int createSocket();
void bindSocket(int fd, int servAddr, struct sockaddr_in addrTbl[]);

/* Packet processing functions */
void initPacket(DATA_stdPacket *, char [], int []);
void processPacket(DATA_stdPacket *);

/* Global utility functions */
extern int getMax(int array[]);
extern PID createChild();

#endif
