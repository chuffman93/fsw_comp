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
#include "core/Singleton.h"
#include "core/Factory.h"
#include "util/Logger.h"
#include "servers/SubsystemServer.h"
#include "POSIX.h"
#include <iostream>
#include <pthread.h>
#include <list>

using namespace std;

namespace AllStar{
namespace Core{

// ------------------------------------ Watchdog Manager Class -------------------------------------
class WatchdogManager : public Singleton {
	friend class Factory;

public:
	bool StartServer(AllStar::Servers::SubsystemServer * instanceIn, int waitIn, bool run);

	bool AddTask(PThread * thread);

	bool DeleteTask(PThread *&pThread);

	void WatchdogManagerTask();

	list<PThread *> taskList;

private:
	static void Initialize(void);

	bool IsFullyInitialized(void);

	virtual ~WatchdogManager(void);

	WatchdogManager(void);

	WatchdogManager(const WatchdogManager & source);

	WatchdogManager & operator=(const WatchdogManager & source);
};

} // End of namespace Core
} // End of namespace AllStar
#endif
