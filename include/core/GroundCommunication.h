/*
 * GroundCommunication.h
 *
 *  Created on: Jan 15, 2018
 *      Author: alpa3119
 */

#ifndef GROUNDCOMMUNICATION_H_
#define GROUNDCOMMUNICATION_H_

#include "core/FileManager.h"
#include "core/ScheduleManager.h"
#include "subsystem/SubsystemBase.h"
#include <queue>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

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
	GroundCommunication(std::vector<SubsystemBase*> subsystems);
	~GroundCommunication();
	void downlinkFiles();
	void clearDownlink();
	void spinGround();

	void parseIEF();

	void parsePPE();
	bool stateDownlink;
	bool statePostPass;
	uint32_t ComStartTime;
	uint32_t ComTimeout;

	std::queue<std::string> DownlinkQueue;
private:
	std::string trimNewline(std::string buffer);
	void executeFSWCommand(int command);
	void parseDownlinkRequest(char *line);
	void parseDeletionRequest(char *line);
	void parseCommandRequest(char *line);
	void parseFileListRequest(char *line);
	std::vector<SubsystemBase*> subsystems;
	LogTags tags;


	//ACS& acs;
	//COM& com;
	//EPS& eps;
	//RAD& rad;

};


#endif /* GROUNDCOMMUNICATION_H_ */
