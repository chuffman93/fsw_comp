/*
 * ScheduleManager.cpp
 *
 *  Created on: Dec 22, 2017
 *      Author: alex
 */

#include "core/ScheduleManager.h"

ScheduleManager::ScheduleManager()
 : CurrentMode(Mode_Bus), modeEnterTime(0)
 {}
ScheduleManager::~ScheduleManager(){};

//check for mode changes
FSWMode ScheduleManager::checkNewMode(){
	//get current time
	uint32_t time = getCurrentTime();

	//check if the mode time has exceeded its duration
	if (((time - modeEnterTime) > currentSchedule.duration) && CurrentMode != Mode_Bus ){
		ScheduleQueue.pop();
		currentSchedule = ScheduleQueue.front();
		CurrentMode = Mode_Bus;
		modeEnterTime = getCurrentTime();
	//check if the schedule is empty and if the satellite is not in bus mode
	}else if (ScheduleQueue.empty() && CurrentMode != Mode_Bus){
		CurrentMode = Mode_Bus;
		modeEnterTime = getCurrentTime();
	//check if it is time for reboot if the schedule is empty
	}else if (ScheduleQueue.empty() && time > REBOOT_TIME){
		CurrentMode = Mode_Reset;
		modeEnterTime = getCurrentTime();
	}else if (currentSchedule.timeSinceEpoch > time){
		CurrentMode = currentSchedule.mode;
		modeEnterTime = getCurrentTime();
	}
	return CurrentMode;
}


//if new schedule: adds new schedule to queue, otherwise add default schedule to queue if empty
void ScheduleManager::loadSchedule(std::string filePath){

	//clear contents of the queue
	while(!ScheduleQueue.empty()){
		ScheduleQueue.pop();
	}

	FileManager fm;
	ScheduleStruct sch;

	std::vector<uint8_t> schedule = fm.readFromFile(filePath);

	ByteStream bs(schedule);
	uint8_t mode;


	//TODO: error handling: make sure that the file size is divisible by 9
	for (size_t i = 0; i < (schedule.size()/9); i++){
		bs >> mode >> sch.timeSinceEpoch >> sch.duration;
		switch (mode){
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

//updates default schedule file
void ScheduleManager::updateDefaultSchedule(){
	FileManager fm;
	bool exist = fm.checkExistance(NEW_SCH);
	if (exist){
		fm.moveFile(NEW_SCH,DEFAULT_SCH);
	}
}


