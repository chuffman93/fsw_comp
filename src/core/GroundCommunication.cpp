/*
 * GroundCommunication.cpp
 *
 *  Created on: Jan 15, 2018
 *      Author: alpa3119
 */

#include "core/GroundCommunication.h"
#include "subsystem/ACS.h"
#include "subsystem/COM.h"
#include "subsystem/EPS.h"
#include "subsystem/RAD.h"
#include "interfaces/ACPInterface.h"

GroundCommunication::GroundCommunication(std::vector<SubsystemBase*> subsystems)
: subsystems(subsystems), stateDownlink(false), statePostPass(false), ComStartTime(-1), ComTimeout(720)
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
	  size_t len = buffer.size();
	  if (len && (buffer[len-1] == '\n')) {
	    buffer[len-1] = '\0';
	  }
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

void GroundCommunication::parseDownlinkRequest(char *line){
	char * type = strtok(line, ",");
	if (type == NULL){
		//log error
		return;
	}
	char * file = strtok(line, ",");
	if (file == NULL){
		//log error
		return;
	}

	if (strcmp(type,"F")){
		while (file != NULL){

			file = strtok(NULL,",");
		}


	}if (strcmp(type,"R")){
		while (file != NULL){

			file = strtok(NULL,",");
		}

	}if (strcmp(type,"RB")){
		while (file != NULL){

			file = strtok(NULL,",");
		}

	}if (strcmp(type,"RA")){
		while (file != NULL){

			file = strtok(NULL,",");
		}

	}
}

void GroundCommunication::parseDeletionRequest(char *line){
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

	if (strcmp(type,"F")){
		while (file != NULL){

			file = strtok(NULL,",");
		}


	}if (strcmp(type,"R")){
		while (file != NULL){

			file = strtok(NULL,",");
		}

	}if (strcmp(type,"RB")){
		while (file != NULL){

			file = strtok(NULL,",");
		}

	}if (strcmp(type,"RA")){
		while (file != NULL){

			file = strtok(NULL,",");
		}

	}
}

void GroundCommunication::parseCommandRequest(char *line){
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

	if (strcmp(sys,"SYS")){
		while (command != NULL){
			std::string cmd = trimNewline(std::string(command));
			command = (char*) cmd.c_str();
			Logger::Stream(LEVEL_INFO,tags) << "Executing System Command: " << command;
			int16_t ret = system(command);
			Logger::Stream(LEVEL_INFO,tags) << "System Command Return: " << ret;
			command = strtok(NULL,",");
		}

	}else if (strcmp(sys,"FSW")){
		while(command != NULL){
			std::string tempCmd = trimNewline(std::string(command));
			int cmd = atoi((char*)tempCmd.c_str());
			if (cmd <= 0 || cmd >= FSW_CMD_MAX) {
				Logger::Stream(LEVEL_ERROR,tags) << "Unable to Execute FSW Command, command does not exist";
			}else{
				Logger::Stream(LEVEL_INFO,tags) << "Executing FSW Command" << (FSWCommandTypes)cmd;
				executeFSWCommand(cmd);
			}
			command = strtok(NULL,",");
		}
	}/*else if (strcmp(sys, "ACS")){
		while (command != NULL){
			std::string tempCmd = trimNewline(std::string(command));
			int cmd = atoi((char*)tempCmd.c_str());
			if (cmd <= OP_ACS_MIN || cmd >= OP_ACS_MAX){
				Logger::Stream(LEVEL_ERROR,tags) << "Unable to Execute ACS Command, command does not exist";
			}else{
				ACPPacket acpPacket(ACS_SYNC, (ACSOpcode)cmd);
				ACPPacket acpReturn;
				//ACPInterface acp;
				//acp.transaction(acpPacket, acpReturn);
			}
			command = strtok(NULL,",");
		}

	}else if (strcmp(sys, "COM")){
		while (command!= NULL){
			std::string tempCmd = trimNewline(std::string(command));
			int cmd = atoi((char*)tempCmd.c_str());
			if (cmd <= OP_COM_MIN || cmd >= OP_COM_MAX){
				Logger::Stream(LEVEL_ERROR,tags) << "Unable to Execute COM Command, command does not exist";
			}else{
				ACPPacket acpPacket(COM_SYNC, (COMOpcode)cmd);
				ACPPacket acpReturn;
				//ACPInterface acp;
				//acp.transaction(acpPacket, acpReturn);
			}
			command = strtok(NULL,",");
		}

	}else if (strcmp(sys, "EPS")){
		while (command != NULL){
			std::string tempCmd = trimNewline(std::string(command));
			int cmd = atoi((char*)tempCmd.c_str());
			if (cmd <= OP_EPS_MIN || cmd >= OP_EPS_MAX){
				Logger::Stream(LEVEL_ERROR,tags) << "Unable to Execute EPS Command, command does not exist";
			}else{
				ACPPacket acpPacket(EPS_SYNC, (EPSOpcode)cmd);
				ACPPacket acpReturn;
				//ACPInterface acp;
				//acp.transaction(acpPacket, acpReturn);

			}
			command = strtok(NULL,",");
		}
	}else if (strcmp(sys, "RAD")){
		while (command != NULL){
			std::string tempCmd = trimNewline(std::string(command));
			int cmd = atoi((char*)tempCmd.c_str());
			if (cmd <= OP_PLD_CMD_MIN || cmd >= OP_PLD_CMD_MAX){
				Logger::Stream(LEVEL_ERROR,tags) << "Unable to Execute RAD Command, command does not exist";
			}else{
				ACPPacket acpPacket(RAD_SYNC, (PLDOpcode)cmd);
				ACPPacket acpReturn;
				//ACPInterface acp;
				//acp.transaction(acpPacket, acpReturn);
			}
			command = strtok(NULL,",");
		}
	}*/
}

void GroundCommunication::parseFileListRequest(char *line){
	char * dir = strtok(NULL, ",");
	if (dir == NULL){
		//log error
	}

	while (dir != NULL){

		dir = strtok(NULL, ",");
	}
}


void GroundCommunication::parseIEF(){

	std::vector<char*> requests = FileManager::parseGroundFile(IEF_PATH);
	char * type;
	char * line;

	while (!requests.empty()){
		line = requests.front();
		type = strtok(line,",");
		if (type == NULL){
			continue;
		}else if (strcmp(type,"CMD")){
			parseCommandRequest(line);
		}else if (strcmp(type,"DWL")){
			parseDownlinkRequest(line);
		}else if (strcmp(type,"DLT")){
			parseDeletionRequest(line);
		}else if (strcmp(type, "DFL")){
			parseFileListRequest(line);
		}
		requests.erase(requests.begin());
	}

	FileManager::deleteFile(IEF_PATH);
}


void GroundCommunication::parsePPE(){
	std::vector<char*> requests = FileManager::parseGroundFile(PPE_PATH);
	char * type;
	char * line;

	while (!requests.empty()){
		line = requests.front();
		type = strtok(line,",");
		if (type == NULL){
			continue;
		}else if (strcmp(type,"CMD")){
			parseCommandRequest(line);
		}else if (strcmp(type,"DWL")){
			parseDownlinkRequest(line);
		}else if (strcmp(type,"DLT")){
			parseDeletionRequest(line);
		}else if (strcmp(type, "DFL")){
			parseFileListRequest(line);
		}
		requests.erase(requests.begin());
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



