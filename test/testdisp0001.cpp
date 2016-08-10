/* testdisp0001.cpp
 * Created on: Jun 11, 2014
 *    Author: fsw
 *  \brief Test 1 of the Dispatcher Class.
 *
 *  This test verifies that the GetInstance method of the Dispatcher class
 *  correctly returns the same reference to a Dispatcher object on subsequent
 *  calls to the method.
 */
#include <iostream>
#include "POSIX.h"
#include "gtest/gtest.h"

#include "core/Singleton.h"
#include "core/Factory.h"
#include "core/Dispatcher.h"

using namespace std;
using namespace AllStar::Core;


static void * taskTest(void * parameters)
{
	Dispatcher * dispatcher, * dispatcher2;

	dispatcher = dynamic_cast<Dispatcher *> (Factory::GetInstance(DISPATCHER_SINGLETON));

	if (NULL == dispatcher)
	{
		ADD_FAILURE() << "dispatcher null" << endl;
		pthread_exit(0);
	}

	dispatcher2 = dynamic_cast<Dispatcher *> (Factory::GetInstance(DISPATCHER_SINGLETON));

	if (dispatcher != dispatcher2)
	{
		ADD_FAILURE() << "dispatcher1 is not equal to dispatcher2" << endl;
		pthread_exit(0);
	}

	pthread_exit(0);

}

TEST(TestDispatcher, GetInstance)
{
	mq_unlink("/subsystemQueueHandle");
	mq_unlink("/queueHandle");
	pthread_t testThread;
	pthread_create(&testThread, NULL, &taskTest, NULL);

	/* Start the scheduler.  This function should not return as it causes the execution
	context to change from main() to one of the created tasks. */
	pthread_join(testThread, NULL);
	
	Factory::Destroy(DISPATCHER_SINGLETON);

}
