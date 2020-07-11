#ifndef ROUTER_H
#define ROUTER_H

/*=====================================================================================
** INCLUDE FILES
**=====================================================================================*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/udp.h>
#include "data_types.h"

/*=====================================================================================
** MACRO DEFINITIONS
**=====================================================================================*/
#define PORT1				      1234				      /* Port used to communicate with child client */
#define PORT2				      1235				      /* Port used to communicate with parent client */
#define HOST				      INADDR_ANY			  /* Choose any available host */
#define MAXBUFFER			    1024				      /* Max string buffer for confirmation msg */
#define NUMSOCK				    2					        /* Number of sockets required - one for each client */
#define SERVMODE			    2					        /* Specifies two-way comms (ie. server sends confirmation of msg received) */
#define NUMADDR				    SERVMODE*NUMSOCK	/* Number of addresses required depends on whether server confirmation is enabled*/
#define MSG_RECVD			    "Received data."	/* Server confirmation msg for two-way comms */
#define FIRST_CLIENTADDR	1					        /* Client addresses start at index 1 in the address table */
#define FIRST_SERVADDR		0					        /* Server addresses start at index 0 in the address table */
#define NEXTADDR			    2					        /* Skip one address to get to the next server or client address in the address table */

/*=====================================================================================
** FUNCTION PROTOTYPES
**=====================================================================================*/
/* Address functions */
void initServAddrs(struct sockaddr_in addrs[]);

/* Socket API functions */
int createSocket();
void bindSocket(int fd, int servAddr, struct sockaddr_in addrTbl[]);

/* Packet processing functions */
void initPacket(DATA_stdPacket *, char, char, int, int);
void processPacket(DATA_stdPacket *);

/* Global utility functions */
extern int getMax(int array[]);
extern PID createChild();

#endif
