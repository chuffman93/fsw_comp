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

void * FSWThreads::HealthStatusThread(void * args) {
	HSStruct * hsStruct = (HSStruct*) args;
	Watchdog * watchdog = hsStruct->watchdog;
	std::vector<SubsystemBase*> healthSeq = hsStruct->subsystemSequence;
	Logger::registerThread("H&S");
	Logger::log(LEVEL_FATAL, "Starting Health and Status Thread");
	while (1){

		watchdog->KickWatchdog();
		for (std::vector<SubsystemBase*>::iterator i = healthSeq.begin();
				i != healthSeq.end(); i++) {
			(*i)->getHealthStatus();
		}

		for(int i = 0; i <= 60; i++){
			sleep(1);
			watchdog->KickWatchdog();
		}

	}
	return NULL;
}

void * FSWThreads::ModeThread(void * args) {
	ModeStruct * modeStruct = (ModeStruct*) args;
	Watchdog * watchdog = modeStruct->watchdog;
	std::map<FSWMode, std::vector<SubsystemBase*> > seq = modeStruct->FSWSequence;
	ScheduleManager * scheduler = modeStruct->scheduler;
	FSWMode mode;

	Logger::registerThread("Mode");
	Logger::log(LEVEL_FATAL, "Starting Mode Thread");
	while (1) {
		watchdog->KickWatchdog();
		scheduler->handleScheduling();
		mode = scheduler->checkNewMode();
		Logger::Stream(LEVEL_INFO) << "Mode: " << mode;
		map<FSWMode, vector<SubsystemBase*> >::iterator it;
		it = seq.find(mode);

		if(it != seq.end()){
			for(vector<SubsystemBase*>::iterator sub = it->second.begin(); sub != it->second.end(); sub++){
				(*sub)->handleMode(mode);
			}
		}

		sleep(2);
	}
	return NULL;

}

void * FSWThreads::GPSThread(void * args) {
	GPSStruct *gpsargs = (GPSStruct*) args;
	Watchdog * watchdog = gpsargs->watchdog;
	GPS * gps = gpsargs->gps;
	ACS* acs = gpsargs->acs;
	Logger::registerThread("GPS");
	Logger::log(LEVEL_FATAL, "Starting GPS Thread");
	while (1) {
		watchdog->KickWatchdog();
		gps->fetchNewGPS();
		acs->sendGPS(gps->getBestXYZ());
		sleep(2);
	}
	return NULL;
}

void * FSWThreads::WatchdogThread(void * args) {
	Watchdog * watchdog = (Watchdog*) args;
	Logger::registerThread("WPUP");
	Logger::log(LEVEL_FATAL, "Starting Watchdog Thread");
	while (1) {
		sleep(4);
		watchdog->CheckThreads();
		Logger::Stream(LEVEL_INFO) << "Pinging from watchdog";

	}
	return NULL;
}

void * FSWThreads::GroundThread(void * args) {
	GroundStruct * groundStruct = (GroundStruct*) args;
	Watchdog * watchdog = groundStruct->watchdog;
	GroundCommunication * gnd = groundStruct->gnd;
	ScheduleManager * scheduler = groundStruct->scheduler;
	Logger::registerThread("GND");
	Logger::log(LEVEL_FATAL, "Starting Ground Communication Thread");
	while (1) {
		bool toCom = gnd->spinGround();
		if(toCom && scheduler->checkMode() != Mode_Com){
			scheduler->setModeToCom();
		}else if(!toCom && scheduler->checkMode() == Mode_Com){
			scheduler->exitComMode();
		}
		watchdog->KickWatchdog();
		sleep(1);
	}

	return NULL;
}

