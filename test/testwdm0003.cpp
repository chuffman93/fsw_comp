/*! \file testwdm0003.cpp
 *  \brief Test 3 of the WatchdogManager Class.
 *
 *  This test verifies that the AddTask method of the WatchdogManager class
 *  correctly adds a TaskState object to the WatchdogManager when a task
 *  handle that has not yet been added to the manager is added and that the
 *  function returns true.  It also verifies that the AddTask method returns
 *  false if it is called with a task handle that has already been added
 *  to the manager.  RemoveTask is also tested, which should remove a
 *  task from the manager if it has been added and return true, and
 *  otherwise return false.
 */

#include <iostream>
#include "POSIX.h"
#include "gtest/gtest.h"

#include <iostream>

#include "core/WatchdogManager.h"
using namespace std;
using namespace AllStar::Core;

PThread *task3A, *task3B, *task3C;
sem_t sync3A;

static void * taskTest3A(void * parameters)
{
	sem_wait(&sync3A); // don't start this thread until all three have been initialized
	bool ret;

	WatchdogManager * wdm =	dynamic_cast<WatchdogManager *>(Factory::GetInstance(WATCHDOG_MANAGER_SINGLETON));

	if (wdm == NULL)
	{
		ADD_FAILURE() << "wdm is null";
		pthread_exit(0);
	}

	ret = wdm->AddTask(task3B, TASK_RUNSTATE_RUNNING);
	// task3A, 3B, and 3C should be running now
	if (!ret || wdm->GetNumberOfTasks() != 3)
	{
		ADD_FAILURE() << "problem after adding task 3B";
		cout << "number of tasks: " << wdm->GetNumberOfTasks() << endl;
		pthread_exit(0);
	}

	ret = wdm->AddTask(task3C, TASK_RUNSTATE_NOT_RUNNING);

	// task3C is still listed, but not running
	if (!ret || wdm->GetNumberOfTasks() != 3)
	{
		ADD_FAILURE() << "problem after adding task 3C";
		pthread_exit(0);
	}

	ret = wdm->AddTask(task3B, TASK_RUNSTATE_RUNNING);
	if (!ret || wdm->GetNumberOfTasks() != 3)
	{
		ADD_FAILURE() << "problem after re-adding task 3B";
		pthread_exit(0);
	}

	ret = wdm->AddTask(task3B, TASK_RUNSTATE_NOT_RUNNING);
	if (!ret || wdm->GetNumberOfTasks() != 3)
	{
		ADD_FAILURE() << "problem after adding task 3B as not running";
		pthread_exit(0);
	}

	ret = wdm->DeleteTask(task3B);
	if (!ret || wdm->GetNumberOfTasks() != 2)
	{
		ADD_FAILURE() << "problem after deleting task 3B";
		pthread_exit(0);
	}

	pthread_exit(0);

}

void * taskTest3B(void * parameters)
{
	while (1)
	{
		sleep(1);
	}

	pthread_exit(0);
}

TEST(WatchdogManager, AddTask)
{
	WatchdogManager *wdm = dynamic_cast<WatchdogManager *>(Factory::GetInstance(WATCHDOG_MANAGER_SINGLETON));

	// Check that the taskMap starts out with zero tasks
	if (wdm->GetNumberOfTasks() != 0)
	{
		ADD_FAILURE() << "Number of tasks should start at 0 before pThreads are initialized";
		exit(0);
	}

	sem_init(&sync3A, SHARE_TO_THREADS, 1);
	sem_wait(&sync3A);

	task3A = wdm->CreateTask(NULL, &taskTest3A, NULL, true);
	task3B = wdm->CreateTask(NULL, &taskTest3B, NULL, true);
	task3C = wdm->CreateTask(NULL, &taskTest3B, NULL, true);

	sem_post(&sync3A); // All tasks have been started

	pthread_join(task3A->tid, NULL);

	bool deleted = (wdm->DeleteTask(task3A) && wdm->DeleteTask(task3B) && wdm->DeleteTask(task3C));
	if (!deleted)
	{
		ADD_FAILURE() << "Failed to delete task(s)";
		exit(0);
	}

	Factory::Destroy(WATCHDOG_MANAGER_SINGLETON);

}
