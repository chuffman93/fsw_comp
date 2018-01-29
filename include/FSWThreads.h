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
#include "core/ScheduleManager.h"
#include "subsystem/SubsystemBase.h"
#include "core/FileManager.h"
#include "util/BeaconManager.h"

#include <vector>

struct ModeManagerStruct{
	std::vector<std::vector<SubsystemBase*> >FSWSequence;
	ScheduleManager * scheduler;
	Watchdog * watchdog;
};

struct HSStruct{
	std::vector<SubsystemBase*> SubsystemSequence;
	Watchdog * watchdog;
};

struct GroundCommunicationStruct{
	std::vector<SubsystemBase*> Subsystems;
	Watchdog * watchdog;
};

struct GPSStruct{
	GPS * gps;
	Watchdog * watchdog;
};

class FSWThreads{

public:

	static void * GetHealthStatusThread(void * args);
	static void * ModeManagerThread(void * args);
	static void * GPSManagerThread(void * args);
	static void * FileManagerThread(void * args);
	static void * WatchdogThread(void * args);
	static void * GroundCommunicationThread(void *args);

};


#endif /* INCLUDE_FSWTHREADS_H_ */
