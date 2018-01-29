/*
 * main.cpp
 *
 *  Created on: Dec 1, 2017
 *      Author: fsw
 */

#include <iostream>
#include <vector>
#include "util/Thread.h"
#include "FSWThreads.h"
#include "core/Watchdog.h"
#include "core/ScheduleManager.h"
#include "subsystem/subsystem.h"
#include "test/testmacros.h"
#include "core/Architecture.h"
using namespace std;




int main() {
	Logger::setMode(MODE_PRINT);
	Logger::setLevel(LEVEL_INFO);
	Logger::registerThread("MAIN");
	Logger::log(LEVEL_FATAL, "Entering Main");

	//---------Step1: Build FSW---------------------------
	Architecture::setInterfaceMode(SOFTWARE);
	Architecture::buildEPS();
	Architecture::buildCOM();
	Architecture::buildACS();
	Architecture::buildRAD();
	Architecture::buildGPS();


	//---------Step2: Initialize HAL-----------------------
	Logger::log(LEVEL_FATAL, "Initializing HAL");
	vector<HardwareManager*> halinit = Architecture::buildHALInitVector();
	for(vector<HardwareManager*>::iterator i = halinit.begin(); i != halinit.end(); i++){
		(*i)->initialize();
	}

	//---------Step3: Initialize Subsystems-----------------------
	vector<SubsystemBase*> subinit = Architecture::buildInitVector();
	Logger::log(LEVEL_FATAL, "Initializing Subsystems");
	for(vector<SubsystemBase*>::iterator i = subinit.begin(); i != subinit.end(); i++){
		(*i)->initialize();
	}

	//---------Step4: Initialize Watchdog-----------------------
	Watchdog watchdog;

	//---------Step5: Initialize HS Thread-----------------------
	Thread hsThread;
	HSStruct hsargs;
	hsargs.SubsystemSequence = Architecture::buildHSVector();
	hsargs.watchdog = &watchdog;
	hsThread.CreateThread(NULL, FSWThreads::GetHealthStatusThread, (void*)&hsargs);
	watchdog.AddThread(hsThread.GetID());

	//---------Step6: Initialize GPS Thread-----------------------
	Thread gpsThread;
	GPSStruct gpsargs;
	gpsargs.gps = Architecture::getGPS();
	gpsargs.watchdog = &watchdog;
	gpsThread.CreateThread(NULL, FSWThreads::GPSManagerThread, (void*)&gpsargs);
	watchdog.AddThread(gpsThread.GetID());


	//---------StepX: Start Watchdog-----------------------
	Thread watchdogThread;
	watchdogThread.CreateThread(NULL, FSWThreads::WatchdogThread, (void*)&watchdog);

	//---------StepX: Join Threads-----------------------
	int rv = 0;
	hsThread.JoinThread((void*)&rv);
	return 0;
}


