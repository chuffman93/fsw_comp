/*
 * ScheduleManager.h
 *
 *  Created on: Dec 22, 2017
 *      Author: adam
 */

#ifndef INCLUDE_CORE_SCHEDULEMANAGER_H_
#define INCLUDE_CORE_SCHEDULEMANAGER_H_

#include <stdint.h>
#include <queue>
#include <vector>

typedef enum _FSWMode{
	BUS_PRIORITY = 0,
	PLD_PRIORITY,
	COM_MODE,
	RESET_MODE,
	NUM_MODES
} FSWMode;


struct ScheduleStruct {
	FSWMode mode;
	int waitTime; //time to wait in bus priority before entering mode
	int duration; //duration of mode
};



class ScheduleManager
{
public:
	ScheduleManager():CurrentMode(BUS_PRIORITY), modeEnterTime(0){};
	~ScheduleManager(){};

	//check for mode changes
	FSWMode checkNewMode(){return BUS_PRIORITY;}; //TODO: Implement

private:
	std::queue <ScheduleStruct> ScheduleQueue;

	FSWMode CurrentMode;
	int32_t modeEnterTime;

	//if new schedule: adds new schedule to queue, otherwise add default schedule to queue if empty
	void updateSchedule();
	//updates default schedule file
	void updateDefaultSchedule();
	//load default schedule from file
	void loadDefaultSchedule();
};

#endif /* INCLUDE_CORE_SCHEDULEMANAGER_H_ */
