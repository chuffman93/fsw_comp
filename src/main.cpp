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
#include "subsystem/ACS.h"
using namespace std;




int main() {
	cout << "!!!Hello World!!!" << endl; // prints !!!Hello World!!!
	Watchdog watchdog;
	ScheduleManager scheduler;

	GPIOManager gpio(""); //fix this
	SPIManager spi("",0,0); //fix this
	ACPInterface acp(gpio,spi,0,0);
	SubPowerInterface subPower(gpio, 0, 0, 0 , "");

	FileManager::updateRebootCount();

	ACS acs(acp, subPower);
	COM com(acp, subPower);
	EPS eps(acp, subPower);
	//GPS gps(acp, subPower);
	RAD rad(acp, subPower);

	ModeManagerStruct modeStruct;
	modeStruct.scheduler = &scheduler;
	modeStruct.watchdog = &watchdog;

	modeStruct.FSWSequence[0].push_back(&acs);
	modeStruct.FSWSequence[1].push_back(&acs);
	modeStruct.FSWSequence[2].push_back(&acs);
	modeStruct.FSWSequence[2].push_back(&com);
	modeStruct.FSWSequence[3].push_back(&acs);
	modeStruct.FSWSequence[3].push_back(&rad);
	modeStruct.FSWSequence[3].push_back(&com);
	modeStruct.FSWSequence[3].push_back(&eps);
	modeStruct.FSWSequence[4].push_back(&acs);
	modeStruct.FSWSequence[4].push_back(&rad);
	modeStruct.FSWSequence[5].push_back(&rad);
	modeStruct.FSWSequence[5].push_back(&acs);
	modeStruct.FSWSequence[6].push_back(&acs);
	modeStruct.FSWSequence[7].push_back(&com);
	modeStruct.FSWSequence[7].push_back(&acs);

	SubsystemSequenceStruct subsystemStruct;
	subsystemStruct.watchdog = &watchdog;
	subsystemStruct.SubsystemSequence[0] = &acs;
	subsystemStruct.SubsystemSequence[1] = &com;
	subsystemStruct.SubsystemSequence[2] = &eps;
	//subsystemStruct.SubsystemSequence[3] = &gps;
	subsystemStruct.SubsystemSequence[4] = &rad;



	// Create Health and Status Thread
	Thread healthStatusThread, watchdogThread, modeManagerThread, gpsManagerThread, groundCommunicationThread;

	healthStatusThread.CreateThread(NULL, FSWThreads::GetHealthStatusThread, (void*)&subsystemStruct);
	modeManagerThread.CreateThread(NULL, FSWThreads::ModeManagerThread, (void*)&modeStruct);
	gpsManagerThread.CreateThread(NULL, FSWThreads::GPSManagerThread, (void*)&watchdog);
	groundCommunicationThread.CreateThread(NULL, FSWThreads::GroundCommunicationThread, (void*)&watchdog);

	watchdog.AddThread(healthStatusThread.GetID());
	watchdog.AddThread(modeManagerThread.GetID());
	watchdog.AddThread(gpsManagerThread.GetID());
	watchdog.AddThread(groundCommunicationThread.GetID());

	watchdogThread.CreateThread(NULL, FSWThreads::WatchdogThread, (void*)&watchdog);

	// Join Threads
	int rv = 0;
	healthStatusThread.JoinThread((void*)&rv);
	watchdogThread.JoinThread((void*)&rv);


	return 0;
}


