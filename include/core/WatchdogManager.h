/*! \file WatchdogManager.h
 *  \brief Header File for the WatchdogManager Class
 *
 *  This file contains the definition of the WatchdogManager class, which
 *  allows the system to check that all running tasks are actively running
 *  and not stalled.
 */

#ifndef _WATCHDOGMANAGER_H
#define _WATCHDOGMANAGER_H

#include "core/WatchdogStdTasks.h"
#include "core/WatchdogMapClasses.h"
#include "core/StdTypes.h"
#include "core/Core.h"
#include "core/Singleton.h"
#include "core/Factory.h"
#include "util/Logger.h"
#include "servers/SubsystemServer.h"
#include "errno.h"
#include "POSIX.h"
#include <iostream>
#include <string>
#include <pthread.h>
#include <map>

using namespace std;

namespace AllStar{
namespace Core{

// ------------------------------------ Watchdog Manager Class -------------------------------------

/*  This class provides the ability to check that all tasks in the
 *  running state within the system are actively running.  Each task
 *  MUST call WatchdogManager::Kick() at least once between calls
 *  to AllRunningTasksActive().  Otherwise, that method will return
 *  false, indicating that one or more tasks are no longer active.
 */
class WatchdogManager : public Singleton
{
	friend class Factory;

public:

	// TODO: add pause and resume?

	static bool StartServer(AllStar::Servers::SubsystemServer * instanceIn, int waitIn, bool run);

	static PThread* CreateTask(const pthread_attr_t *at, void *(*ro)(void*), void *ar, const bool startNow);

	static bool DeleteTask(PThread *&pThread);

	static void * WatchdogManagerTask();

	static bool AddTask(PThread * thread);

	static void Kick(void);

	static bool AllRunningTasksActive(void);

	static bool ResetAllKickState(void);

	std::size_t GetNumberOfTasks(void) const;

private:
	typedef std::pair<PThread *, TaskState *> PairType;
	typedef std::map<PThread *, TaskState *>::iterator IteratorType;
	typedef std::map<PThread *, TaskState *>::const_iterator ConstIteratorType;
	std::map<PThread *, TaskState *> taskMap;


	static void Initialize(void);

	bool IsFullyInitialized(void);

	static void Destroy(void);

	virtual ~WatchdogManager(void);

	WatchdogManager(void);

	WatchdogManager(const WatchdogManager & source);

	WatchdogManager & operator=(const WatchdogManager & source);
};

} // End of namespace Core
} // End of namespace AllStar
#endif
