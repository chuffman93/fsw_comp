/* testdisp0014.cpp
 * Created on: Jul 8, 2014
 *    Author: fsw
 *  \brief Test 14 of the Dispatcher Class.
 *
 *  This test verifies that the AddRegistry method of the Dispatcher class
 *  does not allow NULL handlers to be registered.
 */
#include <iostream>
#include "POSIX.h"
#include "gtest/gtest.h"
using namespace std;



#include "core/Dispatcher.h"
#include "core/WatchdogManager.h"
using namespace std;
using namespace AllStar::Core;

static void * taskTest(void * parameters)
{
	{
		Dispatcher * dispatcher;
		MessageHandlerRegistry reg;
		Arbitrator arby(1);
		bool ret;

		dispatcher = dynamic_cast<Dispatcher *> (Factory::GetInstance(DISPATCHER_SINGLETON));

		if (NULL == dispatcher)
		{
			ADD_FAILURE(); pthread_exit(0);
		}

		cout << "Initialized" << endl;
		ret = dispatcher->AddRegistry(1, NULL, &arby);
		if (ret)
		{
			ADD_FAILURE(); pthread_exit(0);
		}

		ret = dispatcher->AddRegistry(2, &reg, NULL);
		if (ret)
		{
			ADD_FAILURE(); pthread_exit(0);
		}
	}

	pthread_exit(0);

}


TEST(TestDispatcher, Null_Registry)
{
	pthread_t testThread;

	pthread_create(&testThread, NULL, &taskTest, NULL);

	pthread_join(testThread, NULL);
	Factory::Destroy(DISPATCHER_SINGLETON);
}
