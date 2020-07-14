#ifndef QUEUE_H
#define QUEUE_H

/*==========================================================================
** INCLUDE FILES
**==========================================================================*/
#include "../data_types.h"

/*==========================================================================
** FUNCTION DEFINITIONS
**==========================================================================*/
packetQueue *createQueue(unsigned capacity)
{
	packetQueue *queue = malloc(sizeof(packetQueue));
	queue->capacity = capacity;
	queue->front = queue->size = 0;
	queue->rear = capacity - 1;
	queue->array = malloc(queue->capacity*sizeof(DATA_stdPacket));
	return queue;
}

int isFull(packetQueue *queue)
{
	return (queue->size == queue->capacity);
}

int isEmpty(packetQueue *queue)
{
	return (queue->size == 0);
}

void enqueue(packetQueue *queue, DATA_stdPacket packet)
{
	queue->rear = (queue->rear + 1) % queue->capacity;
	queue->array[queue->rear] = packet;
	queue->size = queue->size + 1;
	printf("Packet enqueued.\n");
}
/* Function call must check if queue is empty first */
DATA_stdPacket dequeue(packetQueue *queue)
{
	DATA_stdPacket packet = queue->array[queue->front];
	queue->front = (queue->front + 1) % queue->capacity;
	queue->size = queue->size - 1;
	return packet;
}

DATA_stdPacket front(packetQueue *queue)
{
	return queue->array[queue->front];
}

DATA_stdPacket rear(packetQueue *queue)
{
	return queue->array[queue->rear];
}

#endif