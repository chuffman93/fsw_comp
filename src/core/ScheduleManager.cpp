/*
 * ScheduleManager.cpp
 *
 *  Created on: Dec 22, 2017
 *      Author: alex
 */

#include "core/ScheduleManager.h"
#include <iostream>
#include <sstream>
#include <stdio.h>
#include <string>

ScheduleManager::ScheduleManager()
 :CurrentMode(Mode_Bus)
 {
	com.mode = Mode_Com;
	com.duration = 720;
	modeEnterTime = getCurrentTime();
	ComPassCount = FileManager::getComPassCount();
	REBOOT_TIME = 86400;
	com.duration = 0;
	acsDetumble = false;
	handleConfig();
	tags += LogTag("Name", "ScheduleManager");
 }
ScheduleManager::~ScheduleManager(){};

//! Checks for mode changes
FSWMode ScheduleManager::checkNewMode(){
	LockGuard l(lock);
	uint32_t time = getCurrentTime();
	Logger::Stream(LEVEL_DEBUG,tags) << "Check new mode -- Queue Size: " << ScheduleQueue.size() << " With new mode at front: " << ScheduleQueue.front().mode;
	//Logger::Stream(LEVEL_DEBUG,tags) <<"Current Schedule Mode: "<< currentSchedule.mode <<" Current Mode: " << CurrentMode << " Next Scheduled Mode: " << ScheduleQueue.front().mode << " Time: " << time;
	Logger::Stream(LEVEL_DEBUG,tags) << "Current Mode: " << CurrentMode << "Current Schedule.mode(): "<< currentSchedule.mode << " Next Scheduled Mode: " << ScheduleQueue.front().mode << " Time since epoch: " << ScheduleQueue.front().timeSinceEpoch << " Time: " << time << " Enter time: " << modeEnterTime;


	//If schedule is empty and its time for reboot, begin satellite reboot
	if((ScheduleQueue.empty() && time > REBOOT_TIME) ){
		// reset if reboot time
		FSWMode tmp = handleModeChange(Mode_Reset,Mode_Reset);
		CurrentMode = tmp;
		Logger::Stream(LEVEL_INFO, tags) << "Setting Mode to Reset";
	}

	else if (CurrentMode == Mode_Detumble){
			if (!acsDetumble){
				FSWMode tmp = handleModeChange(CurrentMode,Mode_Bus);
				CurrentMode = tmp;
				Logger::Stream(LEVEL_INFO, tags) << "Setting Mode to: " << CurrentMode;
				modeEnterTime = getCurrentTime();
			}
	}
	//If it is time to switch to next scheduled mode and it is not in that mode, switch to next scheduled mode
	else if((currentSchedule.mode!=0) && currentSchedule.timeSinceEpoch <= time && CurrentMode != currentSchedule.mode){
		FSWMode tmp = handleModeChange(CurrentMode,currentSchedule.mode);
		CurrentMode = tmp;
		modeEnterTime = getCurrentTime();
		Logger::Stream(LEVEL_INFO, tags) << "Setting Mode to: " << CurrentMode;
	}
	//If the satellite is not in bus mode and the current mode is not the scheduled mode
	else if(CurrentMode != Mode_Bus && CurrentMode != currentSchedule.mode){
		FSWMode tmp = handleModeChange(CurrentMode,Mode_Bus);
		CurrentMode = tmp;
		Logger::Stream(LEVEL_INFO, tags) << "Setting Mode to Bus";
	}
	//If the scheduled mode duration has surpassed, switch to next scheduled mode
	else if((time - modeEnterTime) >= currentSchedule.duration && CurrentMode != Mode_Bus){
		// exit mode on time
		Logger::Stream(LEVEL_INFO, tags) << "Mode: " << CurrentMode << " Timeout.";
		FSWMode tmp = handleModeChange(CurrentMode,currentSchedule.mode);
		CurrentMode = tmp;
		Logger::Stream(LEVEL_INFO, tags) << "Setting Mode to: " << CurrentMode;
	}
	return CurrentMode;
}

