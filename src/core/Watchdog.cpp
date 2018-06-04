/*
 * Watchdog.cpp
 *
 *  Created on: Dec 22, 2017
 *      Author: adam
 */


#include "core/Watchdog.h"
#include <assert.h>
#include "util/Thread.h"


Watchdog::Watchdog(EPS& eps)
: eps(eps)
{

}

Watchdog::~Watchdog(){

}

void Watchdog::AddThread(ThreadID id, bool ignore){
	ThreadInfo info(ignore);
	threadMap.insert(std::pair<ThreadID,ThreadInfo>(id, info));
}


void Watchdog::CheckThreads(){
	LockGuard lock_guard(lock);

	for (std::map<ThreadID, ThreadInfo>::iterator iter = threadMap.begin(); iter != threadMap.end(); iter++){

		if (iter->second.ignore == true){
			continue;
		}

		if (iter->second.alive == false){
			Logger::Stream(LEVEL_FATAL,tags) << Logger::getThreadName(iter->first) << " thread is dead!";
			eps.commandReset();
		}

		iter->second.alive = false;
	}

}


void Watchdog::KickWatchdog(){
	LockGuard lock_guard(lock);
	ThreadID id = Thread::GetMyThreadID();
	std::map<ThreadID, ThreadInfo>::iterator iter = threadMap.find(id);
	assert(iter != threadMap.end());
	iter->second.alive = true;
}

void Watchdog::SleepWatchdog(){
	LockGuard lock_guard(lock);
	ThreadID id = Thread::GetMyThreadID();
	std::map<ThreadID, ThreadInfo>::iterator iter = threadMap.find(id);
	assert(iter != threadMap.end());
	iter->second.ignore = true;
}



