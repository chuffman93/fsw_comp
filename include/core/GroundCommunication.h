/*
 * GroundCommunication.h
 *
 *  Created on: Jan 15, 2018
 *      Author: alpa3119
 */

#ifndef GROUNDCOMMUNICATION_H_
#define GROUNDCOMMUNICATION_H_

#include "core/FileManager.h"
#include "core/Lock.h"
#include "subsystem/SubsystemBase.h"
#include "core/Watchdog.h"
#include "subsystem/ACS.h"
#include "subsystem/COM.h"
#include "subsystem/EPS.h"
#include "subsystem/RAD.h"
#include "util/BeaconManager.h"
#include <queue>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <string>

typedef enum FSWCommandTypes {
	FSW_CMD_REQUEST_RESET = 1,
	FSW_CMD_HARD_SATELLITE_RESET,
	FSW_CMD_TX_SILENCE_START,
	FSW_CMD_TX_SILENCE_END,
	FSW_CMD_CLEAR_DWNLK,
	FSW_CMD_CLEAR_IMMED,
	FSW_CMD_CLEAR_UPLK,
	FSW_CMD_MAX
} FSWCommand;

class GroundCommunication{
public:
	GroundCommunication(std::vector<SubsystemBase*> subsystems, BeaconManager& beacon);
	~GroundCommunication();
	void downlinkFiles();
	void clearDownlink();
	bool spinGround(Watchdog* watchdog);
	void parseIEF();

	void parsePPE();
	bool stateDownlink;
	bool statePostPass;
	uint32_t ComStartTime;
	uint32_t ComTimeout;

	std::queue<std::string> DownlinkQueue;
PRIVATE:
	std::string trimNewline(std::string buffer);
	void executeFSWCommand(int command);
	void parseDownlinkRequest(std::string line);
	void parseDeletionRequest(std::string line);
	void parseCommandRequest(std::string line);
	void parseFileListRequest(std::string line);
	std::string grabFileName(std::string path);
	std::vector<SubsystemBase*> subsystems;
	bool firstFile;
	BeaconManager& beacon;


	LogTags tags;
	Lock lock;


};


#endif /* GROUNDCOMMUNICATION_H_ */
