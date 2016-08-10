/*! \file testwdm0001.cpp
 *  \brief Test 1 of the WatchdogManager Class.
 *
 *  This test verifies that the GetInstance method of the WatchdogManager class
 *  correctly returns the same reference to a WatchdogManager object on
 *  subsequent calls to the method. It also verifies that tasks are set to NULL
 *  when calling DeleteTask
 */

#include <iostream>
#include "POSIX.h"
#include "gtest/gtest.h"

#include <cassert>
#include <iostream>

#include "core/WatchdogManager.h"
#include "core/Singleton.h"
#include "core/Factory.h"
using namespace std;
using namespace AllStar::Core;

static void * taskTest1A(void * parameters)
{
	WatchdogManager *wdm, *wdm2;

	wdm = dynamic_cast<WatchdogManager *> (Factory::GetInstance(WATCHDOG_MANAGER_SINGLETON));

	if(wdm == NULL)
	{
		ADD_FAILURE() << "wdm is null";
		pthread_exit(0);
	}

	wdm2 = dynamic_cast<WatchdogManager *> (Factory::GetInstance(WATCHDOG_MANAGER_SINGLETON));

	if(wdm != wdm2)
	{
		ADD_FAILURE() << "wdm and wdm2 are not equal";
		pthread_exit(0);
	}

	pthread_exit(0);
}

TEST(WatchdogManager, GetInstance)
{
	WatchdogManager *wdm = dynamic_cast<WatchdogManager *> (Factory::GetInstance(WATCHDOG_MANAGER_SINGLETON));
	PThread *task1A = wdm->CreateTask(NULL, &taskTest1A, NULL, true);

	if (task1A == NULL)
	{
		ADD_FAILURE() << "task1A is null after being created";
		exit(0);
	}

	pthread_join(task1A->tid, NULL);

	bool deleted = wdm->DeleteTask(task1A);
	if (!deleted)
	{
		ADD_FAILURE() << "Failed to delete task(s)";
		exit(0);
	}

	Factory::Destroy(WATCHDOG_MANAGER_SINGLETON);
}
