/* testdisp0002.cpp
 * Created on: Jun 11, 2014
 *    Author: fsw
 *  \brief Test 2 of the Dispatcher Class.
 *
 *  This test verifies that the AddRegistry method of the Dispatcher class
 *  only allows a single MessageHandlerRegistry pointer for a given
 *  LocationIDType.
 */
#include <iostream>
#include "POSIX.h"
#include "gtest/gtest.h"

#include "core/Dispatcher.h"

using namespace std;
using namespace AllStar::Core;

static void * taskTest(void * parameters)
{
	Dispatcher * dispatcher;
	bool ret;

	dispatcher = dynamic_cast<Dispatcher *> (Factory::GetInstance(DISPATCHER_SINGLETON));

	if (NULL == dispatcher)
	{
		ADD_FAILURE() << "dispatcher is null";
		pthread_exit(0);
	}


	MessageHandlerRegistry * reg = new MessageHandlerRegistry( );
	Arbitrator * arby = new Arbitrator(1);

	ret = dispatcher->AddRegistry(1, reg, arby);
	if (!ret)
	{
		ADD_FAILURE();
		pthread_exit(0);
	}


	ret = dispatcher->AddRegistry(1, reg, arby);


	if (ret)
	{
		ADD_FAILURE();
		pthread_exit(0);
	}

	delete reg;
	delete arby;

	pthread_exit(0);
}

TEST(TestDispatcher, AddRegistry)
{
	mq_unlink("/subsystemQueueHandle");
	mq_unlink("/queueHandle");
	pthread_t testThread;
	pthread_create(&testThread, NULL, &taskTest, NULL);

	pthread_join(testThread, NULL);

	Factory::Destroy(DISPATCHER_SINGLETON);
}
