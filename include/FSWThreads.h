/*
 * FSWThreads.h
 *
 *  Created on: Dec 22, 2017
 *      Author: adam
 */

#ifndef INCLUDE_FSWTHREADS_H_
#define INCLUDE_FSWTHREADS_H_

#include <unistd.h>
#include <stdio.h>
#include <queue>
#include "core/Watchdog.h"
#include "core/GroundCommunication.h"
#include "core/ScheduleManager.h"
#include "subsystem/SubsystemBase.h"
#include "core/FileManager.h"
#include "util/BeaconManager.h"

#include <vector>

struct ModeStruct{
	std::map<FSWMode, std::vector<SubsystemBase*> >FSWSequence;
	ScheduleManager * scheduler;
	Watchdog * watchdog;
};

struct HSStruct{
	std::vector<SubsystemBase*> subsystemSequence;
	Watchdog * watchdog;
};

struct GroundStruct{
	GroundCommunication * gnd;
	Watchdog * watchdog;
	ScheduleManager * scheduler;
};

struct GPSStruct{
	GPS * gps;
	Watchdog * watchdog;
};

class FSWThreads{

public:

	static void * HealthStatusThread(void * args);
	static void * ModeThread(void * args);
	static void * GPSThread(void * args);
	static void * WatchdogThread(void * args);
	static void * GroundThread(void *args);

};


#endif /* INCLUDE_FSWTHREADS_H_ */
