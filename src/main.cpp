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
	Logger::registerThread("MAIN");
	Logger::log(LEVEL_FATAL, "Entering Main");

	//Build FSW
	Architecture::setInterfaceMode(SOFTWARE);
	Architecture::buildEPS();
	Architecture::buildCOM();
	Architecture::buildACS();
	Architecture::buildRAD();


	//Initialize HAL
	Logger::log(LEVEL_FATAL, "Initializing HAL");
	//TODO: This

	//Initialize Subsystems
	vector<SubsystemBase*> subinit = Architecture::buildInitVector();
	Logger::log(LEVEL_FATAL, "Initializing Subsystems");
	for(vector<SubsystemBase*>::iterator i = subinit.begin(); i != subinit.end(); i++){
		(*i)->initialize();
	}

	// Create Health and Status Thread
	Thread healthStatusThread;
	HSStruct hsargs;
	hsargs.SubsystemSequence = Architecture::buildHSVector();
	healthStatusThread.CreateThread(NULL, FSWThreads::GetHealthStatusThread, (void*)&hsargs);

	// Join Threads
	int rv = 0;
	healthStatusThread.JoinThread((void*)&rv);
	return 0;
}


