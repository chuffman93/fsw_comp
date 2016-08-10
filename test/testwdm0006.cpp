/*! \file testwdm0006.cpp
 *  \brief Test 6 of the WatchdogManager Class.
 *
 *  This test verifies that the ActivateTask method adds the given task to
 *  the manager if it has not yet been added, and that the task is tracked
 *  in the running state and has been kicked.  It also verifies that if a
 *  task has been added to the manager that it is put in the running state
 *  and is kicked.
 */

#include <iostream>
#include "POSIX.h"
#include "gtest/gtest.h"

#include <iostream>

#include "core/WatchdogManager.h"
using namespace std;
using namespace AllStar::Core;

PThread *task6A, *task6B, *task6C;
sem_t sync6A;

static void * taskTest6A(void * parameters)
{
	sem_wait(&sync6A);

	WatchdogManager *wdm = dynamic_cast<WatchdogManager *>(Factory::GetInstance(WATCHDOG_MANAGER_SINGLETON));

	if (wdm == NULL)
	{
		ADD_FAILURE() << "wdm is null";
		pthread_exit(0);
	}

	if (!wdm->AllRunningTasksActive())
	{
		ADD_FAILURE() << "All tasks should be active";
		exit(0);
	}
	wdm->Kick();

	TaskState s1(TASK_RUNSTATE_NOT_RUNNING, false), s2(TASK_RUNSTATE_NOT_RUNNING, false);

	wdm->ActivateTask(task6B); // Activate an active task
	wdm->ActivateTask(task6C); // Activate an inactive task

	if (!wdm->GetTaskState(task6B, s1) || !wdm->GetTaskState(task6C, s2))
	{
		ADD_FAILURE() << "failed to get TaskState";
		pthread_exit(0);
	}

	if (s1.GetTaskState() != TASK_RUNSTATE_RUNNING || !s1.GetKickState())
	{
		ADD_FAILURE() << "Wrong state for s1";
		pthread_exit(0);
	}

	if (s2.GetTaskState() != TASK_RUNSTATE_RUNNING || !s2.GetKickState())
	{
		ADD_FAILURE() << "Wrong state for s2";
		pthread_exit(0);
	}

	if (!wdm->AllRunningTasksActive())
	{
		ADD_FAILURE() << "All running tasks should be active";
		pthread_exit(0);
	}

	pthread_exit(0);
}

void * taskTest6B(void * parameters)
{
	while (1)
	{
		sleep(1);
	}

	pthread_exit(0);
}

TEST(WatchdogManager, ActivateTask)
{
	WatchdogManager *wdm = dynamic_cast<WatchdogManager *>(Factory::GetInstance(WATCHDOG_MANAGER_SINGLETON));

	sem_init(&sync6A, SHARE_TO_THREADS, 1);
	sem_wait(&sync6A);

	task6A = wdm->CreateTask(NULL, &taskTest6A, NULL, true);
	task6B = wdm->CreateTask(NULL, &taskTest6B, NULL, true);
	task6C = wdm->CreateTask(NULL, &taskTest6B, NULL, false);
	sem_post(&sync6A);

	pthread_join(task6A->tid, NULL);

	bool deleted = (wdm->DeleteTask(task6A) && wdm->DeleteTask(task6B) && wdm->DeleteTask(task6C)) ;
	if (!deleted)
	{
		ADD_FAILURE() << "Failed to delete task(s)";
		exit(0);
	}

	Factory::Destroy(WATCHDOG_MANAGER_SINGLETON);

}
