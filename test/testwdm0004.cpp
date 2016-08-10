/*! \file testwdm0004.cpp
 *  \brief Test 4 of the WatchdogManager Class.
 *
 *  This test verifies that the Kick method of the WatchdogManager class
 *  correctly kicks the state of the current task if it has been added.  If
 *  it hasn't, the task is first added as a running task and then kicked.
 */

#include <iostream>
#include "POSIX.h"
#include "gtest/gtest.h"

#include <iostream>

#include "core/WatchdogManager.h"
using namespace std;
using namespace AllStar::Core;

PThread *task4A, *task4B;
sem_t sync4A, sync4B, sync4C, sync4D;

static void * taskTest4A(void * parameters)
{
	sem_wait(&sync4A);

	WatchdogManager * wdm =	dynamic_cast<WatchdogManager *>(Factory::GetInstance(WATCHDOG_MANAGER_SINGLETON));

	if (wdm == NULL)
	{
		ADD_FAILURE() << "wdm is null";
		pthread_exit(0);
	}

	if (wdm->GetNumberOfTasks() != 2)
	{
		ADD_FAILURE() << "wrong number of tasks";
		pthread_exit(0);
	}

	wdm->Kick();
	sem_post(&sync4C);

	pthread_exit(0);
}

void * taskTest4B(void * parameters)
{
	sem_wait(&sync4B);

	WatchdogManager * wdm =	dynamic_cast<WatchdogManager *>(Factory::GetInstance(WATCHDOG_MANAGER_SINGLETON));

	if (wdm == NULL)
	{
		ADD_FAILURE() << "wdm is null";
		pthread_exit(0);
	}

	wdm->Kick();
	sem_post(&sync4D);

	pthread_exit(0);
}

TEST(WatchdogManager, Kick)
{
	WatchdogManager *wdm = dynamic_cast<WatchdogManager *>(Factory::GetInstance(WATCHDOG_MANAGER_SINGLETON));

	sem_init(&sync4A, SHARE_TO_THREADS, 1);
	sem_init(&sync4B, SHARE_TO_THREADS, 1);
	sem_init(&sync4C, SHARE_TO_THREADS, 1);
	sem_init(&sync4D, SHARE_TO_THREADS, 1);

	sem_wait(&sync4A);
	sem_wait(&sync4B);
	sem_wait(&sync4C);
	sem_wait(&sync4D);

	task4A = wdm->CreateTask(NULL, &taskTest4A, NULL, false);
	task4B = wdm->CreateTask(NULL, &taskTest4B, NULL, false);

	// Make sure that kickState is false
	TaskState s1(TASK_RUNSTATE_RUNNING, true), s2(TASK_RUNSTATE_RUNNING, true);
	wdm->GetTaskState(task4A, s1);
	wdm->GetTaskState(task4B, s2);

	if (s1.GetKickState() != false || s2.GetKickState() != false)
	{
		ADD_FAILURE()<< "Incorrect kickState: should be false";
		exit(0);
	}

	task4A->start();
	task4B->start();

	sem_post(&sync4A); // All tasks have been started
	sem_post(&sync4B);

	sem_wait(&sync4C);
	sem_wait(&sync4D); // Both threads have kicked

	wdm->GetTaskState(task4A, s1);
	wdm->GetTaskState(task4B, s2);
	if (s1.GetKickState() != true || s2.GetKickState() != true)
	{
		ADD_FAILURE()<< "Incorrect kickState: should be true";
		exit(0);
	}

	pthread_join(task4A->tid, NULL);

	bool deleted = (wdm->DeleteTask(task4A) && wdm->DeleteTask(task4B)) ;
	if (!deleted)
	{
		ADD_FAILURE() << "Failed to delete task(s)";
		exit(0);
	}

	Factory::Destroy(WATCHDOG_MANAGER_SINGLETON);
}
