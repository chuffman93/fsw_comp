/*
 * CircularBuffer.h
 *
 *  Created on: Jun 28, 2013
 *      Author: Admin
 */

#ifndef CIRCULARBUFFER_H_
#define CIRCULARBUFFER_H_

#include "core/StdTypes.h"

typedef struct
{
    uint8 * buffer;
    uint32 capacity;
    uint32 used;
    uint32 head;
    uint32 tail;
} CircularBuffer;

//extern CircularBuffer * inputBuffer;

CircularBuffer * CreateCircularBuffer(uint32 capacity);

uint8 * GetCurrentBuffer(CircularBuffer * circBuffer);

void FlushBuffer(CircularBuffer * circBuffer);

void DestroyCircularBuffer(CircularBuffer * circBuffer);

#endif /* CIRCULARBUFFER_H_ */
