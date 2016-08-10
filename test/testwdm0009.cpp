/*! \file testwdm0009.cpp
 *  \brief Test 9 of the WatchdogManager Class.
 *
 *  This test verifies that the WatchdogManager task correctly runs without
 *  crashing.
 */

#include <iostream>
#include "POSIX.h"
#include "gtest/gtest.h"

#include <iostream>

#include "core/WatchdogManager.h"
using namespace std;
using namespace AllStar::Core;

#ifdef HOST
#define DEBUG_PRINT(m) cout << m << endl
#else
#define DEBUG_PRINT(m)
#endif // HOST

sem_t sync9A, sync9B;
int threadHasBeenRestarted = 0;

static void * taskTest9A(void * parameters)
{

	DEBUG_PRINT("Inside taskTest9A"); // Should see this twice (once after the thread has been restarted)

	if (threadHasBeenRestarted == 1)
	{
		sem_post(&sync9B);

		while(1)
		{
			sleep(1);
		}
	}
	WatchdogManager *wdm = dynamic_cast<WatchdogManager *>(Factory::GetInstance(WATCHDOG_MANAGER_SINGLETON));

	if (wdm == NULL)
	{
		ADD_FAILURE() << "wdm is null";
		pthread_exit(0);
	}

	DEBUG_PRINT("taskTest9A waiting for sync9A");
	sem_wait(&sync9A);

	// Do not kick the wdm for 15 seconds. This will cause the thread to be restarted by the wdm
	// The WatchdogManager task will be running at the same time and will see during its check that
	// this thread is dead.
	DEBUG_PRINT("Thread sleeping forever seconds.");
	threadHasBeenRestarted += 1;

	while(1)
	{
		sleep(1);
	}

	pthread_exit(0);
}

TEST(WatchdogManager, WatchdogManagerTask)
{
	WatchdogManager *wdm = dynamic_cast<WatchdogManager *>(Factory::GetInstance(WATCHDOG_MANAGER_SINGLETON));

	sem_init(&sync9A, SHARE_TO_THREADS, 1);
	sem_init(&sync9B, SHARE_TO_THREADS, 1);
	sem_wait(&sync9A);
	sem_wait(&sync9B);

	PThread * wdmTask = wdm->CreateTask(NULL, &(WatchdogManager::WatchdogManagerTask), NULL, true);
	PThread * task9A = wdm->CreateTask(NULL, &taskTest9A, NULL, true);

	if (!wdm->AllRunningTasksActive())
	{
		ADD_FAILURE() << "Not all running tasks are active";
	}

	sem_post(&sync9A); // hang out for 15 seconds. The other process doesn't kick and should get restarted during this time.
	// Check the console output for confirmation that the thread is getting restarted.
	sem_wait(&sync9B);
	DEBUG_PRINT("Thread done sleeping and posted semaphore.");

	DEBUG_PRINT("Main thread waiting 5 seconds."); // Sleep and make sure that the taskTest thread puts out its print statement again
	sleep(5);

	bool deleted = (wdm->DeleteTask(task9A)) ;
	if (!deleted)
	{
		ADD_FAILURE() << "Failed to delete task(s)";
		exit(0);
	}

	Factory::Destroy(WATCHDOG_MANAGER_SINGLETON);

}
