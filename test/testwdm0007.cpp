/*! \file testwdm0007.cpp
 *  \brief Test 7 of the WatchdogManager Class.
 *
 *  This test verifies that the DeactivateTask method adds the given task to
 *  the manager if it has not yet been added, and that the task is tracked
 *  in the not running state and has not been kicked.  It also verifies that
 *  if a task has been added to the manager that it is put in the not running
 *  state and is not kicked.
 */

#include <iostream>
#include "POSIX.h"
#include "gtest/gtest.h"

#include <iostream>

#include "core/WatchdogManager.h"
using namespace std;
using namespace AllStar::Core;

PThread *task7A, *task7B, *task7C;
sem_t sync7A;

static void * taskTest7A(void * parameters)
{
	sem_wait(&sync7A);

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

	TaskState s1(TASK_RUNSTATE_NOT_RUNNING, false), s2(TASK_RUNSTATE_NOT_RUNNING, false);

	wdm->DeactivateTask(task7B); // Deactivate an active task
	wdm->DeactivateTask(task7C); // Deactivate an inactive task

	if (!wdm->GetTaskState(task7B, s1) || !wdm->GetTaskState(task7C, s2))
	{
		ADD_FAILURE() << "failed to get TaskState";
		pthread_exit(0);
	}

	if (s1.GetTaskState() != TASK_RUNSTATE_NOT_RUNNING || s1.GetKickState())
	{
		ADD_FAILURE() << "Wrong state for s1";
		pthread_exit(0);
	}

	if (s2.GetTaskState() != TASK_RUNSTATE_NOT_RUNNING || s2.GetKickState())
	{
		ADD_FAILURE() << "Wrong state for s2";
		pthread_exit(0);
	}

	pthread_exit(0);

	//TODO: If a PThread deactivates itself?

}

void * taskTest7B(void * parameters)
{
	while (1)
	{
		sleep(1);
	}

	pthread_exit(0);
}

TEST(WatchdogManager, DeactivateTask)
{
	WatchdogManager *wdm = dynamic_cast<WatchdogManager *>(Factory::GetInstance(WATCHDOG_MANAGER_SINGLETON));

	sem_init(&sync7A, SHARE_TO_THREADS, 1);
	sem_wait(&sync7A);

	task7A = wdm->CreateTask(NULL, &taskTest7A, NULL, true);
	task7B = wdm->CreateTask(NULL, &taskTest7B, NULL, true);
	task7C = wdm->CreateTask(NULL, &taskTest7B, NULL, false);

	sem_post(&sync7A);
	pthread_join(task7A->tid, NULL);

	bool deleted = (wdm->DeleteTask(task7A) && wdm->DeleteTask(task7B) && wdm->DeleteTask(task7C)) ;
	if (!deleted)
	{
		ADD_FAILURE() << "Failed to delete task(s)";
		exit(0);
	}

	Factory::Destroy(WATCHDOG_MANAGER_SINGLETON);

}
