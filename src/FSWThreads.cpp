/*
 * FSWthreads.cpp
 *
 *  Created on: Dec 22, 2017
 *      Author: adam
 */

#include "FSWThreads.h"
#include "subsystem/COM.h"
#include "core/ScheduleManager.h"
#include "core/GroundCommunication.h"
#include "util/TimeKeeper.h"

using namespace std;

void * FSWThreads::GetHealthStatusThread(void * args) {
	HSStruct * hsStruct = (HSStruct*) args;
	Watchdog * watchdog = hsStruct->watchdog;
	std::vector<SubsystemBase*> healthSeq = hsStruct->SubsystemSequence;
	Logger::registerThread("H&S");
	Logger::log(LEVEL_FATAL, "Starting Health and Status Thread");
	while (1) {

		//watchdog->KickWatchdog();
		for (std::vector<SubsystemBase*>::iterator i = healthSeq.begin();
				i != healthSeq.end(); i++) {
			(*i)->getHealthStatus();
		}
		sleep(15);
	}
	return NULL;
}

void * FSWThreads::ModeManagerThread(void * args) {
	ModeManagerStruct * modeStruct = (ModeManagerStruct*) args;
	Watchdog * watchdog = modeStruct->watchdog;
	std::vector<std::vector<SubsystemBase*> > seq = modeStruct->FSWSequence;
	ScheduleManager * scheduler = modeStruct->scheduler;

	FSWMode mode;
	while (1) {
		puts("Mode Manager thread Pinging");
		watchdog->KickWatchdog();
		mode = scheduler->checkNewMode();
		for (unsigned int i = 0; i < seq.at(mode).size(); i++) {
			seq.at(mode).at(i)->handleMode(mode);
		}

		sleep(2);
	}
	return NULL;

}

void * FSWThreads::GPSManagerThread(void * args) {
	Watchdog * watchdog = (Watchdog*) args;
	while (1) {
		puts("GPS Manager thread Pinging");
		//watchdog->KickWatchdog();
		sleep(2);
	}
	return NULL;
}

void * FSWThreads::WatchdogThread(void * args) {
	Watchdog * watchdog = (Watchdog*) args;
	while (1) {
		sleep(4);
		watchdog->CheckThreads();
		puts("Watchdog thread Pinging");

	}
	return NULL;
}

void * FSWThreads::GroundCommunicationThread(void * args) {
	GroundCommunicationStruct * groundStruct = (GroundCommunicationStruct*) args;
	Watchdog * watchdog = groundStruct->watchdog;
	GroundCommunication ground(groundStruct->Subsystems);
	while (1) {
		sleep(2);
		watchdog->CheckThreads();
	}

	return NULL;
}

