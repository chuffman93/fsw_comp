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
	EPS* eps;
};

struct HSStruct{
	std::vector<SubsystemBase*> subsystemSequence;
	Watchdog * watchdog;
	ScheduleManager * scheduler;
	EPS* eps;
	GPS * gps;
};

struct GroundStruct{
	GroundCommunication * gnd;
	Watchdog * watchdog;
	ScheduleManager * scheduler;
	COM* com;
};

struct GPSStruct{
	GPS * gps;
	Watchdog * watchdog;
	ACS * acs;
	ScheduleManager * scheduler;
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
