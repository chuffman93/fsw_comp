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
#endif // VERBOSE

#include "core/WatchdogManager.h"
#include "core/Factory.h"


using namespace std;

#if VERBOSE
#define PRINT_ACTION(m) cout << m
#else
#define PRINT_ACTION(m)
#endif // VERBOSE

namespace Phoenix
{
    namespace Core
    {
#ifdef WIN32
		xSemaphoreHandle WatchdogManager::staticLockHandle = 0;
		
        void WatchdogManager::Initialize(void)
        {
			staticLockHandle = xSemaphoreCreateMutex( );
// #ifndef WIN32
// 			CREATE_TASK(WatchdogManagerTask,
// 				(const signed char* const)"WatchdogManager",
// 				1500,
// 				NULL,
// 				configMAX_PRIORITIES - 2,
// 				NULL);
// #endif

        }
		
 		void WatchdogManager::Destroy(void )
 		{
 			vQueueDelete(staticLockHandle);
 		}

		bool WatchdogManager::IsFullyInitialized(void)
		{
			return (Singleton::IsFullyInitialized() && (staticLockHandle != 0));
		}
        
        void WatchdogManager::CreateTask(pdTASK_CODE taskFunction,
										 const signed char * const name,
										 unsigned short stackDepth,
										 void *parameters,
										 unsigned long priority,
										 xTaskHandle *taskHandle)
        {
        	xTaskHandle handle, thisTask;
			
			WatchdogManager * watchdogManager = dynamic_cast<WatchdogManager *> (Factory::GetInstance(WATCHDOG_MANAGER_SINGLETON));

        	thisTask = xTaskGetCurrentTaskHandle();

        	if (thisTask != 0)
        	{
        		if (pdTRUE != xSemaphoreTake(staticLockHandle, portMAX_DELAY))
        		{
        			return;
        		}
        	}

			xTaskCreate( taskFunction, name, stackDepth, parameters, priority, &handle);

			if (taskHandle != NULL)
			{
				*taskHandle = handle;
			}

			watchdogManager->ActivateTask(handle);

			if (thisTask != 0)
			{
				xSemaphoreGive(staticLockHandle);
        	}
        }

        void WatchdogManager::DeleteTask(xTaskHandle taskHandle)
        {
        	xTaskHandle thisTask;
			
			WatchdogManager * watchdogManager = dynamic_cast<WatchdogManager *> (Factory::GetInstance(WATCHDOG_MANAGER_SINGLETON));

			thisTask = xTaskGetCurrentTaskHandle();

			if (thisTask != 0)
			{
				if (pdTRUE != xSemaphoreTake(staticLockHandle, portMAX_DELAY))
				{
					return;
				}
			}

        	watchdogManager->RemoveTask(taskHandle);
        	vTaskDelete(taskHandle);

        	if (thisTask != 0)
			{
				xSemaphoreGive(staticLockHandle);
			}
        }

        void WatchdogManager::ResumeTask(xTaskHandle taskHandle)
        {
        	xTaskHandle thisTask;
			
			WatchdogManager * watchdogManager = dynamic_cast<WatchdogManager *> (Factory::GetInstance(WATCHDOG_MANAGER_SINGLETON));

			thisTask = xTaskGetCurrentTaskHandle();

			if (thisTask != 0)
			{
				if (pdTRUE != xSemaphoreTake(staticLockHandle, portMAX_DELAY))
				{
					return;
				}
			}

			watchdogManager->ActivateTask(taskHandle);
			vTaskResume(taskHandle);

			if (thisTask != 0)
			{
				xSemaphoreGive(staticLockHandle);
			}
        }

        void WatchdogManager::SuspendTask(xTaskHandle taskHandle)
        {
        	xTaskHandle thisTask;
			
			WatchdogManager * watchdogManager = dynamic_cast<WatchdogManager *> (Factory::GetInstance(WATCHDOG_MANAGER_SINGLETON));

			thisTask = xTaskGetCurrentTaskHandle();

			if (thisTask != 0)
			{
				if (pdTRUE != xSemaphoreTake(staticLockHandle, portMAX_DELAY))
				{
					return;
				}
			}

			watchdogManager->DeactivateTask(taskHandle);
			vTaskSuspend(taskHandle);

			if (thisTask != 0)
			{
				xSemaphoreGive(staticLockHandle);
			}
        }

        void WatchdogManager::WatchdogManagerTask(void * paramters)
        {
        	portTickType xLastWakeTime;
			const portTickType xFrequency = WATCHDOG_MANAGER_FREQUENCY;
			
			WatchdogManager * watchdogManager = dynamic_cast<WatchdogManager *> (Factory::GetInstance(WATCHDOG_MANAGER_SINGLETON));
			
			vTaskDelay(xFrequency);

			// Initialize the xLastWakeTime variable with the current time.
			xLastWakeTime = xTaskGetTickCount();
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
				}

