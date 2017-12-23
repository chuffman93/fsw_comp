/*
 * main.cpp
 *
 *  Created on: Dec 1, 2017
 *      Author: fsw
 */

#include <iostream>
#include "util/Thread.h"
#include "FSWThreads.h"
#include "core/Watchdog.h"
#include "core/ScheduleManager.h"
#include "subsystem/subsystem.h"
#include "test/testmacros.h"
using namespace std;




int main() {
	cout << "!!!Hello World!!!" << endl; // prints !!!Hello World!!!
	Watchdog watchdog;
	ScheduleManager scheduler;
	ACS acs;
	COM com;
	FSWSequence seq;
	VECTOROFDATA(temp, SubsystemBase *, &com,&acs);
	seq.push_back(temp);

	// Create Health and Status Thread
	Thread healthStatusThread, fileManagerThread, watchdogThread, modeManagerThread, gpsManagerThread;

	ModeManagerStruct modeStruct;
	modeStruct.scheduler = &scheduler;
	modeStruct.watchdog = &watchdog;
	modeStruct.seq = seq;

	fileManagerThread.CreateThread(NULL, FSWThreads::FileManagerThread, (void*)&watchdog);
	healthStatusThread.CreateThread(NULL, FSWThreads::GetHealthStatusThread, (void*)&watchdog);
	modeManagerThread.CreateThread(NULL, FSWThreads::ModeManagerThread, (void*)&modeStruct);
	gpsManagerThread.CreateThread(NULL, FSWThreads::GPSManagerThread, (void*)&watchdog);

	watchdog.AddThread(fileManagerThread.GetID());
	watchdog.AddThread(healthStatusThread.GetID());
	watchdog.AddThread(modeManagerThread.GetID());
	watchdog.AddThread(gpsManagerThread.GetID());

	watchdogThread.CreateThread(NULL, FSWThreads::WatchdogThread, (void*)&watchdog);




	// Join Threads
	int rv = 0;
	healthStatusThread.JoinThread((void*)&rv);
	fileManagerThread.JoinThread((void*)&rv);
	watchdogThread.JoinThread((void*)&rv);


	return 0;
}


