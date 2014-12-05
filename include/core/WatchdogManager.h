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
#include <pthread.h>
#include "errno.h"
#include "POSIX.h"
#include <iostream>

#ifdef HOST
#define DEBUG_PRINT(m) std::cout << m << std::endl
#else
#define DEBUG_PRINT(m)
#endif // HOST

namespace Phoenix
{
namespace Core
{

//typedef union { struct { int hi; int lo; } intId; } pthread_id_np_t;

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

class PThread {
public:
	pthread_t tid;
	const pthread_attr_t *attr;
	void *(*routine)(void*);
	void *arg;
	unsigned int err;
	bool isRunning;
	clock_t startTime;
	clock_t lastKickTime;

    PThread (const pthread_attr_t  *at,
			void *(*start_routine)(void*),
			void *ar,
			bool startNow)
    {
    	tid = 1;
    	attr = at;
    	routine = start_routine;
    	arg = ar;
    	startTime = 0;

    	if (startNow)
    	{
    		start();
    	}
    }

    PThread()
    {
    	tid = 1;
    	attr = NULL;
    	routine = NULL;
    	arg = NULL;
    	startTime = 0;
    	err = 0;
    	isRunning = false;
    	lastKickTime = 0;
    }

    void start()
    {
    	DEBUG_PRINT("At start() in PThread.");
    	tid = 1;
        err = pthread_create(&tid, attr, routine, arg);
        startTime = getTimeInMilis();
        isRunning = true;

        if (err)
        {
        	DEBUG_PRINT("Error when creating thread.");
        	tid = 0;
        	isRunning = false;
        }
    }

    void stop()
    {
    	DEBUG_PRINT("At stop() in PThread.");
    	pthread_cancel(tid);
    	isRunning = false;
    }

    clock_t getTickCount()
    {
    	return (getTimeInMilis() - this->startTime);
    }

//    bool isSuspended;
//    pthread_mutex_t m_SuspendMutex;
//    pthread_cond_t m_ResumeCond;
//
//    void suspend() {
//        pthread_mutex_lock(&m_SuspendMutex);
//        isSuspended = true;
//        do {
//            pthread_cond_wait(&m_ResumeCond, &m_SuspendMutex);
//        } while (isSuspended);
//        pthread_mutex_unlock(&m_SuspendMutex);
//    }
//
//    void resume() {
//        /* The shared state 'isSuspended' must be updated with the mutex held. */
//        pthread_mutex_lock(&m_SuspendMutex);
//        isSuspended = false;
//        pthread_cond_signal(&m_ResumeCond);
//        pthread_mutex_unlock(&m_SuspendMutex);
//    }
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

	/*
	 static void CreateTask(pdTASK_CODE taskFunction,
	 const signed char * const name,
	 unsigned short stackDepth,
	 void *parameters,
	 unsigned long priority,
	 xTaskHandle *taskHandle);
	 */

	static PThread* CreateTask(const pthread_attr_t *at, void *(*ro)(void*), void *ar, const bool startNow);

	/*! \brief Delete a Task from the WatchdogManager. Kill the thread
	 *
	 *  Remove a task from the list of registered tasks in the
	 *  WatchdogManager. Stop the pThread also
	 *
	 *	\param pThraed Pointer to the PThread object
	 *
	 *	\return true if the operation was successful and false
	 *	otherwise.
	 */
	static bool DeleteTask(PThread *&pThread);

	static void * WatchdogManagerTask(void * parameters);

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
	static bool AddTask(PThread *pThread, TaskRunStateEnum taskState);

	/*! \brief Kick the WatchdogManager for a Task
	 *
	 *  Indicate that the current running task is active by kicking
	 *  the WatchdogManager.
	 *
	 *  \note This method takes no parameters, so it must be called
	 *  from the desired task.
	 */
	static void Kick(void);

	/*! \brief Check if All Running Tasks Are Active
	 *
	 *	\return true if all running tasks in the operating system
	 *	are actively running and not stalled.
	 */
	static bool AllRunningTasksActive(void);

	/*! \brief Indicate that a Task is Active
	 *
	 *  \param task Task to be activated.
	 */
	static void ActivateTask(PThread *pThread);

	/*! \brief Indicate that a Task is No Longer Active
	 *
	 *  \param task Task to be deactivated.
	 */
	static void DeactivateTask(PThread *pThread);

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
	static bool GetTaskState(PThread *pThread, TaskState & state);

//	static void ResumeTask(PThread *pThread);

//	static void SuspendTask(PThread *pThread);

private:
	/*! \brief Typedef for Pairs in the Task Map */
	typedef std::pair<PThread *, TaskState *> PairType;

	/*! \brief Typedef for the Iterator Type of the Task Map */
	typedef std::map<PThread *, TaskState *>::iterator IteratorType;

	/*! \brief Typedef for the Constant Iterator Type of the Task Map */
	typedef std::map<PThread *, TaskState *>::const_iterator ConstIteratorType;

	/*! \brief Task Registration Map */
	std::map<PThread *, TaskState *> taskMap;

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
	static void Destroy(void);

	/*! \brief Destructor for WatchdogManager */
	virtual ~WatchdogManager(void);

	/*! Constructor for WatchdogManager */
	WatchdogManager(void);

	/*! Copy Constructor for WatchdogManager */
	WatchdogManager(const WatchdogManager & source);

	/*! Assignment Operator for WatchdogManager */
	WatchdogManager & operator=(const WatchdogManager & source);
};
}
}
#endif
