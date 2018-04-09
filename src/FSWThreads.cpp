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
#include "core/Architecture.h"
#include "util/TimeKeeper.h"


using namespace std;

void * FSWThreads::HealthStatusThread(void * args) {
	HSStruct * hsStruct = (HSStruct*) args;
	Watchdog * watchdog = hsStruct->watchdog;
	std::vector<SubsystemBase*> healthSeq = hsStruct->subsystemSequence;
	Logger::registerThread("H&S");
	Logger::log(LEVEL_INFO, "Starting Health and Status Thread");
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
	Logger::log(LEVEL_INFO, "Starting Mode Thread");
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
	Logger::log(LEVEL_INFO, "Starting GPS Thread");
	while (1) {
		// GPS on, if lock, shut off GPS.
		// Override in PLD (GPS) on
		for(int i = 0; i <= gps->timein; i++){
			// if gps is on, try to get a lock
			if(gps->isOn()){
				Logger::Stream(LEVEL_INFO) << "Fetching GPS";
				gps->fetchNewGPS();
			}
			// check if the lock was a success
			if(!gps->getSuccess() && gps->isOn() && (i < gps->timeout || !gps->getLockStatus())){
				Logger::Stream(LEVEL_INFO) << "No Lock";
				if(gps->getLockStatus()){
					acs->sendGPS(gps->getBestXYZI());
				}
				watchdog->KickWatchdog();
				sleep(1);
				continue;
			}
			else if(gps->isOn()){
				gps->powerOff();
			}
			watchdog->KickWatchdog();
			acs->sendGPS(gps->getBestXYZI());
			sleep(1);
		}
		gps->powerOn();

	}
	return NULL;
}

void * FSWThreads::WatchdogThread(void * args) {
	Watchdog * watchdog = (Watchdog*) args;
	Logger::registerThread("WPUP");
	Logger::log(LEVEL_INFO, "Starting Watchdog Thread");
	while (1) {
		sleep(20);
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
	Logger::log(LEVEL_INFO, "Starting Ground Communication Thread");
	while (1) {
		if(gnd->ComTimeout != scheduler->getComTimeout()){
			gnd->ComTimeout = scheduler->getComTimeout();
		}
		bool toCom = gnd->spinGround(watchdog);
		if(toCom && scheduler->getCurrentMode() != Mode_Com){
			scheduler->setModeToCom();
		}else if(!toCom && scheduler->getCurrentMode() == Mode_Com){
			scheduler->exitComMode();
			std::vector<SubsystemBase*> seq = Architecture::buildCFVector();
			for (std::vector<SubsystemBase*>::iterator i = seq.begin();
					i != seq.end(); i++) {
				(*i)->updateConfig();
				watchdog->KickWatchdog();
			}
			scheduler->updateConfig();
			FileManager::updateConfig();
		}
		watchdog->KickWatchdog();
		sleep(1);
	}

	return NULL;
}

