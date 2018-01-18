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
	GroundCommunication();
	~GroundCommunication();
	void downlinkFiles();
	void clearDownlink();
	void handleScheduling();
	void parseDownlinkRequest(char *line);
	void parseDeletionRequest(char *line);
	void parseCommandRequest(char *line);
	void parseFileListRequest(char *line);

	std::queue<std::string> DownlinkQueue;
private:
	std::string trimNewline(std::string buffer);
	void executeFSWCommand(int command);
	LogTags tags;

};


#endif /* GROUNDCOMMUNICATION_H_ */