				if (xLastWakeTime + xFrequency <= xTaskGetTickCount())
				{
					xLastWakeTime = xTaskGetTickCount();
				}
				vTaskDelayUntil( &xLastWakeTime, xFrequency );
			}
        }

        bool WatchdogManager::AddTask(xTaskHandle handle, TaskRunStateEnum taskState)
        {
        	pair<IteratorType, bool> ret;
			
			WatchdogManager * watchdogManager = dynamic_cast<WatchdogManager *> (Factory::GetInstance(WATCHDOG_MANAGER_SINGLETON));
			
			// Get the semaphore.
			if (pdTRUE == watchdogManager->TakeLock(portMAX_DELAY))
			{
				// Insert the registry into the map with serverID as the key.
				TaskState * newState = new TaskState(taskState, true);
				ret = taskMap.insert(PairType(handle, newState));
				if (!ret.second)
				{
					delete newState;
				}
				watchdogManager->GiveLock();
				return ret.second;
			}
			else
			{
				return false;
			}
        }

		bool WatchdogManager::RemoveTask(xTaskHandle task)
		{
			IteratorType it;
			
			WatchdogManager * watchdogManager = dynamic_cast<WatchdogManager *> (Factory::GetInstance(WATCHDOG_MANAGER_SINGLETON));

			// Get the semaphore.
			if (pdTRUE == watchdogManager->TakeLock(portMAX_DELAY))
			{
				it = taskMap.find(task);
				if (it == taskMap.end())
				{
					watchdogManager->GiveLock();
					return false;
				}
				else
				{
					delete it->second;
					taskMap.erase(it);
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
        	xTaskHandle handle;
        	IteratorType it;
			
			WatchdogManager * watchdogManager = dynamic_cast<WatchdogManager *> (Factory::GetInstance(WATCHDOG_MANAGER_SINGLETON));

        	// Get the semaphore.
			if (pdTRUE == watchdogManager->TakeLock(portMAX_DELAY))
			{
				handle = xTaskGetCurrentTaskHandle();
				it = taskMap.find(handle);

				if (taskMap.end( ) == it)
				{
					// The task has not yet been registered, so add it to the
					// map and kick it.
					taskMap.insert(PairType(handle, new TaskState(TASK_RUNSTATE_RUNNING, true)));
				}
				else
				{
					it->second->SetKickState(true);
				}
				watchdogManager->GiveLock();
			}
			else
			{
				return;
			}
        }
        
        bool WatchdogManager::AllRunningTasksActive(void) const
        {
        	bool ret = true;
			
			WatchdogManager * watchdogManager = dynamic_cast<WatchdogManager *> (Factory::GetInstance(WATCHDOG_MANAGER_SINGLETON));

        	// Get the semaphores.
        	if (pdTRUE == xSemaphoreTake(staticLockHandle, portMAX_DELAY))
        	{
				if (pdTRUE == watchdogManager->TakeLock(portMAX_DELAY))
				{
					// Add one to the list of registered tasks for the IDLE task.
					if (uxTaskGetNumberOfTasks() != taskMap.size() + 1)
					{
						watchdogManager->GiveLock();
						xSemaphoreGive(staticLockHandle);
						return false;
					}

					for (ConstIteratorType it = taskMap.begin(); it != taskMap.end(); ++it)
					{
						TaskState * state = it->second;
						if ((xTaskIsTaskSuspended(it->first) && state->GetTaskState() == TASK_RUNSTATE_RUNNING) ||
							(!xTaskIsTaskSuspended(it->first) && state->GetTaskState() == TASK_RUNSTATE_NOT_RUNNING))
						{
							ret = false;
						}

						else if (state->GetTaskState() == TASK_RUNSTATE_RUNNING && !state->GetKickState())
						{
							ret = false;
						}

						it->second->SetKickState(false);
					}

					watchdogManager->GiveLock();
					xSemaphoreGive(staticLockHandle);
					return ret;
				}
				else
				{
					xSemaphoreGive(staticLockHandle);
					return false;
				}
        	}
			else
			{
				return false;
			}
        }
        
        void WatchdogManager::ActivateTask(xTaskHandle task)
        {
        	IteratorType it;
			
			WatchdogManager * watchdogManager = dynamic_cast<WatchdogManager *> (Factory::GetInstance(WATCHDOG_MANAGER_SINGLETON));

        	// Get the semaphore.
			if (pdTRUE == watchdogManager->TakeLock(portMAX_DELAY))
			{
				it = taskMap.find(task);

				if (taskMap.end( ) == it)
				{
					// The task has not yet been registered, so add it to the
					// map.
					taskMap.insert(PairType(task, new TaskState(TASK_RUNSTATE_RUNNING, true)));
				}
				else
				{
					it->second->SetTaskState(TASK_RUNSTATE_RUNNING);
					it->second->SetKickState(true);
				}
				watchdogManager->GiveLock();
			}
			else
			{
				return;
			}
        }
        
        void WatchdogManager::DeactivateTask(xTaskHandle task)
        {
        	IteratorType it;
			
			WatchdogManager * watchdogManager = dynamic_cast<WatchdogManager *> (Factory::GetInstance(WATCHDOG_MANAGER_SINGLETON));

			// Get the semaphore.
			if (pdTRUE == watchdogManager->TakeLock(portMAX_DELAY))
			{
				it = taskMap.find(task);

				if (taskMap.end( ) == it)
				{
					// The task has not yet been registered, so add it to the
					// map.
					taskMap.insert(PairType(task, new TaskState(TASK_RUNSTATE_NOT_RUNNING, true)));
				}
				else
				{
					it->second->SetTaskState(TASK_RUNSTATE_NOT_RUNNING);
					it->second->SetKickState(true);
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
        	return taskMap.size();
        }

        bool WatchdogManager::GetTaskState(xTaskHandle task, TaskState & state) const
        {
        	ConstIteratorType it = taskMap.find(task);

        	if (it == taskMap.end())
        	{
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
#endif
    }
}
