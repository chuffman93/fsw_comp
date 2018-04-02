/*
 * GroundCommunication.cpp
 *
 *  Created on: Jan 15, 2018
 *      Author: alpa3119
 */

#include "core/GroundCommunication.h"

GroundCommunication::GroundCommunication(std::vector<SubsystemBase*> subsystems, BeaconManager& beacon)
: stateDownlink(false), statePostPass(false), ComStartTime(0), ComTimeout(720), subsystems(subsystems), beacon(beacon), firstFile(true)
{
	tags += LogTag("Name", "GroundCommunication");
}

GroundCommunication::~GroundCommunication(){}

/*!
 * Function used to run through the downlink queue and pass them into the downlink folder for COM
 */
void GroundCommunication::downlinkFiles(){

	if (!DownlinkQueue.empty()){
		std::string file = DownlinkQueue.front();
		if (firstFile == true){
			Logger::Stream(LEVEL_INFO,tags) << "Downlinking Next File: " << grabFileName(file).c_str();
			FileManager::copyFile(file, DOWNLINK_DIRECTORY + grabFileName(file));
			firstFile = false;
		}else if (!FileManager::checkExistance(DOWNLINK_DIRECTORY + grabFileName(file))){
			DownlinkQueue.pop();
			if (!DownlinkQueue.empty()){
				file = DownlinkQueue.front();
				Logger::Stream(LEVEL_INFO,tags) << "Downlinking Next File: " << grabFileName(file).c_str();
				FileManager::copyFile(file, DOWNLINK_DIRECTORY + grabFileName(file));
			}
		}
	}
	if (DownlinkQueue.empty()){
		stateDownlink = false;
		statePostPass = true;
		firstFile = true;
	}
	Logger::Stream(LEVEL_INFO,tags) << "Completed Downlink of All Files";
}

/*!
 * Used to clean the downlink queue pending the ending of a COM pass
 */
void GroundCommunication::clearDownlink(){
	LockGuard l(lock);
	Logger::Stream(LEVEL_INFO,tags) << "Communication Pass over, clearing downlink queue";
	while (!DownlinkQueue.empty()){
		DownlinkQueue.pop();
	}
}

/*!
 * Used to trim the new line character when reading in lines from ground files
 */
std::string GroundCommunication::trimNewline(std::string buffer){
	  // Remove the newline at the end of a string
	buffer.erase(std::remove(buffer.begin(), buffer.end(), '\n'), buffer.end());
	return buffer;
}


// TODO: Do we need this?
void GroundCommunication::executeFSWCommand(int command){
	int ret;
	switch (command) {
	case FSW_CMD_REQUEST_RESET:
		//schServer->RequestReset();
		break;
	case FSW_CMD_HARD_SATELLITE_RESET:
		//cdhServer->resetAssert(HARDWARE_LOCATION_EPS);
		break;
	case FSW_CMD_TX_SILENCE_START:
		//comServer->setTxSilence(true);
		break;
	case FSW_CMD_TX_SILENCE_END:
		//comServer->setTxSilence(false);
		break;
	case FSW_CMD_CLEAR_DWNLK:
		ret = system("rm -rf " DOWNLINK_DIRECTORY "/*");
		break;
	case FSW_CMD_CLEAR_IMMED:
		//ret = system("rm -rf " IMMED_DIRECTORY "/*");
		break;
	case FSW_CMD_CLEAR_UPLK:
		ret = system("rm -rf " UPLINK_DIRECTORY "/*");
		break;
	default:
		break;
	}
}

/*!
 * Decides how to handle a downlink request
 * \param string read in from the IEF.txt indicating a desire for a downlink
 */
