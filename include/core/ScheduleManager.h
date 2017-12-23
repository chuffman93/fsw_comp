/*
 * ScheduleManager.h
 *
 *  Created on: Dec 22, 2017
 *      Author: adam
 */

#ifndef INCLUDE_CORE_SCHEDULEMANAGER_H_
#define INCLUDE_CORE_SCHEDULEMANAGER_H_

#include <queue>

enum Mode{
	BUS_PRIORITY,
	PLD_PRIORITY,
	COM_MODE,
	RESET_MODE
};

struct ScheduleStruct {
	Mode mode;
	int waitTime; //time to wait in bus priority before entering mode
	int duration; //duration of mode
};

class ScheduleManager{
private:
	std::queue <ScheduleStruct> ScheduleQueue;

	Mode CurrentMode;
	int32_t modeEnterTime;

	//if new schedule: adds new schedule to queue, otherwise add default schedule to queue if empty
	void updateSchedule();
	//updates default schedule file
	void updateDefaultSchedule();
	//load default schedule from file
	void loadDefaultSchedule();
	//check for mode changes
	void checkNewMode();
};

#endif /* INCLUDE_CORE_SCHEDULEMANAGER_H_ */
