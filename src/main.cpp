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

	FileManager fm;
	fm.updateRebootCount();

	ACS acs;
	COM com;
	EPS eps;
	RAD rad;

	ModeManagerStruct modeStruct;
	modeStruct.scheduler = &scheduler;
	modeStruct.watchdog = &watchdog;
	modeStruct.FSWSequence[0] = {&acs};
	modeStruct.FSWSequence[1] = {&acs};
	modeStruct.FSWSequence[2] = {&acs,&com};
	modeStruct.FSWSequence[3] = {&acs,&rad,&com,&eps};
	modeStruct.FSWSequence[4] = {&acs,&rad};
	modeStruct.FSWSequence[5] = {&rad,&acs};
	modeStruct.FSWSequence[6] = {&acs};
	modeStruct.FSWSequence[7] = {&com,&acs};

	VECTOROFDATA(temp, SubsystemBase *, &com,&acs);
	//seq.push_back(temp);

	// Create Health and Status Thread
	Thread healthStatusThread, watchdogThread, modeManagerThread, gpsManagerThread;

	healthStatusThread.CreateThread(NULL, FSWThreads::GetHealthStatusThread, (void*)&watchdog);
	modeManagerThread.CreateThread(NULL, FSWThreads::ModeManagerThread, (void*)&modeStruct);
	gpsManagerThread.CreateThread(NULL, FSWThreads::GPSManagerThread, (void*)&watchdog);

	watchdog.AddThread(healthStatusThread.GetID());
	watchdog.AddThread(modeManagerThread.GetID());
	watchdog.AddThread(gpsManagerThread.GetID());

	watchdogThread.CreateThread(NULL, FSWThreads::WatchdogThread, (void*)&watchdog);

	// Join Threads
	int rv = 0;
	healthStatusThread.JoinThread((void*)&rv);
	watchdogThread.JoinThread((void*)&rv);


	return 0;
}


