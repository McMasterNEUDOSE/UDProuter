#ifndef DATA_TYPES_H
#define DATA_TYPES_H

/*==========================================================================
** INCLUDE FILES
**==========================================================================*/
#include <stdbool.h>
#include <pthread.h>
#include <semaphore.h>

/*==========================================================================
** MACRO DEFINITIONS
**==========================================================================*/
#define UINT8 	uint8_t
#define PID 	pid_t
#define MUTEX	pthread_mutex_t
#define SEM 	sem_t

/*==========================================================================
** CUSTOM DATA TYPES
**==========================================================================*/
typedef struct stdPacket
{
	char 	firstChar;
	char 	secondChar;
	UINT8	firstNum;
	UINT8 	secondNum;
} DATA_stdPacket;

typedef struct Queue
{
	int front, rear, size;
	unsigned capacity;
	DATA_stdPacket *array;
} packetQueue;

typedef struct threadData
{
	pthread_t 			tid;
	int					threadnum;
	int					fd;
	packetQueue 		*buffer;
	struct sockaddr_in 	clientAddr;
} DATA_pthread;

#endif