//! Update the schedule sent by ground
void ScheduleManager::handleScheduling(){
	LockGuard l(lock);
	if (FileManager::checkExistance(NEW_SCH)){
		Logger::Stream(LEVEL_INFO,tags) << "Loading New Schedule";
		loadSchedule(NEW_SCH);
	}else if(!ScheduleQueue.empty()){
		Logger::Stream(LEVEL_INFO,tags) << "Schedule Remains Unchanged";
	}else if(CurrentMode == Mode_Bus){
		Logger::Stream(LEVEL_INFO,tags) << "Floating in Bus Mode";
	}

}

/*!
 * If new schedule: adds new schedule to queue
 * \param path to the schedule
 */
void ScheduleManager::loadSchedule(std::string filePath){


	ScheduleStruct sch;

	std::vector<uint8_t> schedule = FileManager::readFromFile(filePath);

	ByteStream bs(schedule);
	uint8_t mode;

	Logger::Stream(LEVEL_DEBUG,tags) << "Schedule size: " << schedule.size();
	//make sure that the file size is divisible by 9
	if ((schedule.size()%9) != 0){
		Logger::Stream(LEVEL_ERROR,tags) << "Unable To Load Next Schedule, incorrect file size";
		return;
	}
	else{
		std::string writeToFile = "1";
		std::vector<std::string> buff;
		buff.push_back(writeToFile);
		//clear contents of the queue
		while(!ScheduleQueue.empty()){
			ScheduleQueue.pop();
		}
		for (size_t i = 0; i < (schedule.size()/9); i++){
			bs >> mode >> sch.timeSinceEpoch >> sch.duration;
			int md = (int)mode;
			char log[100];
			sprintf(log,"Loading Schedule -- Mode: %u Time Since Epoch: %u Duration: %u ",mode,sch.timeSinceEpoch,sch.duration);
			Logger::Stream(LEVEL_DEBUG,tags) << log;
			switch (md){
			case 1:
				sch.mode = Mode_Bus;
				ScheduleQueue.push(sch);
				break;
			case 2:
				sch.mode = Mode_Payload;
				FileManager::writeToStringFile(SCIENCE_MODE,buff);
				ScheduleQueue.push(sch);
				break;
			case 3:
				sch.mode = Mode_Com;
				ScheduleQueue.push(sch);
				break;
			case 4:
				sch.mode = Mode_Reset;
				ScheduleQueue.push(sch);
				break;
			case 5:
				sch.mode = Mode_ADS;
				FileManager::writeToStringFile(ADS_MODE,buff);
				ScheduleQueue.push(sch);
				break;
			default:
				break;
			}
		}
		Logger::Stream(LEVEL_INFO,tags) << "Successfully loaded next schedule";
	}
	if(filePath == NEW_SCH){
		FileManager::deleteFile(filePath);
	}
	Logger::Stream(LEVEL_DEBUG,tags) << "New Queue Size: " << ScheduleQueue.size() << " With new mode up front being: " << ScheduleQueue.front().mode;
}

/*!
 * Handles mode changes
 * \param Current Mode
 * \param Mode switching to
 */
FSWMode ScheduleManager::handleModeChange(FSWMode current, FSWMode next){
	Logger::Stream(LEVEL_DEBUG,tags) << "handleModeChange: current-" << current << " next-" << next;
	switch(current){
		case Trans_BusToPayload:
			ScheduleQueue.pop();
			return Mode_Payload;

		case Trans_PayloadToBus:
			currentSchedule = ScheduleQueue.front();
			return Mode_Bus;

		case Trans_BusToCom:
			FileManager::updateComPassCount();
			return Mode_Com;

		case Trans_ComToBus:
			currentSchedule = ScheduleQueue.front();
			return Mode_Bus;

		case Mode_Payload:
			return Trans_PayloadToBus;

		case Mode_Com:
			return Trans_ComToBus;

		case Mode_Bus:
			if(next == Mode_Payload){
				return Trans_BusToPayload;
			}else if(next == Mode_Com){
				return Trans_BusToCom;
			}else if(next == Mode_Reset){
				return Mode_Reset;
			}
			else{
				//error
				currentSchedule = ScheduleQueue.front();
				return Mode_Bus;
			}

		case Mode_Detumble:
			return Trans_DetumbleToBus;

		case Mode_Reset:
			ScheduleQueue.pop();
			currentSchedule = ScheduleQueue.front();
			return Mode_Reset;

		default:
			return Mode_Bus;
	}

}

