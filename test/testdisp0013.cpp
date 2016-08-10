/* testdisp0013.cpp
 * Created on: Jul 8, 2014
 *    Author: fsw
 *  \brief Test 13 of the Dispatcher Class.
 *
 *  This test verifies that the GetInstance method of Dispatcher returns
 *  true instantly if it is called a second time and that the instance
 *  does not change if this second call is made.
 */
#include <iostream>
#include "POSIX.h"
#include "gtest/gtest.h"

#include "core/Dispatcher.h"
#include "core/WatchdogManager.h"
using namespace std;
using namespace AllStar::Core;

static void * taskTest(void * parameters)
{
	Dispatcher * dispatcher, * dispatcher2;

	dispatcher = dynamic_cast<Dispatcher *> (Factory::GetInstance(DISPATCHER_SINGLETON));

	if (NULL == dispatcher)
	{
		ADD_FAILURE(); pthread_exit(0);
	}

	dispatcher2 = dynamic_cast<Dispatcher *> (Factory::GetInstance(DISPATCHER_SINGLETON));

	if (dispatcher != dispatcher2)
	{
		ADD_FAILURE(); pthread_exit(0);
	}

	pthread_exit(0);

}


TEST(TestDispatcher, GetInstanceIdentity)
{
	pthread_t testThread;

	pthread_create(&testThread, NULL, &taskTest, NULL);

	pthread_join(testThread, NULL);
	Factory::Destroy(DISPATCHER_SINGLETON);
}
