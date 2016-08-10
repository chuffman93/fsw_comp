/*! \file testwdm0008.cpp
 *  \brief Test 8 of the WatchdogManager Class.
 *
 *  This test verifies that the static task manipulation methods of the
 *  WatchdogManager class correctly create, suspend, resume, and destroy
 *  tasks.
 */

#include <iostream>
#include "POSIX.h"
#include "gtest/gtest.h"

#include <iostream>

#include "core/WatchdogManager.h"
using namespace std;
using namespace AllStar::Core;

PThread *task8A, *task8B, *task8C;
sem_t sync8A, sync8B, sync8C;

static void * taskTest8A(void * parameters)
{
	sem_wait(&sync8C);

	TaskState s1(TASK_RUNSTATE_RUNNING, false), s2(TASK_RUNSTATE_RUNNING,
			false), s3(TASK_RUNSTATE_RUNNING, false);

	WatchdogManager *wdm = dynamic_cast<WatchdogManager *>(Factory::GetInstance(WATCHDOG_MANAGER_SINGLETON));

	if (wdm == NULL)
	{
		ADD_FAILURE() << "wdm is null in task8A";
		pthread_exit(0);
	}

	if (wdm->GetNumberOfTasks() != 3)
	{
		ADD_FAILURE() << "Incorrect number of tasks";
		cout << wdm->GetNumberOfTasks() << endl;
		pthread_exit(0);
	}

	if (!wdm->AllRunningTasksActive())
	{
		ADD_FAILURE() << "AllRunningTasksActive reported false (first instance)";
		pthread_exit(0);
	}

	if (!wdm->GetTaskState(task8A, s1) || !wdm->GetTaskState(task8B, s2)
			|| !wdm->GetTaskState(task8C, s3))
	{
		ADD_FAILURE() << "error with GetTaskState";
		pthread_exit(0);
	}

	if (s1.GetKickState() || s2.GetKickState() || s3.GetKickState())
	{
		ADD_FAILURE() << "a kickState is incorrect";
		pthread_exit(0);
	}

	WatchdogManager::DeleteTask(task8C);

	if (wdm->AllRunningTasksActive())
	{
		ADD_FAILURE() << "AllRunningTasksActive reported true after kickStates set to false";
		pthread_exit(0);
	}

	wdm->Kick();

	sem_post(&sync8A);
	sem_wait(&sync8B);

	if (!wdm->AllRunningTasksActive())
	{
		ADD_FAILURE() << "AllRunningTasksActive reported false after all threads kicked";
		pthread_exit(0);
	}

	pthread_exit(0);
}

void * taskTest8B(void * parameters)
{
	WatchdogManager *wdm = dynamic_cast<WatchdogManager *>(Factory::GetInstance(WATCHDOG_MANAGER_SINGLETON));

	if (wdm == NULL)
	{
		ADD_FAILURE() << "wdm is null in task8B";
		pthread_exit(0);
	}

	sem_wait(&sync8A);
	wdm->Kick();
	sem_post(&sync8B);

	while(1)
	{
		sleep(1);
	}

	pthread_exit(0);
}

void * taskTest8C(void * parameters)
{
	WatchdogManager *wdm = dynamic_cast<WatchdogManager *>(Factory::GetInstance(WATCHDOG_MANAGER_SINGLETON));

	if (NULL == wdm)
	{
		ADD_FAILURE() << "wdm is null in task8C";
		pthread_exit(0);
	}

	while(1)
	{
		sleep(1);
	}

	pthread_exit(0);
}

TEST(WatchdogManager, CreateAndDeleteTasks)
{
	WatchdogManager *wdm = dynamic_cast<WatchdogManager *>(Factory::GetInstance(WATCHDOG_MANAGER_SINGLETON));

	sem_init(&sync8A, SHARE_TO_THREADS, 1);
	sem_init(&sync8B, SHARE_TO_THREADS, 1);
	sem_init(&sync8C, SHARE_TO_THREADS, 1);

	sem_wait(&sync8A);
	sem_wait(&sync8B);
	sem_wait(&sync8C);

	task8A = wdm->CreateTask(NULL, &taskTest8A, NULL, true);
	task8B = wdm->CreateTask(NULL, &taskTest8B, NULL, true);
	task8C = wdm->CreateTask(NULL, &taskTest8C, NULL, true);
	sem_post(&sync8C);

	pthread_join(task8A->tid, NULL);

	bool deleted = (wdm->DeleteTask(task8A) && wdm->DeleteTask(task8B) && wdm->DeleteTask(task8C)) ;
	if (!deleted)
	{
		ADD_FAILURE() << "Failed to delete task(s)";
		exit(0);
	}

	Factory::Destroy(WATCHDOG_MANAGER_SINGLETON);
}
