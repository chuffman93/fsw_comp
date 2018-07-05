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
#include "DayInTheLife/softwareTest.h"
using namespace std;




int main() {
	Architecture::buildTime();
	FileManager::handleConfig();
	Logger::setMode(MODE_PW);
	Logger::setLevel(LEVEL_INFO);
	Logger::registerThread("MAIN");
	Logger::registerFilter(LogTag("Name", "GPS"), LEVEL_DEBUG);
	Logger::log(LEVEL_INFO, "Entering Main");

	//---------Step1: Build FSW---------------------------
	Architecture::setInterfaceMode(SOFTWARE);
	//Architecture::buildCDH();
	// KEEP ACS OFF
	Architecture::setInterfaceMode(SOFTWARE);
	Architecture::buildACS();
	// KEEP ACS OFF
	Architecture::setInterfaceMode(SOFTWARE);
	Architecture::buildGPS();
	Architecture::buildEPS();
	Architecture::buildRAD();
	Architecture::buildCOM();
	Architecture::buildScheduleManager();
	Architecture::buildBeaconManager();
	Architecture::buildGND();


	//---------Step2: Initialize HAL-----------------------
	Logger::log(LEVEL_INFO, "Initializing HAL");
	vector<HardwareManager*> halinit = Architecture::buildHALInitVector();
	for(vector<HardwareManager*>::iterator i = halinit.begin(); i != halinit.end(); i++){
		(*i)->initialize();
	}


	//---------Step3: Initialize Subsystems-----------------------
	vector<SubsystemBase*> subinit = Architecture::buildInitVector();
	Logger::log(LEVEL_INFO, "Initializing Subsystems");
	for(vector<SubsystemBase*>::iterator i = subinit.begin(); i != subinit.end(); i++){
		(*i)->initialize();
	}
	Architecture::getACS()->sch = Architecture::getSchedulerManager();

	//---------Step4: Initialize Watchdog-----------------------
	Watchdog watchdog(*(Architecture::getEPS()));
	Architecture::getRAD()->watchdog = &watchdog;


	//---------Step5: Initialize HS Thread-----------------------
	Thread hsThread;
	HSStruct hsargs;
	hsargs.subsystemSequence = Architecture::buildHSVector();
	hsargs.watchdog = &watchdog;
	hsThread.CreateThread(NULL, FSWThreads::HealthStatusThread, (void*)&hsargs);
	watchdog.AddThread(hsThread.GetID());

	//---------Step6: Initialize GPS Thread-----------------------
	Thread gpsThread;
	GPSStruct gpsargs;
	gpsargs.gps = Architecture::getGPS();
	gpsargs.watchdog = &watchdog;
	gpsargs.acs = Architecture::getACS();
	gpsargs.scheduler = Architecture::getSchedulerManager();
	gpsThread.CreateThread(NULL, FSWThreads::GPSThread, (void*)&gpsargs);
	watchdog.AddThread(gpsThread.GetID());

	//---------Step7: Initialize Mode Thread-----------------------
	Thread modeThread;
	ModeStruct modeargs;
	modeargs.FSWSequence = Architecture::buildModeSequencing();
	modeargs.scheduler = Architecture::getSchedulerManager();
	modeargs.watchdog = &watchdog;
	modeThread.CreateThread(NULL, FSWThreads::ModeThread, (void*)&modeargs);
	watchdog.AddThread(modeThread.GetID());

	//---------Step8: Initialize GND Thread-----------------------
	Thread gndThread;
	GroundStruct gndargs;
	gndargs.watchdog = &watchdog;
	gndargs.gnd = Architecture::getGND();
	gndargs.scheduler = Architecture::getSchedulerManager();
	gndargs.com = Architecture::getCOM();
	gndThread.CreateThread(NULL, FSWThreads::GroundThread, (void*)&gndargs);
	watchdog.AddThread(gndThread.GetID());

	//---------Step9: Start Watchdog-----------------------
	Thread watchdogThread;
	watchdogThread.CreateThread(NULL, FSWThreads::WatchdogThread, (void*)&watchdog);

	//---------Step10: Join Threads (never reached)-----------------------
	int rv = 0;
	hsThread.JoinThread((void*)&rv);
	gpsThread.JoinThread((void*)&rv);
	modeThread.JoinThread((void*)&rv);
	gndThread.JoinThread((void*)&rv);
	watchdogThread.JoinThread((void*)&rv);
	return 0;
}


