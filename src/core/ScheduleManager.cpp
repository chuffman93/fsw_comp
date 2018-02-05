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
	modeEnterTime = getCurrentTime();
	tags += LogTag("Name", "ScheduleManager");
 }
ScheduleManager::~ScheduleManager(){};

//check for mode changes
FSWMode ScheduleManager::checkNewMode(){
	//get current time
	uint32_t time = getCurrentTime();
	//check if it is time to switch modes
	Logger::Stream(LEVEL_DEBUG,tags) << "Check new mode -- Queue Size: " << ScheduleQueue.size() << " With new mode at front: " << ScheduleQueue.front().mode;

	if (!ScheduleQueue.empty() && currentSchedule.timeSinceEpoch <= time && CurrentMode != currentSchedule.mode){
		if(CurrentMode != Mode_Bus){
			if(CurrentMode == Mode_Payload){
				CurrentMode = Trans_PayloadToBus;
				Logger::Stream(LEVEL_DEBUG, tags) << "Transition Mode: " << CurrentMode;
			}
			else if(CurrentMode == Mode_Com){
				CurrentMode = Trans_ComToBus;
				Logger::Stream(LEVEL_DEBUG, tags) << "Transition Mode: " << CurrentMode;
			}

			sleep(2);
			CurrentMode = Mode_Bus;
			sleep(2);
		}
		if(currentSchedule.mode == Mode_Payload){
			CurrentMode = Trans_BusToPayload;
			Logger::Stream(LEVEL_DEBUG, tags) << "Transition Mode: " << CurrentMode;
		}
		else if(currentSchedule.mode == Mode_Com){
			CurrentMode = Trans_BusToCom;
			Logger::Stream(LEVEL_DEBUG, tags) << "Transition Mode: " << CurrentMode;
		}
		sleep(2);
		CurrentMode = currentSchedule.mode;
		modeEnterTime = getCurrentTime();
		Logger::Stream(LEVEL_INFO, tags) << "Setting Mode to: " << CurrentMode;
	//check if the mode time has exceeded its duration
	}else if (!ScheduleQueue.empty() && ((time - modeEnterTime) >= currentSchedule.duration) && CurrentMode != Mode_Bus ){
		ScheduleQueue.pop();
		currentSchedule = ScheduleQueue.front();
		CurrentMode = Mode_Bus;
		Logger::Stream(LEVEL_INFO, tags) << "Setting Mode to Bus";
	//check if it is time for reboot if the schedule is empty
	}else if (ScheduleQueue.empty() && time > REBOOT_TIME){
		CurrentMode = Mode_Reset;
		Logger::Stream(LEVEL_INFO, tags) << "Setting Mode to Reset";
	}
	return CurrentMode;
}

void ScheduleManager::handleScheduling(){

	if (FileManager::checkExistance(NEW_DEFAULT_SCH)){
		Logger::Stream(LEVEL_INFO,tags) << "Updating Default Schedule";
		FileManager::moveFile(NEW_DEFAULT_SCH, DEFAULT_SCH);
	}

	if (FileManager::checkExistance(NEW_SCH)){
		Logger::Stream(LEVEL_INFO,tags) << "Loading New Schedule";
		loadSchedule(NEW_SCH);
	}else{
		Logger::Stream(LEVEL_INFO,tags) << "Loading Default Schedule";
		loadSchedule(DEFAULT_SCH);
	}

}

//if new schedule: adds new schedule to queue, otherwise add default schedule to queue if empty
void ScheduleManager::loadSchedule(std::string filePath){
	Logger::Stream(LEVEL_INFO,tags) << "Loading Next Schedule";
	//clear contents of the queue
	while(!ScheduleQueue.empty()){
		ScheduleQueue.pop();
	}
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
	Logger::Stream(LEVEL_DEBUG,tags) << "New Queue Size: " << ScheduleQueue.size() << " With new mode up front being: " << ScheduleQueue.front().mode;
}


