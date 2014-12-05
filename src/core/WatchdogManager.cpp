/*! \file WatchdogManager.cpp
 *  \brief Implementation File for the WatchdogManager Class
 *
 *  This file contains the implementation of the WatchdogManager class, which
 *  allows the system to check that all running tasks are actively running
 *  and not stalled.
 */

#define VERBOSE	0

#if VERBOSE
#include <iostream>
#include <pthread.h>
#include <errno.h>
#include <time.h>
#endif // VERBOSE#include "core/WatchdogManager.h"#include "core/Factory.h"
#include "core/Singleton.h"
using namespace std;
using namespace Phoenix::Core;

#define WATCHDOG_MANAGER_DELAY	10000 // milliseconds
#define WATCHDOG_MANAGER_TASK_RESET_TICKS  30000// milliseconds

#if VERBOSE
#define PRINT_ACTION(m) cout << m
#else
#define PRINT_ACTION(m)
#endif // VERBOSE

#ifdef HOST
#define DEBUG_PRINT(m) cout << m << endl
#else
#define DEBUG_PRINT(m)
#endif // HOST
namespace Phoenix{	namespace Core	{

		void WatchdogManager::Destroy(void )
		{
//			vQueueDelete(staticLockHandle);
//			sem_t *staticLockHandle;
			DEBUG_PRINT("In Destroy");
			return;
		}

		void WatchdogManager::Initialize()
		{
			DEBUG_PRINT("In initialize");
			return;
		}

		bool WatchdogManager::IsFullyInitialized(void)
		{
//			static sem_t *staticLockHandle;
			return (Singleton::IsFullyInitialized()); // && (staticLockHandle != 0));
		}

		PThread* WatchdogManager::CreateTask(const pthread_attr_t *at, void *(*ro)(void*), void *ar, const bool startNow)
		{
			PThread *pThread = new PThread(at, ro, ar, startNow);

			if (pThread->tid == 0) // Unsuccessful thread start
			{
				return NULL;
			}

			WatchdogManager * watchdogManager = dynamic_cast<WatchdogManager *> (Factory::GetInstance(WATCHDOG_MANAGER_SINGLETON));

			if (startNow)
			{
				watchdogManager->AddTask(pThread, TASK_RUNSTATE_RUNNING);
			}
			else
			{
				watchdogManager->AddTask(pThread, TASK_RUNSTATE_NOT_RUNNING);
			}

			return pThread;
		}

		bool WatchdogManager::DeleteTask(PThread *&pThread)
		{
			WatchdogManager * watchdogManager = dynamic_cast<WatchdogManager *> (Factory::GetInstance(WATCHDOG_MANAGER_SINGLETON));

			IteratorType it;

			if (watchdogManager->TakeLock(MAX_BLOCK_TIME) == true)
			{
				if (pThread == NULL)
				{
					watchdogManager->GiveLock();
					return true;
				}

				it = watchdogManager->taskMap.find(pThread);
				if (it != watchdogManager->taskMap.end())
				{
					if (pThread->isRunning)
					{
						pThread->stop();
					}
					delete pThread;
					pThread = NULL;
					delete it->second;
					watchdogManager->taskMap.erase(it);
				}

				watchdogManager->GiveLock();
				return true;

			}
			else
			{
				return false;
			}
		}

