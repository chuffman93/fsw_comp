/*
 * Watchdog.h
 *
 *  Created on: Dec 22, 2017
 *      Author: adam
 */

#ifndef INCLUDE_CORE_WATCHDOG_H_
#define INCLUDE_CORE_WATCHDOG_H_


#include "util/Thread.h"
#include <stdlib.h>
#include <stdio.h>
#include <map>
#include "core/Lock.h"
#include "util/Logger.h"
#include "subsystem/EPS.h"

class Watchdog
{
	struct ThreadInfo{
		bool ignore;
		bool alive;
		ThreadInfo(bool ignore) : ignore(ignore){
			alive = false;
		};
	};

public:


	Watchdog(EPS& eps);
	~Watchdog();

	void AddThread(ThreadID id, bool ignore = false);
	void CheckThreads();
	void KickWatchdog();
	void SleepWatchdog();

private:
	std::map<ThreadID,ThreadInfo> threadMap;
	EPS& eps;
	Lock lock;
	LogTags tags;
};





#endif /* INCLUDE_CORE_WATCHDOG_H_ */
