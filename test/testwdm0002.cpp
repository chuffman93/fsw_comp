/*! \file testwdm0002.cpp
 *  \brief Test 2 of the WatchdogManager Class.
 *
 *  This test verifies that the GetInstance method of WatchdogManager returns
 *  true instantly if it is called a second time and that the instance
 *  does not change if this second call is made.
 */

#include <iostream>
#include "POSIX.h"
#include "gtest/gtest.h"

#include <cassert>
#include <iostream>

#include "core/WatchdogManager.h"
using namespace std;
using namespace AllStar::Core;

static void * taskTest2A(void * parameters)
{
	WatchdogManager * wdm, *wdm2;

	wdm = dynamic_cast<WatchdogManager *>(Factory::GetInstance(WATCHDOG_MANAGER_SINGLETON));

	if (NULL == wdm)
	{
		ADD_FAILURE() << "wdm is null";
		pthread_exit(0);
	}

	wdm2 = dynamic_cast<WatchdogManager *>(Factory::GetInstance(WATCHDOG_MANAGER_SINGLETON));

	if (wdm != wdm2)
	{
		ADD_FAILURE() << "wdm and wdm2 are not equal";
		pthread_exit(0);
	}

	pthread_exit(0);
}

TEST(WatchdogManager, GetInstanceCallTwice)
{
	WatchdogManager *wdm = dynamic_cast<WatchdogManager *>(Factory::GetInstance(WATCHDOG_MANAGER_SINGLETON));

	PThread *task2A = wdm->CreateTask(NULL, &taskTest2A, NULL, true);

	pthread_join(task2A->tid, NULL);

	bool deleted = wdm->DeleteTask(task2A);
	if (!deleted)
	{
		ADD_FAILURE() << "Failed to delete task(s)";
		exit(0);
	}

	Factory::Destroy(WATCHDOG_MANAGER_SINGLETON);

}
