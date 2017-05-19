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
#include <string>
using namespace std;
using namespace AllStar::Servers;

#define WATCHDOG_MANAGER_DELAY 30000
namespace AllStar{namespace Core{

// -------------------------------------- Necessary Methods --------------------------------------
WatchdogManager::WatchdogManager(void)
: Singleton() {
}

WatchdogManager::WatchdogManager(const WatchdogManager & source) {
}

WatchdogManager::~WatchdogManager(void) {

}

WatchdogManager & WatchdogManager::operator=(const WatchdogManager & source) {
	return *this;
}

void WatchdogManager::Destroy(void) {
	return;
}

void WatchdogManager::Initialize() {
	return;
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
	ModeManager * modeManager = dynamic_cast<ModeManager *> (Factory::GetInstance(MODE_MANAGER_SINGLETON));
	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
	clock_t startTime = getTimeInMillis();
	int64 lastWakeTime = startTime;

	WatchdogManager * watchdogManager = dynamic_cast<WatchdogManager *> (Factory::GetInstance(WATCHDOG_MANAGER_SINGLETON));
	waitUntil(lastWakeTime, WATCHDOG_MANAGER_DELAY);

	while (1) {
		if (modeManager->GetMode() == MODE_RESET) {
			usleep(30000000); // allow 30 seconds for reset
		}

		// check all threads
	}
}

} // End of namespace Core
} // End of namespace AllStar
