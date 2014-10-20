/*! \file WatchdogManager.h
 *  \brief Header File for the WatchdogManager Class
 *
 *  This file contains the definition of the WatchdogManager class, which
 *  allows the system to check that all running tasks are actively running
 *  and not stalled.
 */

#ifndef _WATCHDOGMANAGER_H
#define _WATCHDOGMANAGER_H

#include <map>

#include "core/StdTypes.h"
#include "core/Core.h"
#include "core/Singleton.h"
#include "core/Factory.h"

namespace Phoenix
{
    namespace Core
    {
#ifdef WIN32
		#define WATCHDOG_MANAGER_FREQUENCY	10000
		/*! \brief Task Run State Enumeration
		 *
		 *  Enumerates the different run states that a task can have as
		 *  viewed by the WatchdogManager class.
		 */
		enum TaskRunStateEnum
		{
			/*! \brief Task Running */
			TASK_RUNSTATE_RUNNING = 0,
			/*! \brief Task Not Running */
			TASK_RUNSTATE_NOT_RUNNING,
			/*! \brief Task RunState Max */
			TASK_RUNSTATE_MAX
		};

		/*! \brief Class that Encapsulates the State of a Task */
		class TaskState
		{
		public:
			/*! \brief Constructor for TaskState
			 *
			 *  \param taskStateIn Run state of the task.
			 *  \param kickStateIn Kick state of the task.
			 */
			TaskState(TaskRunStateEnum taskStateIn, bool kickStateIn = false)
			         : taskState(taskStateIn), kickState(kickStateIn)
		    {
				
		    }

			/*! \brief Return the Task Kick State
			 *
			 *  \return Task kick state.
			 */
		    bool GetKickState() const
		    {
		        return kickState;
		    }

		    /*! \brief Return the Task Run State
			 *
			 *  \return Task run state.
			 */
		    TaskRunStateEnum GetTaskState() const
		    {
		        return taskState;
		    }

		    /*! \brief Set the Task Kick State
		     *
		     *  \param kickState New kick state for the task.
		     */
		    void SetKickState(bool kickState)
			{
				this->kickState = kickState;
			}

		    /*! \brief Set the Task Run State
			 *
			 *  \param runState New run state for the task.
			 */
			void SetTaskState(TaskRunStateEnum taskState)
			{
				this->taskState = taskState;
			}

		private:
			/* \brief Task Run State */
			TaskRunStateEnum taskState;

			/*! \brief Task Kick State */
		    bool kickState;
		};

		/*! \brief Class that Monitors Running Tasks for Activity
		 *
		 *  This class provides the ability to check that all tasks in the
		 *  running state within the system are actively running.  Each task
		 *  MUST call WatchdogManager::Kick() at least once between calls
		 *  to AllRunningTasksActive().  Otherwise, that method will return
		 *  false, indicating that one or more tasks are no longer active.
		 */
        class WatchdogManager : public Singleton
        {
			/*! \brief Declare Factory a friend class 
			*	
			*	This allows factory to call WatchdogManager's private constructor
			*/
			friend class Factory;
			
        public:

            static void CreateTask(pdTASK_CODE taskFunction,
            		               const signed char * const name,
            		               unsigned short stackDepth,
            		               void *parameters,
            		               unsigned long priority,
            		               xTaskHandle *taskHandle);

            static void DeleteTask(xTaskHandle taskHandle);

            static void ResumeTask(xTaskHandle taskHandle);

            static void SuspendTask(xTaskHandle taskHandle);

            static void WatchdogManagerTask(void * paramters);

            /*! \brief Add a Task to the WatchdogManager
             *
             *  Add a task to the list of registered tasks in the
             *  WatchdogManager.
             *
             *	\param handle Handle to the task.
             *	\param taskState Run state of the task.
             *
             *	\return true if the operation was successful and false
             *	otherwise.
             */
            bool AddTask(xTaskHandle handle, TaskRunStateEnum taskState);

            /*! \brief Remove a Task from the WatchdogManager
			 *
			 *  Remove a task from the list of registered tasks in the
			 *  WatchdogManager.
			 *
			 *	\param handle Handle to the task.
			 *
			 *	\return true if the operation was successful and false
			 *	otherwise.
			 */
            bool RemoveTask(xTaskHandle handle);

            /*! \brief Kick the WatchdogManager for a Task
             *
             *  Indicate that the current running task is active by kicking
             *  the WatchdogManager.
             *
             *  \note This method takes no parameters, so it must be called
             *  from the desired task.
             */
            void Kick(void);

            /*! \brief Check if All Running Tasks Are Active
             *
             *	\return true if all running tasks in the operating system
             *	are actively running and not stalled.
             */
            bool AllRunningTasksActive(void) const;

            /*! \brief Indicate that a Task is Active
             *
             *  \param task Task to be activated.
             */
            void ActivateTask(xTaskHandle task);

            /*! \brief Indicate that a Task is No Longer Active
			 *
			 *  \param task Task to be deactivated.
			 */
            void DeactivateTask(xTaskHandle task);

            /*! \brief Return the Number of Registered Tasks
             *
             *  \return Number of registered tasks.
             */
            std::size_t GetNumberOfTasks(void) const;

            /*! \brief Return the State of a Task
             *
             *  \param task Task to retrieve state for.
             *  \param state Set to the state of the task if the return value
             *  is true.
             *
             *  \return true if the operation was successful and false
             *  otherwise.
             */
            bool GetTaskState(xTaskHandle task, TaskState & state) const;

        private:
            /*! \brief Typedef for Pairs in the Task Map */
            typedef std::pair<xTaskHandle, TaskState *> PairType;

            /*! \brief Typedef for the Iterator Type of the Task Map */
            typedef std::map<xTaskHandle, TaskState *>::iterator IteratorType;

            /*! \brief Typedef for the Constant Iterator Type of the Task Map */
            typedef std::map<xTaskHandle, TaskState *>::const_iterator ConstIteratorType;

            static xSemaphoreHandle staticLockHandle;

            /*! \brief Task Registration Map */
            std::map<xTaskHandle, TaskState *> taskMap;
			
			/*! \brief Initialize the WatchdogManager Class
			 *
			 *  Initializes the operating system constructs needed for
			 *  WatchdogManager to work properly.
			 */
        	static void Initialize(void);
			
			/*! \brief Checks if WatchdogManager Class is initialized
             *
             *  \return true if the initialization was successful and
             *  false otherwise.
             */
			bool IsFullyInitialized(void);
			
			/*! Destroy the WatchdogManager
             *
             *  Frees all memory and operating system constructs used by
             *  WatchdogManager.  This method is intended primarily for use
             *  in unit tests during tear down.
             */
            static void Destroy(void );

            /*! \brief Destructor for WatchdogManager */
            virtual ~WatchdogManager(void);

            /*! Constructor for WatchdogManager */
            WatchdogManager(void);

            /*! Copy Constructor for WatchdogManager */
            WatchdogManager(const WatchdogManager & source);

            /*! Assignment Operator for WatchdogManager */
            WatchdogManager & operator=(const WatchdogManager & source);
        };
#endif
    }
}

#endif  //_WATCHDOGMANAGER_H
