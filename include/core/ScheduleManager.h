/*
 * ScheduleManager.h
 *
 *  Created on: Dec 22, 2017
 *      Author: adam
 */

#ifndef INCLUDE_CORE_SCHEDULEMANAGER_H_
#define INCLUDE_CORE_SCHEDULEMANAGER_H_


#include <stdint.h>
#include <string>
#include <unistd.h>
#include <queue>
#include <vector>
#include <math.h>

#include "util/TimeKeeper.h"
#include "core/FileManager.h"
#include "core/FileSystem.h"
#include "util/ByteStream.h"

#define REBOOT_TIME 86400

typedef enum _FSWMode{
	Mode_Bus = 1,
	Mode_Payload = 2,
	Mode_Com = 3,
	Mode_Reset = 4,
	Trans_BusToPayload = 5,
	Trans_PayloadToBus = 6,
	Trans_BusToCom = 7,
	Trans_ComToBus = 8,
}FSWMode;

struct ScheduleStruct {
	FSWMode mode;
	uint32_t timeSinceEpoch;
	uint32_t duration;
};

class ScheduleManager
{
public:
	ScheduleManager();
	~ScheduleManager();

	//check for mode changes
	FSWMode checkNewMode();
	void handleScheduling();
	//if new schedule: adds new schedule to queue, otherwise add default schedule to queue if empty
	void loadSchedule(std::string filePath);
	FSWMode handleModeChange(FSWMode current, FSWMode next);


PRIVATE:
	std::queue <ScheduleStruct> ScheduleQueue;

	ScheduleStruct currentSchedule;
	uint32_t modeEnterTime;
	FSWMode CurrentMode;


	//updates default schedule file
	void updateDefaultSchedule();

	LogTags tags;

};

#endif /* INCLUDE_CORE_SCHEDULEMANAGER_H_ */