void GroundCommunication::parseDownlinkRequest(std::string line){
	char downlinkRequest[100];
	strcpy(downlinkRequest, line.c_str());

	char * dwl = strtok(downlinkRequest,",");

	char * type = strtok(NULL, ",");
	if (type == NULL){
		//log error
		return;
	}
	char * file = strtok(NULL, ",");
	if (file == NULL){
		//log error
		return;
	}
	if (strcmp(type,"F") == 0){
		Logger::Stream(LEVEL_INFO,tags) << "Adding file(s): " << line << " to downlink queue";
		while (file != NULL){
			std::string path = trimNewline(std::string(file));
			DownlinkQueue.push(path);
			file = strtok(NULL,",");
		}


	}else if (strcmp(type,"R") == 0){
		Logger::Stream(LEVEL_INFO,tags) << "Adding regex(s) " << line << " to downlink queue";
		while (file != NULL){
			std::string regex = trimNewline(std::string(file));
			std::vector<std::string> reg = FileManager::packageFiles(regex,"R");
			std::vector<std::string>::iterator it;
			for (it = reg.begin(); it < reg.end(); it++){
				Logger::Stream(LEVEL_DEBUG,tags) << "Adding: " << (*it).c_str() << ". to downlink queue";
				DownlinkQueue.push(*it);
			}
			file = strtok(NULL,",");
		}

	}else if (strcmp(type,"RB") == 0){
		Logger::Stream(LEVEL_INFO,tags) << "Adding regex(s) before " << line << " to downlink queue";
		while (file != NULL){
			std::string regex = trimNewline(std::string(file));
			std::vector<std::string> reg = FileManager::packageFiles(regex,"RB");
			std::vector<std::string>::iterator it;
			for (it = reg.begin(); it < reg.end(); it++){
				Logger::Stream(LEVEL_DEBUG,tags) << "Adding: " << (*it).c_str() << ". to downlink queue";
				DownlinkQueue.push(*it);
			}
			file = strtok(NULL,",");
		}

	}else if (strcmp(type,"RA") == 0){
		Logger::Stream(LEVEL_INFO,tags) << "Adding regex after(s) " << line << " to downlink queue";
		while (file != NULL){
			std::string regex = trimNewline(std::string(file));
			std::vector<std::string> reg = FileManager::packageFiles(regex,"RA");
			std::vector<std::string>::iterator it;
			for (it = reg.begin(); it < reg.end(); it++){
				Logger::Stream(LEVEL_DEBUG,tags) << "Adding: " << (*it).c_str() << " to downlink queue";
				DownlinkQueue.push(*it);
			}
			file = strtok(NULL,",");
		}

	}else {
		//log error
	}
}

/*!
 * Decides how to handle a deletion request
 * \param string read in from the IEF.txt indicating a desire for deletion
 */
void GroundCommunication::parseDeletionRequest(std::string line){
	char deleteRequest[100];
	strcpy(deleteRequest, line.c_str());
	char * del = strtok(deleteRequest, ",");

	char * type = strtok(NULL, ",");
	if (type == NULL){
		//log error
		return;
	}
	char * file = strtok(NULL, ",");
	if (file == NULL){
		//log error
		return;
	}
	if (strcmp(type,"F") == 0){
		Logger::Stream(LEVEL_INFO,tags) << "Deleting file(s) " << line ;
		while (file != NULL){
			std::string path = trimNewline(std::string(file));
			FileManager::deleteFile(path);
			file = strtok(NULL,",");
		}


	}if (strcmp(type,"R") == 0){
		Logger::Stream(LEVEL_INFO,tags) << "Deleting regex(s) " << line ;
		while (file != NULL){
			std::string regex = trimNewline(std::string(file));
			FileManager::regexDelete(regex,"R");
			file = strtok(NULL,",");
		}

	}if (strcmp(type,"RB") == 0){
		Logger::Stream(LEVEL_INFO,tags) << "Deleting regex(s) before " << line ;
		while (file != NULL){
			std::string regex = trimNewline(std::string(file));
			FileManager::regexDelete(regex,"RB");
			file = strtok(NULL,",");
		}

	}if (strcmp(type,"RA") == 0){
		Logger::Stream(LEVEL_INFO,tags) << "Deleting regex(s) after " << line ;
		while (file != NULL){
			std::string regex = trimNewline(std::string(file));
			FileManager::regexDelete(regex,"RA");
			file = strtok(NULL,",");
		}

	}
}

/*!
 * Decides how to handle a command request
 * \param string read in from the IEF.txt indicating a desire for commanding the system or a subsystem
 */
