/*! \file testwdm0005.cpp
 *  \brief Test 5 of the WatchdogManager Class.
 *
 *  This test verifies that the AllRunningTasksActive method of the
 *  WatchdogManager class returns true if all running tasks have kicked the
 *  manager in the latest cycle and that afterwards, all tasks are unkicked.
 *  It also verifies that if one or more tasks have not been kicked, then
 *  the method returns false.
 */

#include <iostream>
#include "POSIX.h"
#include "gtest/gtest.h"

#include <iostream>

#include "core/WatchdogManager.h"
using namespace std;
using namespace AllStar::Core;

PThread *task5A, *task5B;
sem_t sync5A, sync5B, sync5C;

static void * taskTest5A(void * parameters)
{
	sem_wait(&sync5B);

	WatchdogManager *wdm = dynamic_cast<WatchdogManager *>(Factory::GetInstance(WATCHDOG_MANAGER_SINGLETON));

	if (wdm == NULL)
	{
		ADD_FAILURE() << "wdm is null";
		pthread_exit(0);
	}

	wdm->Kick();

	if (wdm->GetNumberOfTasks() != 2)
	{
		ADD_FAILURE() << "incorrect number of tasks";
		pthread_exit(0);
	}

	if (!wdm->AllRunningTasksActive())
	{
		ADD_FAILURE() << "All running tasks should be active.";
		pthread_exit(0);
	}

	wdm->DeactivateTask(task5B);
	wdm->Kick();

	if (!wdm->AllRunningTasksActive())
	{
		ADD_FAILURE() << "All running tasks should be active";
		pthread_exit(0);
	}

	sem_post(&sync5C);
	pthread_exit(0);
}

void * taskTest5B(void * parameters)
{
	sem_wait(&sync5A);

	WatchdogManager *wdm = dynamic_cast<WatchdogManager *>(Factory::GetInstance(WATCHDOG_MANAGER_SINGLETON));
	wdm->Kick();

	sem_post(&sync5B);

	if (wdm == NULL)
	{
		ADD_FAILURE() << "wdm is null in task5B";
		pthread_exit(0);
	}

	while(1)
	{
		sleep(1);
	}

	pthread_exit(0);
}

TEST(WatchdogManager, AllRunningTasksActive)
{
	WatchdogManager *wdm = dynamic_cast<WatchdogManager *>(Factory::GetInstance(WATCHDOG_MANAGER_SINGLETON));

	sem_init(&sync5A, SHARE_TO_THREADS, 1);
	sem_init(&sync5B, SHARE_TO_THREADS, 1);
	sem_init(&sync5C, SHARE_TO_THREADS, 1);

	sem_wait(&sync5A);
	sem_wait(&sync5B);
	sem_wait(&sync5C);

	task5A = wdm->CreateTask(NULL, &taskTest5A, NULL, false);
	task5B = wdm->CreateTask(NULL, &taskTest5B, NULL, false);

	if (!wdm->AllRunningTasksActive())
	{
		ADD_FAILURE() << "All tasks should be active";
		exit(0);
	}

	task5A->start();
	task5B->start();

	sem_post(&sync5A); // All tasks have been started
	sem_wait(&sync5C); // Both tasks are done

	pthread_join(task5A->tid, NULL);

	bool deleted = (wdm->DeleteTask(task5A) && wdm->DeleteTask(task5B)) ;
	if (!deleted)
	{
		ADD_FAILURE() << "Failed to delete task(s)";
		exit(0);
	}

	Factory::Destroy(WATCHDOG_MANAGER_SINGLETON);
}
