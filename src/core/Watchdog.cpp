/*
 * Watchdog.cpp
 *
 *  Created on: Dec 22, 2017
 *      Author: adam
 */


#include "core/Watchdog.h"
#include <assert.h>
#include "util/Thread.h"


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
			printf("Thread %lu is dead!\n", iter->first);
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