void GroundCommunication::parseCommandRequest(std::string line){
	//TODO: error handling for opcodes numbers in between min and max that don't exist
	char commandRequest[100];
	strcpy(commandRequest, line.c_str());

	char * c = strtok(commandRequest,",");

	char * sys = strtok(NULL,",");
	if (sys == NULL){
		Logger::Stream(LEVEL_ERROR,tags) << "ParseCommandRequest: No system given for command execution";
		return;
	}

	char * command = strtok(NULL, ",");
	if (command == NULL){
		Logger::Stream(LEVEL_ERROR,tags) << "ParseCommandRequest: No command given for command execution";
		return;
	}

	if (strcmp(sys,"SYS") == 0){
		while (command != NULL){
			std::string cmd = trimNewline(std::string(command));
			command = (char*) cmd.c_str();
			Logger::Stream(LEVEL_INFO,tags) << "Executing System Command: " << command;
			int16_t ret = system(command);
			Logger::Stream(LEVEL_INFO,tags) << "System Command Return: " << ret;
			command = strtok(NULL,",");
		}

	}else if (strcmp(sys,"FSW") == 0){
		while(command != NULL){
			std::string tempCmd = trimNewline(std::string(command));
			int cmd = atoi(tempCmd.c_str());
			if (cmd <= 0 || cmd >= FSW_CMD_MAX) {
				Logger::Stream(LEVEL_ERROR,tags) << "Unable to Execute FSW Command, command does not exist";
			}else{
				Logger::Stream(LEVEL_INFO,tags) << "Executing FSW Command" << (FSWCommandTypes)cmd;
				executeFSWCommand(cmd);
			}
			command = strtok(NULL,",");
		}
	}else if (strcmp(sys, "ACS") == 0){
		while (command != NULL){
			std::string tempCmd = trimNewline(std::string(command));
			uint8_t cmd = (uint8_t)atoi((char*)tempCmd.c_str());
			if ((cmd >= OP_ACS_MIN && cmd < OP_ACS_MAX) || (cmd >= OP_MIN && cmd < OP_MAX)){
				Logger::Stream(LEVEL_INFO,tags) << "Executing ACS command: " << command;
				std::vector<uint8_t> buff;
				ACPPacket ret = subsystems[0]->sendOpcode(cmd,buff);
			}else{
				Logger::Stream(LEVEL_ERROR,tags) << "Unable to Execute ACS Command, command does not exist";

			}
			command = strtok(NULL,",");
		}

	}else if (strcmp(sys, "COM") == 0){
		while (command!= NULL){
			std::string tempCmd = trimNewline(std::string(command));
			uint8_t cmd = (uint8_t)atoi((char*)tempCmd.c_str());
			if ((cmd >= OP_ACS_MIN && cmd < OP_ACS_MAX) || (cmd >= OP_MIN && cmd < OP_MAX)){
				Logger::Stream(LEVEL_INFO,tags) << "Executing COM command: " << command;
				std::vector<uint8_t> buff;
				ACPPacket ret = subsystems[1]->sendOpcode(cmd,buff);
			}else{
				Logger::Stream(LEVEL_ERROR,tags) << "Unable to Execute COM Command, command does not exist";
			}
			command = strtok(NULL,",");
		}

	}else if (strcmp(sys, "EPS") == 0){
		while (command != NULL){
			std::string tempCmd = trimNewline(std::string(command));
			uint8_t cmd = (uint8_t)atoi((char*)tempCmd.c_str());
			if ((cmd >= OP_ACS_MIN && cmd < OP_ACS_MAX) || (cmd >= OP_MIN && cmd < OP_MAX)){
				Logger::Stream(LEVEL_INFO,tags) << "Executing EPS command: " << command;
				std::vector<uint8_t> buff;
				ACPPacket ret = subsystems[2]->sendOpcode(cmd,buff);
			}else{
				Logger::Stream(LEVEL_ERROR,tags) << "Unable to Execute EPS Command, command does not exist";

			}
			command = strtok(NULL,",");
		}
	}else if (strcmp(sys, "RAD") == 0){
		while (command != NULL){
			std::string tempCmd = trimNewline(std::string(command));
			uint8_t cmd = (uint8_t)atoi((char*)tempCmd.c_str());
			if ((cmd >= OP_ACS_MIN && cmd < OP_ACS_MAX) || (cmd >= OP_MIN && cmd < OP_MAX)){
				Logger::Stream(LEVEL_INFO,tags) << "Executing RAD command: " << command;
				std::vector<uint8_t> buff;
				ACPPacket ret = subsystems[3]->sendOpcode(cmd,buff);
			}else{
				Logger::Stream(LEVEL_ERROR,tags) << "Unable to Execute RAD Command, command does not exist";
			}
			command = strtok(NULL,",");
		}
	}
}

/*!
 * Handles getting a request for obtaining a list of files in a directory
 * \param string read in from the IEF.txt indicating a desire for a file list
 */
void GroundCommunication::parseFileListRequest(std::string line){
	char downlinkRequest[100];
	FileManager::deleteFile(DFL_PATH);
	strcpy(downlinkRequest, line.c_str());
	char * request = strtok(downlinkRequest,",");
	char * dir = strtok(NULL, ",");
	if (dir == NULL){
		//log error
	}
	while (dir != NULL){
		std::string directory = trimNewline(std::string(dir));
		FileManager::generateFilesList(directory);
		dir = strtok(NULL, ",");
	}
	DownlinkQueue.push(DFL_PATH);
}

// TODO: Do we need this?
void GroundCommunication::createCommandAcknowledgement(std::string command, std::string success){

}

/*!
 * Grabs the name of a file from a path. Used for downlinking
 * \param Path to the file
 */
