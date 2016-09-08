/*! \file WatchdogManager.cpp
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
#include "util/Logger.h"
#include <string>
using namespace std;
using namespace AllStar::Servers;

#define WATCHDOG_MANAGER_DELAY 5000
namespace AllStar{namespace Core{

// -------------------------------------- Necessary Methods --------------------------------------
WatchdogManager::WatchdogManager(void)
: Singleton(), taskMap( ){
}

WatchdogManager::WatchdogManager(const WatchdogManager & source){
}

WatchdogManager::~WatchdogManager(void){
	IteratorType it;
	while (!taskMap.empty()){
		it = taskMap.begin();
		taskMap.erase(it);
	}
	taskMap.clear();
}

WatchdogManager & WatchdogManager::operator=(const WatchdogManager & source){
	return *this;
}

void WatchdogManager::Destroy(void){
	return;
}

void WatchdogManager::Initialize(){
	return;
}

bool WatchdogManager::IsFullyInitialized(void){
	return (Singleton::IsFullyInitialized());
}

// -------------------------------------- Watchdog Methods ---------------------------------------
bool WatchdogManager::StartServer(SubsystemServer * serverIn, int waitIn, bool run){
	serverIn->startupWait = waitIn;
	printf("here\n");
	if(run){
		PThread * thread = new PThread(serverIn);
		if(!thread->isRunning){
			delete thread;
			return false;
		}

		// Thread created, now add it to the task map
		AddTask(thread);
	}

	return true;
}

bool WatchdogManager::AddTask(PThread * thread){
	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));

	pair<IteratorType, bool> ret;

	WatchdogManager * watchdogManager = dynamic_cast<WatchdogManager *> (Factory::GetInstance(WATCHDOG_MANAGER_SINGLETON));

	// Get the semaphore.
	if (watchdogManager->TakeLock(MAX_BLOCK_TIME) == true){
		TaskState * newState = new TaskState(TASK_RUNSTATE_RUNNING, true);

		IteratorType it;
		it = watchdogManager->taskMap.find(thread);

		if (it == watchdogManager->taskMap.end()){
			// thread not there, add the pair
			ret = watchdogManager->taskMap.insert(PairType(thread, newState));
			if (!ret.second){
				logger->Log("WatchdogManager: failed to insert new task into the map", LOGGER_LEVEL_WARN);
				delete newState;
			}
			logger->Log("WatchdogManager: Inserted new task into the map", LOGGER_LEVEL_INFO);
			watchdogManager->GiveLock();
			return ret.second;
		}else{
			delete it->second;
			it->second = newState;
			watchdogManager->GiveLock();
			return true;
		}
	}else{
		return false;
	}
}

bool WatchdogManager::DeleteTask(PThread *&pThread){
	WatchdogManager * watchdogManager = dynamic_cast<WatchdogManager *> (Factory::GetInstance(WATCHDOG_MANAGER_SINGLETON));

	IteratorType it;

	if (watchdogManager->TakeLock(MAX_BLOCK_TIME) == true){
		if (pThread == NULL){
			watchdogManager->GiveLock();
			return true;
		}

		it = watchdogManager->taskMap.find(pThread);
		if (it != watchdogManager->taskMap.end()){
			if (pThread->isRunning){
				pThread->stop();
			}
			delete pThread;
			pThread = NULL;
			delete it->second;
			watchdogManager->taskMap.erase(it);
		}

		watchdogManager->GiveLock();
		return true;

	}else{
		return false;
	}
}

void * WatchdogManager::WatchdogManagerTask(){
	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
	clock_t startTime = getTimeInMillis();
	clock_t lastWakeTime = startTime;

	WatchdogManager * watchdogManager = dynamic_cast<WatchdogManager *> (Factory::GetInstance(WATCHDOG_MANAGER_SINGLETON));
	waitUntil(lastWakeTime, WATCHDOG_MANAGER_DELAY);

	while (1){
		if (watchdogManager->AllRunningTasksActive()){
			// all running
		}else{
			// Reset inactive tasks
			if (watchdogManager->TakeLock(MAX_BLOCK_TIME) == true){
				for (IteratorType it = watchdogManager->taskMap.begin(); it != watchdogManager->taskMap.end(); ++it){
					if (it->second->GetTaskState() == TASK_RUNSTATE_RUNNING && !it->second->GetKickState()){
						it->first->stop();
						logger->Log("Restarting inactive task", LOGGER_LEVEL_WARN);
						it->first->start();
					}
				}
				watchdogManager->GiveLock();
			}else{
				logger->Log("WatchdogManager: failed to take lock", LOGGER_LEVEL_WARN);
			}
		}
		lastWakeTime = getTimeInMillis();
		waitUntil(lastWakeTime, WATCHDOG_MANAGER_DELAY);
	}

	void * ret;
	return ret;
}

void WatchdogManager::Kick(void){
	WatchdogManager * watchdogManager = dynamic_cast<WatchdogManager *> (Factory::GetInstance(WATCHDOG_MANAGER_SINGLETON));

	// Get the semaphore.
	if (watchdogManager->TakeLock(MAX_BLOCK_TIME) == true){
		pthread_t tid;
		tid = pthread_self();
		printf("\t\t\t\t\tWatchdog was kicked, TID: %lu\n", tid);

		// Find the PThread with the correct tid
		for (ConstIteratorType it = watchdogManager->taskMap.begin(); it != watchdogManager->taskMap.end(); ++it){
			if (it->first->tid == tid){
				it->second->SetTaskState(TASK_RUNSTATE_RUNNING);
				it->second->SetKickState(true);
			}
		}

		watchdogManager->GiveLock();
		return;
	}else{
		return;
	}
}

bool WatchdogManager::AllRunningTasksActive(void){
	WatchdogManager * watchdogManager = dynamic_cast<WatchdogManager *> (Factory::GetInstance(WATCHDOG_MANAGER_SINGLETON));

	if (watchdogManager->TakeLock(MAX_BLOCK_TIME) == true){
		for (ConstIteratorType it = watchdogManager->taskMap.begin(); it != watchdogManager->taskMap.end(); ++it){
			if (it->second->GetTaskState() == TASK_RUNSTATE_RUNNING && !it->second->GetKickState()){
				watchdogManager->GiveLock();
				return false;
			}
			it->second->SetKickState(false);
		}

		watchdogManager->GiveLock();
		return true;
	}else{
		return false;
	}
}

std::size_t WatchdogManager::GetNumberOfTasks(void) const{
	WatchdogManager *watchdogManager = dynamic_cast<WatchdogManager *> (Factory::GetInstance(WATCHDOG_MANAGER_SINGLETON));
	return watchdogManager->taskMap.size();
}

} // End of namespace Core
} // End of namespace AllStar
