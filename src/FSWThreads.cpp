/*
 * FSWthreads.cpp
 *
 *  Created on: Dec 22, 2017
 *      Author: adam
 */

#include "FSWThreads.h"
#include "core/Watchdog.h"

void * FSWThreads::GetHealthStatusThread(void * args){
	Watchdog * watchdog = (Watchdog*)args;
	while(1){
		puts("Health and status thread Pinging");

		watchdog->KickWatchdog();
		sleep(2);
	}
	return NULL;
}

void * FSWThreads::ModeManagerThread(void * args){
	ModeManagerStruct * modeStruct = (ModeManagerStruct*)args;
	Watchdog * watchdog = modeStruct->watchdog;
	FSWSequence seq = modeStruct->seq;
	ScheduleManager * scheduler = modeStruct->scheduler;

	FSWMode mode;
	while(1){
		puts("Mode Manager thread Pinging");
		watchdog->KickWatchdog();

		mode = scheduler->checkNewMode();
		for (unsigned int i = 0; i < seq.at(mode).size(); i++){
			seq.at(mode).at(i)->handleMode(mode);
		}

		sleep(2);
	}
	return NULL;
}

void * FSWThreads::GPSManagerThread(void * args){
	Watchdog * watchdog = (Watchdog*)args;
	while(1){
		puts("GPS Manager thread Pinging");
		//watchdog->KickWatchdog();
		sleep(2);
	}
	return NULL;
}

void * FSWThreads::FileManagerThread(void * args){
	Watchdog * watchdog = (Watchdog*)args;
	while(1){
		puts("File Manager thread Pinging");
		watchdog->KickWatchdog();
		sleep(2);
	}
	return NULL;
}


void * FSWThreads::WatchdogThread(void * args){
	Watchdog * watchdog = (Watchdog*) args;
	while(1){
		sleep(4);
		watchdog->CheckThreads();
		puts("Watchdog thread Pinging");

	}
	return NULL;
}


