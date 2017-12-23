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
}

void * FSWThreads::ModeManagerThread(void * args){
	ModeManagerStruct modeStruct = (ModeManagerStruct*)args;
	Watchdog * watchdog = (Watchdog*)args;

	while(1){
		puts("Mode Manager thread Pinging");
		watchdog->KickWatchdog();



		// call scehdueler

		sleep(2);
	}
}

void * FSWThreads::GPSManagerThread(void * args){
	Watchdog * watchdog = (Watchdog*)args;
	while(1){
		puts("GPS Manager thread Pinging");
		//watchdog->KickWatchdog();
		sleep(2);
	}
}

void * FSWThreads::FileManagerThread(void * args){
	Watchdog * watchdog = (Watchdog*)args;
	while(1){
		puts("File Manager thread Pinging");
		watchdog->KickWatchdog();
		sleep(2);
	}
}


void * FSWThreads::WatchdogThread(void * args){
	Watchdog * watchdog = (Watchdog*) args;
	while(1){
		sleep(4);
		watchdog->CheckThreads();
		puts("Watchdog thread Pinging");

	}
}


