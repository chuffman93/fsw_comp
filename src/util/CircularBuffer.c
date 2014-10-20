/*
 * CircularBuffer.c
 *
 *  Created on: Jun 28, 2013
 *      Author: Admin
 */

#include "util/CircularBuffer.h"
#include <string.h>
#include <stdlib.h>

CircularBuffer * CreateCircularBuffer(uint32 capacity)
{
    CircularBuffer * circBuffer = malloc(sizeof(CircularBuffer));
    circBuffer->capacity  = capacity;
    circBuffer->used = 0;
    circBuffer->head = 0;
    circBuffer->tail = 0;
    circBuffer->buffer = calloc(circBuffer->capacity, sizeof(uint8));

    return circBuffer;
}

uint8 * GetCurrentBuffer(CircularBuffer * circBuffer)
{
	uint8 * rv;
	if(circBuffer->used == 0)
	{
		return NULL;
	}
	rv = (uint8 *) malloc(circBuffer->used*sizeof(uint8));
	if(circBuffer->head > circBuffer->tail)
	{
		memcpy(rv, (circBuffer->buffer + circBuffer->tail), circBuffer->used);
	}
	else
	{
		memcpy(rv, (circBuffer->buffer + circBuffer->tail), (circBuffer->capacity - circBuffer->tail));
		rv += circBuffer->capacity - circBuffer->tail;
		memcpy(rv, circBuffer->buffer, circBuffer->head);
	}
	return rv;
}

void FlushBuffer(CircularBuffer * circBuffer)
{
	circBuffer->used = 0;
	circBuffer->head = 0;
	circBuffer->tail = 0;
}

void DestroyCircularBuffer(CircularBuffer * circBuffer)
{
    if(circBuffer)
    {
        free(circBuffer->buffer);
        free(circBuffer);
    }
}
