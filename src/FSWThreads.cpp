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
	EPS * eps = hsStruct->eps;
	ScheduleManager * sch = hsStruct->scheduler;
	GPS * gps = hsStruct->gps;
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
		if((sch->getCurrentMode() != Mode_Com) &&
				sch->scheduleEmpty() &&
				(gps->getLockStatus() || !gps->isOn()) &&
				!(FileManager::checkExistance(SCIENCE_MODE) || FileManager::checkExistance(COM_MODE) || FileManager::checkExistance(ADS_MODE))){
			if(!gps->getLockStatus() && !gps->isOn()){
				eps->sendSleepTime(14400);
			}else{
				GPSPositionTime st = gps->pt;
				uint32_t sleepTime = gps->calcSleepTime(st);
				if(sleepTime > 0){
					std::string writeToFile = "COM_MODE!\n";
					std::vector<std::string> buff;
					buff.push_back(writeToFile);
					FileManager::writeToStringFile(COM_MODE,buff);
					eps->sendSleepTime(sleepTime);
				}
			}

		}
	}
	return NULL;
}

void * FSWThreads::ModeThread(void * args) {
	ModeStruct * modeStruct = (ModeStruct*) args;
	Watchdog * watchdog = modeStruct->watchdog;
	EPS* eps = modeStruct->eps;
	std::map<FSWMode, std::vector<SubsystemBase*> > seq = modeStruct->FSWSequence;
	ScheduleManager * scheduler = modeStruct->scheduler;
	FSWMode mode;
	scheduler->checkForSchedule();
	Logger::registerThread("Mode");
	if(FileManager::checkExistance(ADS_MODE) || FileManager::checkExistance(SCIENCE_MODE)){
		sleep(10);
	}
	Logger::log(LEVEL_INFO, "Starting Mode Thread");
	while (1) {
		watchdog->KickWatchdog();
		scheduler->handleScheduling();
		if(!scheduler->ScheduleQueue.empty() &&
				scheduler->currentSchedule.timeSinceEpoch != 0 &&
				scheduler->CurrentMode != Mode_Com){
			eps->sendSleepTime(scheduler->currentSchedule.timeSinceEpoch);
		}
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
	ScheduleManager * scheduler = gpsargs->scheduler;
	gps->sfFile=FileManager::createFileName(GPS_SRT_FNSH);
	std::vector<std::string> buff;
	std::string start = "";
	std::string finish = "";
	stringstream ss;
	Logger::registerThread("GPS");
	Logger::log(LEVEL_INFO, "Starting GPS Thread");
	FSWMode mode;
	while (1) {
		// GPS on, if lock, shut off GPS.
		// Override in PLD (GPS) on
		for(int i = 0; i <= gps->timein; i++){
			// if gps is on, try to get a lock
			if (gps->isOn()){
				Logger::log(LEVEL_INFO,"Fetching GPS");
				gps->fetchNewGPS();
				// check if the lock was a success
				if(i < gps->timeout && !gps->getLockStatus()){
					Logger::log(LEVEL_INFO,"No Lock");
					if(gps->getLockStatus()){
						acs->sendGPS(gps->getBestXYZI());
					}
					watchdog->KickWatchdog();
					sleep(1);
					continue;
				}
				else{
					gps->powerOff();
				}
			}
			watchdog->KickWatchdog();
			acs->sendGPS(gps->getBestXYZI());
			mode = scheduler->getCurrentMode();
			if((mode == Trans_BusToPayload || mode == Mode_Payload) && !(gps->stcl)){
				Logger::log(LEVEL_INFO,"Logging Start GPS Lock");
				start = "Start: ";
				ss << start << gps->getBestXYZI() << "\n";
				buff.push_back(ss.str());
				FileManager::writeToStringFile(gps->sfFile,buff);
				buff.clear();
				ss.str("");
				gps->stcl = true;
			}else if((mode == Trans_PayloadToBus || mode == Mode_Bus) && gps->stcl){
				Logger::log(LEVEL_INFO,"Logging Finish GPS Lock");
				finish = "Finish: ";
				ss << finish << gps->getBestXYZI() << "\n";
				buff.push_back(ss.str());
				FileManager::writeToStringFile(gps->sfFile,buff);
				buff.clear();
				ss.str("");
				gps->stcl = false;
			}
			sleep(1);
		}
		gps->resetLock();
		gps->powerOn();

	}
	return NULL;
}

void * FSWThreads::WatchdogThread(void * args) {
	Watchdog * watchdog = (Watchdog*) args;
	Logger::registerThread("WPUP");
	Logger::log(LEVEL_INFO, "Starting Watchdog Thread");
	while (1) {
		sleep(60);
		watchdog->CheckThreads();
		Logger::Stream(LEVEL_INFO) << "Pinging from watchdog";

	}
	return NULL;
}

void * FSWThreads::GroundThread(void * args) {
	GroundStruct * groundStruct = (GroundStruct*) args;
	Watchdog * watchdog = groundStruct->watchdog;
	GroundCommunication * gnd = groundStruct->gnd;
	gnd->com = groundStruct->com;
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

