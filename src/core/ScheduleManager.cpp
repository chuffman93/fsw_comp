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
 : CurrentMode(Mode_Bus)
 {
	com.duration = 720;
	com.mode = Mode_Com;
	CurrentMode = Mode_Bus;
	modeEnterTime = getCurrentTime();
	tags += LogTag("Name", "ScheduleManager");
 }
ScheduleManager::~ScheduleManager(){};

//check for mode changes
FSWMode ScheduleManager::checkNewMode(){
	LockGuard l(lock);
	//get current time
	uint32_t time = getCurrentTime();
	//check if it is time to switch modes
	Logger::Stream(LEVEL_DEBUG,tags) << "Check new mode -- Queue Size: " << ScheduleQueue.size() << " With new mode at front: " << ScheduleQueue.front().mode;
	Logger::Stream(LEVEL_DEBUG,tags) << "Current Mode:: " << CurrentMode << " Next Scheduled Mode: " << ScheduleQueue.front().mode << " Time: " << time;
	if (!ScheduleQueue.empty() && currentSchedule.timeSinceEpoch <= time && CurrentMode != currentSchedule.mode && ScheduleQueue.front().mode != 0 && (CurrentMode != Trans_PayloadToBus) && (CurrentMode != Trans_ComToBus)){
		FSWMode tmp = handleModeChange(CurrentMode,currentSchedule.mode);
		CurrentMode = tmp;
		modeEnterTime = getCurrentTime();
		Logger::Stream(LEVEL_INFO, tags) << "Setting Mode to: " << CurrentMode;
	//check if the mode time has exceeded its duration, if so go into proper trans mode
	}else if(((time - modeEnterTime) >= currentSchedule.duration) && CurrentMode != Mode_Bus && (CurrentMode != Trans_PayloadToBus) && (CurrentMode != Trans_ComToBus)){
		FSWMode tmp = handleModeChange(CurrentMode,currentSchedule.mode);
		CurrentMode = tmp;
		Logger::Stream(LEVEL_INFO, tags) << "Setting Mode: " << CurrentMode;
	//Transfer back to bus
	}else if (CurrentMode == Trans_PayloadToBus || CurrentMode == Trans_ComToBus){
		currentSchedule = ScheduleQueue.front();
		FSWMode tmp = handleModeChange(CurrentMode,Mode_Bus);
		CurrentMode = tmp;
		Logger::Stream(LEVEL_INFO, tags) << "Setting Mode to Bus";
	//check if it is time for reboot if the schedule is empty
	}else if (ScheduleQueue.empty() && time > REBOOT_TIME){
		FSWMode tmp = handleModeChange(Mode_Reset,Mode_Reset);
		CurrentMode = tmp;
		Logger::Stream(LEVEL_INFO, tags) << "Setting Mode to Reset";
	}
	return CurrentMode;
}

void ScheduleManager::handleScheduling(){
	LockGuard l(lock);
	if (FileManager::checkExistance(NEW_DEFAULT_SCH)){
		Logger::Stream(LEVEL_INFO,tags) << "Updating Default Schedule";
		FileManager::moveFile(NEW_DEFAULT_SCH, DEFAULT_SCH);
	}
	if (FileManager::checkExistance(NEW_SCH)){
		Logger::Stream(LEVEL_INFO,tags) << "Loading New Schedule";
		loadSchedule(NEW_SCH);
	}else if(!ScheduleQueue.empty()){
		Logger::Stream(LEVEL_INFO,tags) << "Schedule Remains Unchanged";
	}else if(CurrentMode == Mode_Bus){
		Logger::Stream(LEVEL_INFO,tags) << "Loading Default Schedule";
		loadSchedule(DEFAULT_SCH);
	}

}

//if new schedule: adds new schedule to queue, otherwise add default schedule to queue if empty
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
	else if(filePath == DEFAULT_SCH){
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
				default:
					break;
				}
			}
			Logger::Stream(LEVEL_INFO,tags) << "Successfully loaded default schedule";
	}
	else{
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

FSWMode ScheduleManager::handleModeChange(FSWMode current, FSWMode next){
	switch(current){
		case Trans_BusToPayload:
			ScheduleQueue.pop();
			return Mode_Payload;
		case Trans_PayloadToBus:
			return Mode_Bus;
		case Trans_BusToCom:
			return Mode_Com;
		case Trans_ComToBus:
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
			}else{
				//error
				return Mode_Bus;
			}
		case Mode_Reset:
			return Mode_Reset;
		default:
			return Mode_Bus;
	}

}

void ScheduleManager::setModeToCom(){
	LockGuard l(lock);
	Logger::Stream(LEVEL_DEBUG,tags) << "Attempting to switch to Com from GroundCommunication Thread";
	com.timeSinceEpoch = getCurrentTime();
	currentSchedule = com;
}

void ScheduleManager::exitComMode(){
	LockGuard l(lock);
	Logger::Stream(LEVEL_DEBUG,tags) << "Attempting to switch back from Com from GroundCommunication Thread";
	currentSchedule = ScheduleQueue.front();
}

FSWMode ScheduleManager::checkMode(){
	return CurrentMode;
}

