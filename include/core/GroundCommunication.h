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
#include <deque>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <string>

//FSW commands (from old FSW), these are unused and can be deleted
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

	bool stateDownlink; // Flag that signifies we are downlinking files when true
	bool statePostPass; // Flag that signifies that the com pass is over when true, performs post pass tasks
	uint32_t ComStartTime; // Time stamp for beginning of com pass
	uint32_t ComTimeout; // Max length for com pass (seconds)
	std::vector<std::string> CommandAcknowledgements; //Queue containing all of the command acknowledgments (for current com pass)
	std::deque<std::string> DownlinkQueue; // Queue containing all the requested downlink items (for current com pass)
PRIVATE:
	std::string trimNewline(std::string buffer);
	void parseDownlinkRequest(std::string line);
	void parseDeletionRequest(std::string line);
	void parseCommandRequest(std::string line);
	void parseFileListRequest(std::string line);
	void sendCommandAcknowledgements();
	std::string grabFileName(std::string path);
	std::vector<SubsystemBase*> subsystems;
	bool firstFile; //Flag necessary for downlink logic, otherwise more than one file will be in an incorrect downlink order and incomplete downlink
	BeaconManager& beacon;
	COM* com;
	LogTags tags;
	Lock lock;


};


#endif /* GROUNDCOMMUNICATION_H_ */