		void * WatchdogManager::WatchdogManagerTask(void * parameters)
		{
			clock_t startTime = getTimeInMilis();
			clock_t lastWakeTime = startTime;

			WatchdogManager * watchdogManager = dynamic_cast<WatchdogManager *> (Factory::GetInstance(WATCHDOG_MANAGER_SINGLETON));

			waitUntil(lastWakeTime, WATCHDOG_MANAGER_DELAY);

			while (1)
			{
				watchdogManager->Kick();
				if (watchdogManager->AllRunningTasksActive())
				{
					PRINT_ACTION("WDM: All Tasks Actively Running!" << endl);
				}
				else
				{
					PRINT_ACTION("WDM: Not Kicked By All Tasks!" << endl);

					// Reset a task if it has exceeded the time limit
					if (watchdogManager->TakeLock(MAX_BLOCK_TIME) == true)
					{
						watchdogManager->Kick();
						for (IteratorType it = watchdogManager->taskMap.begin(); it != watchdogManager->taskMap.end(); ++it)
						{
							DEBUG_PRINT("In WatchdogManagerTask for-loop");
							TaskState * state = it->second;
							if (state->GetTaskState() == TASK_RUNSTATE_RUNNING && !state->GetKickState())
							{
								// Restart the task
								DEBUG_PRINT("Inactive task found. Restarting thread.");
								it->first->stop();
								DEBUG_PRINT("Stopped the thread.");
								it->first->start();
								DEBUG_PRINT("Restarted the thread.");
							}
						}
					}
					DEBUG_PRINT("WatchdogManagerTask calling GiveLock()");
					bool gaveLock = watchdogManager->GiveLock();
					DEBUG_PRINT("WatchdogManagerTask called GiveLock()");
					cout << "GiveLock return value: " << gaveLock << endl;
				}

				if ((lastWakeTime + WATCHDOG_MANAGER_DELAY) <= (getTimeInMilis() - startTime))
				{
					lastWakeTime = getTimeInMilis();
				}
				waitUntil( lastWakeTime, WATCHDOG_MANAGER_DELAY);
			}

			void * ret;
			return ret;
		}

		bool WatchdogManager::AddTask(PThread *pThread, TaskRunStateEnum taskState)
		{
			pair<IteratorType, bool> ret;

			WatchdogManager * watchdogManager = dynamic_cast<WatchdogManager *> (Factory::GetInstance(WATCHDOG_MANAGER_SINGLETON));

			// Get the semaphore.
			if (watchdogManager->TakeLock(MAX_BLOCK_TIME) == true)
			{
				bool kickState;
				if (taskState == TASK_RUNSTATE_RUNNING)
				{
					kickState = true;
				}
				else
				{
					kickState = false;
				}

				TaskState * newState = new TaskState(taskState, kickState);

				IteratorType it;
				it = watchdogManager->taskMap.find(pThread);

				if (it == watchdogManager->taskMap.end())
				{
					// pThread is not there. Add the pair
					ret = watchdogManager->taskMap.insert(PairType(pThread, newState));
					if (!ret.second)
					{
						DEBUG_PRINT("ret.second is false in AddTask");
						delete newState;
					}
					watchdogManager->GiveLock();
					return ret.second;
				}
				else
				{
					delete it->second;
					it->second = newState;
					watchdogManager->GiveLock();
					return true;
				}
			}
			else
			{
				return false;
			}
		}

		void WatchdogManager::Kick(void)
		{
//			DEBUG_PRINT("Inside Kick method");
			WatchdogManager * watchdogManager = dynamic_cast<WatchdogManager *> (Factory::GetInstance(WATCHDOG_MANAGER_SINGLETON));

			// Get the semaphore.
			if (watchdogManager->TakeLock(MAX_BLOCK_TIME) == true)
			{
				pthread_t tid;
				tid = pthread_self();

				// Find the PThread with the correct tid
				for (ConstIteratorType it = watchdogManager->taskMap.begin(); it != watchdogManager->taskMap.end(); ++it)
				{
					if (it->first->tid == tid)
					{
						it->first->lastKickTime = getTimeInMilis();
						it->second->SetTaskState(TASK_RUNSTATE_RUNNING);
						it->second->SetKickState(true);
					}
				}

				watchdogManager->GiveLock();
				return;
			}
			else
			{
				return;
			}
		}

		bool WatchdogManager::AllRunningTasksActive(void)
		{
			WatchdogManager * watchdogManager = dynamic_cast<WatchdogManager *> (Factory::GetInstance(WATCHDOG_MANAGER_SINGLETON));

			if (watchdogManager->TakeLock(MAX_BLOCK_TIME) == true)
			{
				for (ConstIteratorType it = watchdogManager->taskMap.begin(); it != watchdogManager->taskMap.end(); ++it)
				{
					TaskState * state = it->second;
					if (state->GetTaskState() == TASK_RUNSTATE_RUNNING && !state->GetKickState())
					{
						watchdogManager->GiveLock();
						return false;
					}

					it->second->SetKickState(false);
				}

				watchdogManager->GiveLock();
				return true;
			}
			else
			{
				return false;
			}
		}

