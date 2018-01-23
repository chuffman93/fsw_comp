/*
 * GroundCommunication.cpp
 *
 *  Created on: Jan 15, 2018
 *      Author: alpa3119
 */

#include "core/GroundCommunication.h"

GroundCommunication::GroundCommunication(std::vector<SubsystemBase*> subsystems)
: stateDownlink(false), statePostPass(false), ComStartTime(-1), ComTimeout(720), subsystems(subsystems)
{
	tags += LogTag("Name", "GroundCommunication");
}

GroundCommunication::~GroundCommunication(){}

void GroundCommunication::downlinkFiles(){
	std::string file = DownlinkQueue.front();
	if (!DownlinkQueue.empty()){
		if (!FileManager::checkExistance(DOWNLINK_DIRECTORY + file)){
			Logger::Stream(LEVEL_INFO,tags) << "Downlinking Next File";
			DownlinkQueue.pop();
			file = DownlinkQueue.front();
			FileManager::copyFile(file, DOWNLINK_DIRECTORY + file);
		}
	}
	if (DownlinkQueue.empty()){
		stateDownlink = false;
		statePostPass = true;
	}
	Logger::Stream(LEVEL_INFO,tags) << "Completed Downlink of All Files";
}

void GroundCommunication::clearDownlink(){
	while (!DownlinkQueue.empty()){
		DownlinkQueue.pop();
	}
}

std::string GroundCommunication::trimNewline(std::string buffer){
	  // Remove the newline at the end of a string
	buffer.erase(std::remove(buffer.begin(), buffer.end(), '\n'), buffer.end());
	return buffer;
}

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
		while (file != NULL){
			std::string path = trimNewline(std::string(file));
			DownlinkQueue.push(path);
			file = strtok(NULL,",");
		}


	}else if (strcmp(type,"R") == 0){
		while (file != NULL){
			std::string regex = trimNewline(std::string(file));
			std::vector<std::string> reg = FileManager::packageFiles(regex,"R");
			std::vector<std::string>::iterator it;
			for (it = reg.begin(); it <= reg.end(); it++){
				DownlinkQueue.push(*it);
			}
			file = strtok(NULL,",");
		}

	}else if (strcmp(type,"RB") == 0){
		while (file != NULL){
			std::string regex = trimNewline(std::string(file));
			std::vector<std::string> reg = FileManager::packageFiles(regex,"RB");
			std::vector<std::string>::iterator it;
			for (it = reg.begin(); it <= reg.end(); it++){
				DownlinkQueue.push(*it);
			}
			file = strtok(NULL,",");
		}

	}else if (strcmp(type,"RA") == 0){
		while (file != NULL){
			std::string regex = trimNewline(std::string(file));
			std::vector<std::string> reg = FileManager::packageFiles(regex,"RA");
			std::vector<std::string>::iterator it;
			for (it = reg.begin(); it <= reg.end(); it++){
				DownlinkQueue.push(*it);
			}
			file = strtok(NULL,",");
		}

	}else {
		//log error
	}
}

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
		while (file != NULL){
			std::string path = trimNewline(std::string(file));
			FileManager::deleteFile(path);
			file = strtok(NULL,",");
		}


	}if (strcmp(type,"R") == 0){
		while (file != NULL){
			std::string regex = trimNewline(std::string(file));
			FileManager::regexDelete(regex,"R");
			file = strtok(NULL,",");
		}

	}if (strcmp(type,"RB") == 0){
		while (file != NULL){
			std::string regex = trimNewline(std::string(file));
			FileManager::regexDelete(regex,"R");
			file = strtok(NULL,",");
		}

	}if (strcmp(type,"RA") == 0){
		while (file != NULL){
			std::string regex = trimNewline(std::string(file));
			FileManager::regexDelete(regex,"R");
			file = strtok(NULL,",");
		}

	}
}

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
				std::vector<uint8_t> buff;
				ACPPacket ret = subsystems[3]->sendOpcode(cmd,buff);
			}else{
				Logger::Stream(LEVEL_ERROR,tags) << "Unable to Execute RAD Command, command does not exist";
			}
			command = strtok(NULL,",");
		}
	}
}

void GroundCommunication::parseFileListRequest(std::string line){
	char downlinkRequest[100];
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
}


void GroundCommunication::parseIEF(){

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

	FileManager::deleteFile(IEF_PATH);
}


void GroundCommunication::parsePPE(){
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

void GroundCommunication::spinGround(){
	if (!FileManager::checkExistance(SOT_PATH)){
		//send beacon
	}else{
		//check if the communication pass has exceeded
		if (ComStartTime + ComTimeout > getCurrentTime()){
			FileManager::deleteFile(IEF_PATH);
			stateDownlink = false;
			clearDownlink();
			statePostPass = true;
		}
		//check if the com state time has been updated
		if (ComStartTime == 0){
			ComStartTime = getCurrentTime();
		}
		//begin IEF processing if IEF has been uplinked
		if (FileManager::checkExistance(IEF_PATH)){
			parseIEF();
			stateDownlink = true;
		//begin downlink if IEF processing has ended
		}else if (stateDownlink){
			downlinkFiles();
		//begin post pass processes if downlink has concluded or if com has timed out
		}else if (statePostPass)
			//ScheduleManager sch;
			//sch.handleScheduling();
			parsePPE();
			FileManager::deleteFile(SOT_PATH);
			statePostPass = false;
			ComStartTime = -1;
			//clear downlink directory
		}
}



