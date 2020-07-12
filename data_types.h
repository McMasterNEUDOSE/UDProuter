#ifndef DATA_TYPES_H
#define DATA_TYPES_H

/*==========================================================================
** INCLUDE FILES
**==========================================================================*/
#include <stdbool.h>

/*==========================================================================
** MACRO DEFINITIONS
**==========================================================================*/
#define UINT8 	uint8_t
#define PID 	pid_t

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

#endif
