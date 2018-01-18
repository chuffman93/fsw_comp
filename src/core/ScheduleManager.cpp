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
	if (!ScheduleQueue.empty() && currentSchedule.timeSinceEpoch <= time && CurrentMode != currentSchedule.mode){
		CurrentMode = currentSchedule.mode;
		modeEnterTime = getCurrentTime();
		Logger::Stream(LEVEL_INFO, tags) << "Setting Mode to: " << CurrentMode;
	//check if the mode time has exceeded its duration
	}else if (!ScheduleQueue.empty() && ((time - modeEnterTime) >= currentSchedule.duration) && CurrentMode != Mode_Bus ){
		ScheduleQueue.pop();
		currentSchedule = ScheduleQueue.front();
		CurrentMode = Mode_Bus;
		Logger::Stream(LEVEL_INFO, tags) << "Setting Mode to: " << CurrentMode;
	//check if it is time for reboot if the schedule is empty
	}else if (ScheduleQueue.empty() && time > REBOOT_TIME){
		CurrentMode = Mode_Reset;
		Logger::Stream(LEVEL_INFO, tags) << "Setting Mode to: " << CurrentMode;
	}
	return CurrentMode;
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


	//make sure that the file size is divisible by 9
	if (floor(schedule.size()/9) != (schedule.size()/9)){
		Logger::Stream::(LEVEL_ERROR,tags) << "Unable To Load Next Schedule, incorrect file size";
		return;
	}

	for (size_t i = 0; i < (schedule.size()/9); i++){
		bs >> mode >> sch.timeSinceEpoch >> sch.duration;
		int md = (int)mode;
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
}


