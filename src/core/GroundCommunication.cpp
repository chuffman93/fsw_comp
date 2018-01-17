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

GroundCommunication::GroundCommunication(){
	tags += LogTags("Name", "GroundCommunication");
}

void GroundCommunication::handleScheduling(){
	ScheduleManager sch;

	if (FileManager::checkExistance(NEW_DEFAULT_SCH)){
		Logger::Stream(LEVEL_INFO,tags) << "Updating Default Schedule";
		FileManager::moveFile(NEW_DEFAULT_SCH, DEFAULT_SCH);
	}

	if (FileManager::checkExistance(NEW_SCH)){
		Logger::Stream(LEVEL_INFO,tags) << "Loading New Schedule";
		sch.loadSchedule(NEW_SCH);
	}else{
		Logger::Stream(LEVEL_INFO,tags) << "Loading Default Schedule";
		sch.loadSchedule(DEFAULT_SCH);
	}

}

void GroundCommunication::downlinkFiles(){
	std::string file = DownlinkQueue.front();
	while (!DownlinkQueue.empty()){
		if (!FileManager::checkExistance(file)){
			Logger::Stream(LEVEL_INFO,tags) << "Downlinking Next File";
			DownlinkQueue.pop();
			file = DownlinkQueue.front();
			FileManager::copyFile(file, DOWNLINK_DIRECTORY + file);
		}
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
	char * file = strtok(line, ",");
	if (file == NULL){
		//log error
	}
}

void GroundCommunication::parseDeletionRequest(char *line){
	char * file = strtok(line, ",");
	if (file == NULL){
		//log error
	}
}

void GroundCommunication::parseCommandRequest(char *line){
	char * sys = strtok(line,",");
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
		std::string cmd = trimNewline(std::string(command));
		command = (char*) cmd.c_str();
		Logger::Stream(LEVEL_INFO,tags) << "Executing System Command: " << command;
		int16_t ret = system(command);
		Logger::Stream(LEVEL_INFO,tags) << "System Command Return: " << ret;

	}else if (strcmp(sys,"FSW")){
		std::string tempCmd = trimNewline(std::string(command));
		int cmd = atoi((char*)tempCmd.c_str());
		if (cmd <= 0 || cmd >= FSW_CMD_MAX) {
			Logger::Stream(LEVEL_ERROR,tags) << "Unable to Execute FSW Command, command does not exist";
		}else{
			Logger::Stream(LEVEL_INFO,tags) << "Executing FSW Command" << (FSWCommandTypes)cmd;
			executeFSWCommand(cmd);
		}
	}else if (strcmp(sys, "ACS")){
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

	}else if (strcmp(sys, "COM")){
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

	}else if (strcmp(sys, "EPS")){
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
	}else if (strcmp(sys, "RAD")){
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

	}
}

void GroundCommunication::parseFileListRequest(char *line){
	char * dir = strtok(line, ",");
	if (dir == NULL){
		//log error
	}
}



