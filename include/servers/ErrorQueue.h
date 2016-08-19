/*
 * ErrorQueue.h
 *
 *  Created on: Mar 23, 2013
 *      Author: Brian Campuzano
 */

#ifndef ERRORQUEUE_H_
#define ERRORQUEUE_H_

#include "core/ACPPacket.h"
#include "core/Singleton.h"
#include "core/Factory.h"


namespace AllStar{
namespace Servers{

#define ERROR_QUEUE_LENGTH 100

class ErrorQueue : public AllStar::Core::Singleton{
	friend class AllStar::Core::Factory;

public:

	bool EnqueueError(AllStar::Core::ACPPacket * packet);
	size_t ErrorsWaiting(void);
	AllStar::Core::ACPPacket * GetNextError(void);

private:
	static void Initialize(void);

#ifdef HOST
	static void Destroy(void);
#endif

	bool IsFullyInitialized(void);

	ErrorQueue(void);
	~ErrorQueue(void);
	ErrorQueue & operator=(const ErrorQueue & source);

	mqd_t errQueueHandle;
	struct mq_attr errQueueAttr;
	char * errQueueName;
	int errQinit;
};

}
}



#endif /* ERRORQUEUE_H_ */
