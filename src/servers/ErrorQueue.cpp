/*
 * ErrorQueue.cpp
 *
 *  Created on: Mar 23, 2013
 *      Author: Brian Campuzano
 */

#include "servers/ErrorQueue.h"
#include <stdio.h>
#include <stdlib.h>

#include "core/FSWPacket.h"
#include "core/Singleton.h"
#include "core/Factory.h"

using namespace AllStar::Core;

namespace AllStar
{
	namespace Servers
	{
		
		ErrorQueue::ErrorQueue(void)
		{
			errQueueName = "/errQueueHandle";
			errQinit = mqCreate(&(errQueueHandle), &errQueueAttr, errQueueName);
		}

		ErrorQueue::~ErrorQueue(void)
		{
			size_t waiting = mq_size(errQueueHandle, errQueueAttr);
			for (size_t i = 0; i < waiting; ++i)
			{
				FSWPacket * packet;
				mq_timed_receive(errQueueName, &packet, MAX_BLOCK_TIME, 0);
				delete packet;
			}

			mq_unlink(errQueueName);
		}

		ErrorQueue & ErrorQueue::operator=(const ErrorQueue & source)
		{
			return *this;
		}

		void ErrorQueue::Initialize(void)
		{
			
		}

#ifdef HOST
		void ErrorQueue::Destroy(void)
		{

		}
#endif
		
		bool ErrorQueue::IsFullyInitialized(void)
		{
			return(Singleton::IsFullyInitialized() && errQueueHandle != -1);
		}


		bool ErrorQueue::EnqueueError(FSWPacket * packet)
		{
			// Error queue is not initialized!
			size_t numPackets = mq_size(errQueueHandle, errQueueAttr);
			bool rv = false;
			if (numPackets < ERROR_QUEUE_LENGTH)
			{
				rv = (true == mq_timed_send(errQueueName, &packet, MAX_BLOCK_TIME, 0));
			}
			return rv;
		}

		size_t ErrorQueue::ErrorsWaiting(void)
		{
			size_t rv = (size_t) mq_size(errQueueHandle, errQueueAttr);
			return rv;
		}

		FSWPacket * ErrorQueue::GetNextError()
		{
			FSWPacket * packetOut = NULL;
			// Get the next packet.
			size_t rv = (size_t) mq_size(errQueueHandle, errQueueAttr);
			if(rv > 0)
			{
				if (mq_timed_receive(errQueueName, &packetOut, MAX_BLOCK_TIME, 0) == false)
				{
					//Not good there should have been a packet in the queue
					packetOut = NULL;
					return packetOut;
				}
				//FSWPacket dequeued successfully
				return packetOut;
			}
			// No packets available to dequeue
			packetOut = NULL;
			return packetOut;
		}
	}
}