std::string GroundCommunication::grabFileName(std::string path){
	int i = path.length()-1;
	std::string filename = "";
	while(path[i]!='/'){
		filename = path[i]+filename;
		i--;
	}
	Logger::Stream(LEVEL_DEBUG,tags) << "Grabbed file name for downlink: " << filename.c_str() << ".";
	return filename;
}


/*!
 * Parsing through the IEF.txt file from ground to interpret the different requests
 */
void GroundCommunication::parseIEF(){

	LockGuard l(lock);
	std::vector<std::string> requests = FileManager::parseGroundFile(IEF_PATH);
	char line[100];
	char * type;

	std::vector<std::string>::iterator it;
	for (it = requests.begin(); it != requests.end(); it++){
		strcpy(line, (*it).c_str());
		type = strtok(line,",");
		if (type == NULL){
			continue;
		}else if (strcmp(type,"CMD") == 0){
			parseCommandRequest((*it).c_str());
		}else if (strcmp(type,"DWL") == 0){
			parseDownlinkRequest((*it).c_str());
		}else if (strcmp(type,"DLT") == 0){
			parseDeletionRequest((*it).c_str());
		}else if (strcmp(type, "DFL") == 0){
			parseFileListRequest((*it).c_str());
		}
	}
}

// TODO: Do we need this?
void GroundCommunication::parsePPE(){
	if (!FileManager::checkExistance(PPE_PATH)){
		Logger::Stream(LEVEL_INFO,tags) << "No PPE file found";
		return;
	}
	LockGuard l(lock);
	std::vector<std::string> requests = FileManager::parseGroundFile(PPE_PATH);
	char line[100];
	char * type;

	std::vector<std::string>::iterator it;
	for (it = requests.begin(); it <= requests.end(); it++){

		strcpy(line, (*it).c_str());
		type = strtok(line,",");

		if (type == NULL){
			continue;
		}else if (strcmp(type,"CMD") == 0){
			parseCommandRequest(line);
		}else if (strcmp(type,"DLT") == 0){
			parseDeletionRequest(line);
		}else {
			//log error
		}
	}

	FileManager::deleteFile(PPE_PATH);
}

/*!
 * Heart of the GroundComm. Checks for SOT.txt for start of transmission and runs through the process for a COM pass.
 * Also sends beacon.
 * \param A pointer to the watchdog to allow spin to let watchdog know ground is not dead
 */
bool GroundCommunication::spinGround(Watchdog* watchdog){
//	LockGuard l(lock);
	if (!FileManager::checkExistance(SOT_PATH)){
		Logger::Stream(LEVEL_INFO,tags) << "Sending beacon...";
		beacon.sendBeacon();
		Logger::Stream(LEVEL_INFO,tags) << "Beacon has been sent";
		for(int i = 0; i <= 60; i++){
			if(FileManager::checkExistance(SOT_PATH)){break;}
			watchdog->KickWatchdog();
			sleep(1);
		}
		return false;

	}else{
		if (ComStartTime == 0){
			Logger::Stream(LEVEL_INFO,tags) << "Beginning Communication Pass Timer";
			ComStartTime = getCurrentTime();
			return true;
		}
		Logger::Stream(LEVEL_DEBUG,tags) << "Start Time: " << ComStartTime << " ComTimeOut: " << ComTimeout << " Current Time: " << getCurrentTime();
		//check if the communication pass has exceeded
		if ((ComStartTime + ComTimeout) < getCurrentTime()){
			Logger::Stream(LEVEL_INFO,tags) << "Communication Pass Timeout";
			if (FileManager::checkExistance(IEF_PATH)){
				FileManager::deleteFile(IEF_PATH);
			}
			ComStartTime = 0;
			stateDownlink = false;
			clearDownlink();
			statePostPass = true;
			return false;
		}
		//begin IEF processing if IEF has been uplinked
		if (FileManager::checkExistance(IEF_PATH)){
			Logger::Stream(LEVEL_INFO,tags) << "Received IEF";
			parseIEF();
			stateDownlink = true;
		//begin downlink if IEF processing has ended
		}else if (stateDownlink){
			Logger::Stream(LEVEL_INFO,tags) << "Beginning Requested Downlink";
			downlinkFiles();
		//begin post pass processes if downlink has concluded or if com has timed out
		}else if (statePostPass){
			Logger::Stream(LEVEL_INFO,tags) << "Entering Communication Post Pass";
			parsePPE();
			FileManager::deleteFile(SOT_PATH);
			statePostPass = false;
			ComStartTime = 0;
			clearDownlink();
			return false;
		}
	}
	return true;
}

