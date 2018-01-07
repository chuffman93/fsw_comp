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
	if ((time - modeEnterTime) > currentSchedule.duration){
		ScheduleQueue.pop();
		currentSchedule = ScheduleQueue.front();
		CurrentMode = currentSchedule.mode;
		modeEnterTime = getCurrentTime();
		return CurrentMode;
	//check if the schedule is empty and if the satellite is not in bus mode
	}else if (ScheduleQueue.empty() && CurrentMode != Mode_Bus){
		CurrentMode = Mode_Bus;
		modeEnterTime = getCurrentTime();
		return CurrentMode;
	//check if it is time for reboot if the schedule is empty
	}else if (ScheduleQueue.empty() && time > REBOOT_TIME){
		CurrentMode = Mode_Reset;
		modeEnterTime = getCurrentTime();
		return CurrentMode;
	}
	return CurrentMode;
}


//if new schedule: adds new schedule to queue, otherwise add default schedule to queue if empty
void updateSchedule(){

}

//updates default schedule file
void updateDefaultSchedule(){

}

//load default schedule from file
void loadDefaultSchedule(){

}