/*!
 * Separate function to override schedule to go into com mode
 */
void ScheduleManager::setModeToCom(){
	LockGuard l(lock);
	Logger::Stream(LEVEL_DEBUG,tags) << "Attempting to switch to Com from GroundCommunication Thread";
	com.timeSinceEpoch = getCurrentTime();
	currentSchedule = com;
}

/*!
 * Exits com mode
 */
void ScheduleManager::exitComMode(){
	LockGuard l(lock);
	Logger::Stream(LEVEL_DEBUG,tags) << "Attempting to switch back from Com from GroundCommunication Thread";
	currentSchedule = ScheduleQueue.front();
}

//! returns the mode that we are currently in
FSWMode ScheduleManager::getCurrentMode(){
	return CurrentMode;
}

//! returns the time the mode was entered
uint32_t ScheduleManager::getModeEnterTime(){
	return modeEnterTime;
}

//! returns the com pass count
int ScheduleManager::getComPassCount(){
	return ComPassCount;
}

//! handles the config for ScheduleManager
void ScheduleManager::handleConfig(){
	LockGuard l(lock);
	if(FileManager::checkExistance(SCH_CONFIG)){
		std::vector<uint8_t> buff = FileManager::readFromFile(SCH_CONFIG);
		ByteStream bs(buff);
		bs >> REBOOT_TIME >> com.duration;
		Logger::Stream(LEVEL_INFO,tags) << " Setting reboot_time to " << REBOOT_TIME/3600<< " hrs and com timeout to " << com.duration/60 << " mins";
	}else{
		Logger::Stream(LEVEL_WARN,tags) << "No Schedule Manager configs found";

	}
}

//! handles updating the configs if ground uploads an update
void ScheduleManager::updateConfig(){
	LockGuard l(lock);
	if(FileManager::checkExistance(SCH_CONFIG_UP)){
		std::vector<uint8_t> buff = FileManager::readFromFile(SCH_CONFIG_UP);
		if(buff.size() != CONFIG_SCH_SIZE){
			Logger::Stream(LEVEL_ERROR,tags) << "Incorrect Size for config";
			return;
		}
		ByteStream bs(buff);
		bs >> REBOOT_TIME >> com.duration;
		Logger::Stream(LEVEL_INFO,tags) << " Setting reboot_time to " << REBOOT_TIME/3600<< " hrs and com timeout to " << com.duration/60 << " mins";
		FileManager::moveFile(SCH_CONFIG_UP,SCH_CONFIG);
	}
	else{
		Logger::Stream(LEVEL_WARN,tags) << "There are no SCH config updates";
	}
}

//! returns timeout for com (used to synch with GroundCommunication timeout
uint32_t ScheduleManager::getComTimeout(){
	return com.duration;
}

bool ScheduleManager::scheduleEmpty(){
	LockGuard l(lock);
	if(ScheduleQueue.empty()){
		return true;
	}
	else{
		return false;
	}
}

void ScheduleManager::checkForSchedule(){
	ScheduleStruct sch;
	if(FileManager::checkExistance(SCIENCE_MODE)){
		sch.mode = Mode_Payload;
		sch.duration = 900;
		sch.timeSinceEpoch = 0;
		Logger::Stream(LEVEL_INFO) << "Adding payload mode to schedule";
		ScheduleQueue.push(sch);
		currentSchedule = ScheduleQueue.front();
	}else if(FileManager::checkExistance(ADS_MODE)){
		sch.mode = Mode_ADS;
		sch.duration = 3600;
		sch.timeSinceEpoch = 0;
		Logger::Stream(LEVEL_INFO) << "Adding ADS mode to schedule";
		ScheduleQueue.push(sch);
		currentSchedule = ScheduleQueue.front();
	}else{
		Logger::Stream(LEVEL_INFO) << "No mode added to schedule";
	}
}


