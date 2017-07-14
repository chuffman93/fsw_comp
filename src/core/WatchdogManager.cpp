/*! \file WatchdogManager.cpp
 *
 *  Last Modified: Jack Dinkel 12/16/16
 *
 *  \brief Implementation File for the WatchdogManager Class
 *
 *  This file contains the implementation of the WatchdogManager class, which
 *  allows the system to check that all running tasks are actively running
 *  and not stalled.
 */
#include "core/WatchdogManager.h"
#include "core/WatchdogMapClasses.h"
#include "core/WatchdogStdTasks.h"#include "core/Factory.h"
#include "core/Singleton.h"
#include "servers/SubsystemServer.h"
#include "servers/FMGServer.h"
#include "util/Logger.h"
#include "util/TLM.h"
#include <string>
using namespace std;
using namespace AllStar::Servers;

#define WATCHDOG_MANAGER_DELAY			5000  // in millis
#define WATCHDOG_MANAGER_START_DELAY	30000 // in millis
namespace AllStar{namespace Core{

// -------------------------------------- Necessary Methods --------------------------------------
WatchdogManager::WatchdogManager(void)
: Singleton() {
}

WatchdogManager::WatchdogManager(const WatchdogManager & source) { }

WatchdogManager::~WatchdogManager(void) { }

WatchdogManager & WatchdogManager::operator=(const WatchdogManager & source) {
	return *this;
}

bool WatchdogManager::IsFullyInitialized(void) {
	return (Singleton::IsFullyInitialized());
}

// -------------------------------------- Watchdog Methods ---------------------------------------
bool WatchdogManager::StartServer(SubsystemServer * serverIn, int waitIn, bool run) {
	serverIn->startupWait = waitIn;
	if (run) {
		PThread * thread = new PThread(serverIn);
		if (!thread->isRunning) {
			delete thread;
			return false;
		}

		// Thread created, now add it to the task list
		taskList.push_back(thread);
	}

	return true;
}

void WatchdogManager::WatchdogManagerTask() {
	ModeManager * modeManager = static_cast<ModeManager *> (Factory::GetInstance(MODE_MANAGER_SINGLETON));
	Logger * logger = static_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
	int64 lastWakeTime = 0;

	usleep(WATCHDOG_MANAGER_START_DELAY * 1000);

	while (1) {
		lastWakeTime = getTimeInMillis();

		if (modeManager->GetMode() == MODE_RESET) {
			usleep(60000000); // allow 60 seconds for reset
		}

		// check all threads
		list<PThread *>::iterator thread;
		for (thread = taskList.begin(); thread != taskList.end(); thread++) {
			switch ((*thread)->server->getWDMState()) {
			case WDM_UNKNOWN:
				(*thread)->stop();
				logger->Error("\x1b[31m" "Restarting inactive task" "\x1b[0m");
				TLM_RESTART_THREAD((*thread)->server->id);
				(*thread)->start();
				break;
			case WDM_ASLEEP:
				// nothing to do
				break;
			case WDM_ALIVE:
				(*thread)->server->wdmUnknown();
				break;
			default:
				logger->Error("WatchdogManager: unknown server state for %s!", (*thread)->serverName.c_str());
				break;
			}
		}

		waitUntil(lastWakeTime, WATCHDOG_MANAGER_DELAY);
	}
}

} // End of namespace Core
} // End of namespace AllStar