		void WatchdogManager::ActivateTask(PThread *pThread)
		{
			IteratorType it;

			WatchdogManager * watchdogManager = dynamic_cast<WatchdogManager *> (Factory::GetInstance(WATCHDOG_MANAGER_SINGLETON));

			if (watchdogManager->TakeLock(MAX_BLOCK_TIME) == true)
			{
				it = watchdogManager->taskMap.find(pThread);

				if (it == watchdogManager->taskMap.end())
				{
					// the task has not yet been registered. Add it to the map
					watchdogManager->taskMap.insert(PairType(pThread, new TaskState(TASK_RUNSTATE_RUNNING, true)));
				}
				else
				{
					it->second->SetTaskState(TASK_RUNSTATE_RUNNING);
					it->second->SetKickState(true);
				}

				// Prevent an inactive task from reporting that it is active
				if (!pThread->isRunning)
				{
					pThread->start();
				}

				watchdogManager->GiveLock();
			}

			return;
		}

		void WatchdogManager::DeactivateTask(PThread *pThread)
		{
			IteratorType it;

			WatchdogManager *watchdogManager = dynamic_cast<WatchdogManager *> (Factory::GetInstance(WATCHDOG_MANAGER_SINGLETON));

			// Get the semaphore
			if (watchdogManager->TakeLock(MAX_BLOCK_TIME) == true)
			{
				it = watchdogManager->taskMap.find(pThread);

				if (it == watchdogManager->taskMap.end())
				{
					// The task has not been registered. Add it
					watchdogManager->taskMap.insert(PairType(pThread, new TaskState(TASK_RUNSTATE_NOT_RUNNING, false)));
				}
				else
				{
					it->second->SetTaskState(TASK_RUNSTATE_NOT_RUNNING);
					it->second->SetKickState(false);
				}

				// Prevent an active task from reporting that it is inactive
				if (pThread->isRunning)
				{
					pThread->stop();
				}

				watchdogManager->GiveLock();
			}
			else
			{
				return;
			}
		}

		std::size_t WatchdogManager::GetNumberOfTasks(void) const
		{
			WatchdogManager *watchdogManager = dynamic_cast<WatchdogManager *> (Factory::GetInstance(WATCHDOG_MANAGER_SINGLETON));
			return watchdogManager->taskMap.size();
		}

		bool WatchdogManager::GetTaskState(PThread *pThread, TaskState & state)
		{
			WatchdogManager *watchdogManager = dynamic_cast<WatchdogManager *> (Factory::GetInstance(WATCHDOG_MANAGER_SINGLETON));
			ConstIteratorType it = watchdogManager->taskMap.find(pThread);

			if (it == watchdogManager->taskMap.end())
			{
				// task does not exist in taskMap
				return false;
			}
			else
			{
				state = *(it->second);
				return true;
			}
		}

		WatchdogManager::WatchdogManager(void)
		: Singleton(), taskMap( )
		{

		}

		WatchdogManager::WatchdogManager(const WatchdogManager & source)
		{

		}

		WatchdogManager::~WatchdogManager(void )
		{
			while (!taskMap.empty())
			{
				IteratorType it = taskMap.begin();
				delete it->second;
				taskMap.erase(it);
			}
			taskMap.clear();
		}

		WatchdogManager & WatchdogManager::operator=(const WatchdogManager & source)
		{
			return *this;
		}

		//		void WatchdogManager::ResumeTask(PThread *pThread)
		//		{
		//			pThread->resume();
		//
		//			WatchdogManager * watchdogManager = dynamic_cast<WatchdogManager *> (Factory::GetInstance(WATCHDOG_MANAGER_SINGLETON));
		//
		//			watchdogManager->ActivateTask(pThread);
		//			watchdogManager->TakeLock(MAX_BLOCK_TIME);
		//
		//			pThread->resume();
		//			watchdogManager->GiveLock();
		//		}

		//		void WatchdogManager::SuspendTask(PThread *pThread)
		//		{
		//			WatchdogManager * watchdogManager = dynamic_cast<WatchdogManager *> (Factory::GetInstance(WATCHDOG_MANAGER_SINGLETON));
		//
		//			if (watchdogManager->TakeLock(MAX_BLOCK_TIME) == false)
		//			{
		//				return;
		//			}
		//
		//			watchdogManager->DeactivateTask(pThread);
		//			watchdogManager->GiveLock();
		//
		//			pThread->suspend();
		//		}
	}
}
