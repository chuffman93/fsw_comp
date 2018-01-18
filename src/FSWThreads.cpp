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



void * FSWThreads::GetHealthStatusThread(void * args){
	SubsystemSequenceStruct * subsystemStruct = (SubsystemSequenceStruct*)args;
	Watchdog * watchdog = subsystemStruct->watchdog;
	std::vector<SubsystemBase*> healthSeq = subsystemStruct->SubsystemSequence;
	while(1){
		puts("Health and status thread Pinging");
		watchdog->KickWatchdog();
		for (std::vector<SubsystemBase*>::iterator i = healthSeq.begin(); i != healthSeq.end(); i++){
			(*i)->getHealthStatus();
		}
		sleep(2);
	}
	return NULL;
}

void * FSWThreads::ModeManagerThread(void * args){
	ModeManagerStruct * modeStruct = (ModeManagerStruct*)args;
	Watchdog * watchdog = modeStruct->watchdog;
	std::vector<std::vector<SubsystemBase*> > seq = modeStruct->FSWSequence;
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


void * FSWThreads::WatchdogThread(void * args){
	Watchdog * watchdog = (Watchdog*) args;
	while(1){
		sleep(4);
		watchdog->CheckThreads();
		puts("Watchdog thread Pinging");

	}
	return NULL;
}

void * FSWThreads::GroundCommunicationThread(void * args){
	Watchdog * watchdog = (Watchdog*) args;
	GroundCommunication ground;
	while(1){
		sleep(2);
		watchdog->CheckThreads();
		if (!FileManager::checkExistance(SOT_PATH)){
		}else{
			if (FileManager::checkExistance(IEF_PATH)){
				FileManager::parseIEF();
				ground.downlinkFiles();
				ground.handleScheduling();
				FileManager::parsePPE();
				FileManager::deleteFile(SOT_PATH);
			}

		}
	}
}


