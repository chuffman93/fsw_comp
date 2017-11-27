/*
 * CMDStdTasks.cpp
 *
 *  Created on: May 21, 2013
 *      Author: Brian Campuzano
 *
 *  Updated: 12/20/16
 *  Jack Dinkel and Alex St. Clair
 *
 *  Updated: 3/21/16
 *  Alex St. Clair (I apologize for how messy this code is)
 *
 *  Updated: 10/14/16
 *  Alexandra Paquin
 */

#include "servers/CMDServer.h"
#include "servers/SCHServer.h"
#include "servers/CMDStdTasks.h"
#include "servers/COMServer.h"
#include "servers/CDHServer.h"
#include "servers/EPSServer.h"
#include "servers/ACSServer.h"
#include "servers/GPSServer.h"
#include "servers/PLDServer.h"
#include "servers/DispatchStdTasks.h"
#include "servers/FileSystem.h"
#include "core/StdTypes.h"
#include "core/Singleton.h"
#include "core/Factory.h"
#include "core/WatchdogManager.h"
#include "core/ModeManager.h"
#include "util/Logger.h"
#include "util/TLM.h"
#include <termios.h>
#include <string>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>

using namespace AllStar::Core;
using namespace std;

namespace AllStar{
namespace Servers{

void uftpSetup(void) {
	Logger * logger = static_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));

	char portname[20] = "/dev/ttyS2";
	struct termios port;
	int portfd = open(portname, O_RDWR | O_NOCTTY | O_NDELAY);

	if(portfd == -1){
		logger->Fatal("CMDServer: Failed to open serial port!");
	}

	if(tcgetattr(portfd, &port) < 0) {
		logger->Error("CMDServer: Problem with initial port attributes.");
	}

	port.c_iflag &= ~IXON;

	if(cfsetispeed(&port, B115200) < 0 || cfsetospeed(&port, B115200) < 0){
		logger->Fatal("GPSServer: Problem setting the baud rate!");
	}

	if(tcsetattr(portfd, TCSANOW, &port) < 0){
		logger->Error("GPSServer: Problem setting port attributes!");
	}

	// FIXME: Handle these errors
	// FIXME: Handle these errors pertaining to already bind device on reset

	//  Switch to SLIP from KISS
	// ---------------------------KISS & SLIP------------------------------------
	// system(KISSATTACH_PATH " /dev/ttyS2 radio 1.1.1.1");
	int a = system("$(ps aux | grep '[s]l0' | awk '{print $2}') 2>/dev/null");
	if(a != 0){
		logger->Warning("CMDStdTasks: UFTP bind found and killed");
		system("kill $(ps aux | grep '[s]l0' | awk '{print $2}')"); // kill uftp process if still initialized to avoid attempt at re-bind
	}
	logger->Info("CMDStdTasks: UFTP bind not found and formed");

	system("slattach -L -m -s 115200 -p slip /dev/ttyS2 &");
	system("ifconfig sl0 1.1.1.1 pointopoint 1.1.1.2 mtu 256 multicast up");


	// ------------------------UFTP Daemon---------------------------------
	// http://uftp-multicast.sourceforge.net/client_usage.txt
	system(UFTPD_PATH " -I sl0 -E -t -D " UPLINK_DIRECTORY);
}

bool openIEF(FILE ** fp, char ** line, size_t * len) {
	Logger * logger = static_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
	ssize_t bytesRead;

	// check if an IEF file has been uplinked
	if (access(IEF_PATH, F_OK) != 0) {
		logger->Error("CMDStdTasks: no IEF");
		return false;
	}

	*fp = fopen(IEF_PATH, "r");
	if (fp == NULL) {
		logger->Error("CMDStdTasks: error opening IEF");
		TLM_IEF_BAD_OPEN();
		remove(IEF_PATH);
		return false;
	}

	// check password
	bytesRead = getline(line, len, *fp);
	if (strcmp(*line,UPLK_PASSWORD) != 0) {
		logger->Error("CMDStdTasks: invalid IEF password");
		TLM_IEF_BAD_PASSWORD();
		fclose(*fp);
		remove(IEF_PATH);
		return false;
	}

	return true;
}

// returns false if EOF is encountered
bool parseIEFLine(FILE * fp, char ** line, size_t * len, uint8 lineNum) {
	CMDServer * cmdServer = static_cast<CMDServer *> (Factory::GetInstance(CMD_SERVER_SINGLETON));
	Logger * logger = static_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
	size_t bytesRead;
	char * type;
	char * command;
	char * arch;
	char * dir;
	char * num;
	char * regex;
	int numFiles;

	bytesRead = getline(line, len, fp);
	if (bytesRead == -1) {
		return false; // end of file
	}

	// ---- Parse line
	type = strtok(*line,",");
	if (type == NULL) {
		logger->Warning("Incomplete IEF line at type");
		TLM_IEF_INCOMPLETE_TYPE(lineNum);
		return true;
	}

	// SYS command
	if (strcmp(type,"SYS") == 0) { // shell command
		command = strtok(NULL,",");
		if (command == NULL) {
			logger->Warning("Incomplete IEF line at command");
			TLM_IEF_INCOMPLETE_CMD(lineNum);
			return true;
		}

		// get rid of the newline
		string cmd = trimNewline(string(command));
		command = (char *) cmd.c_str();

		// execute the system command
		logger->Info("Executing IEF system call");
		cmdServer->wdmAsleep(); // set to asleep due to asynchronous call
		int16 ret = system(command);
		cmdServer->wdmAlive();
		TLM_SYS_CALL(ret);
		return true;

	// FSW command
	} else if(strcmp(type,"FSW") == 0) { // fsw internal command
		command = strtok(NULL,",");
		if (command == NULL) {
			logger->Warning("Incomplete IEF line at command");
			TLM_IEF_INCOMPLETE_CMD(lineNum);
			return true;
		}

		int cmd = atoi(command);
		if (cmd <= 0 || cmd >= FSW_CMD_MAX) {
			logger->Warning("Invalid IEF command number");
			TLM_IEF_INVALID_CNUMBER(lineNum);
			return true;
		}

		// call a function to find and execute the command
		executeFSWCommand(cmd, lineNum);
		return true;

	// Downlink
	} else if(strcmp(type,"DWN") == 0) {
		// ---- Parse line
		arch = strtok(NULL,",");
		if (arch == NULL) {
			logger->Warning("Incomplete IEF line at archive");
			TLM_IEF_INCOMPLETE_ARCHIVE(lineNum);
			return true;
		}

		dir = strtok(NULL,",");
		if (dir == NULL) {
			logger->Warning("Incomplete IEF line at directory");
			TLM_IEF_INCOMPLETE_DIRECTORY(lineNum);
			return true;
		}

		num = strtok(NULL,",");
		if (num == NULL) {
			logger->Warning("Incomplete IEF line at number");
			TLM_IEF_INCOMPLETE_NUMBER(lineNum);
			return true;
		}
		numFiles = atoi(num);
		if (numFiles < 1) {
			logger->Warning("Invalid IEF line at number");
			TLM_IEF_INVALID_NUMBER(lineNum);
			return true;
		}

		regex = strtok(NULL,",");
		if (regex == NULL) {
			logger->Warning("Incomplete IEF line at regex");
			TLM_IEF_INCOMPLETE_REGEX(lineNum);
			return true;
		}

		// get rid of the newline
		string rgx = trimNewline(string(regex));
		regex = (char *) rgx.c_str();

		// ---- Check if there is a regex
		if (strcmp(regex,"X") == 0)
			regex = (char *) "";

		// ---- Prepend the downlink directory path to the archive name
		string archive = string(IMMED_DIRECTORY) + "/" + string(arch);

		// ---- Call the function to gather and create a tarball from the files
		packageFiles((char *) archive.c_str(), dir, regex, numFiles, lineNum); // TODO: error check this?

		cmdServer->DownlinkFile(archive);
		return true;
	//if Subsystem command
		}else if (strcmp(type,"SUB") == 0) {
				char * subsystem = strtok(NULL,",");
				if (subsystem == NULL) {
					logger->Warning("Incomplete IEF line at command");
					TLM_IEF_INCOMPLETE_CMD(lineNum);
					return true;
			 }
			command = strtok(NULL,",");
			if (command == NULL) {
				logger->Warning("Incomplete IEF line at command");
				TLM_IEF_INCOMPLETE_CMD(lineNum);
				return true;
			}
			int cmd = atoi(command);
					char * messagebuff = strtok(NULL,",");
					uint8 * msg = NULL;
					if (messagebuff != NULL) {
						msg = (uint8 *)atoi(messagebuff);
			 }
			if (strcmp(subsystem,"ACS") == 0){
				//if (strcmp(command,""))
				if (cmd <= 0 || cmd >= ACS_OPCODE_MAX) {
					logger->Warning("Invalid IEF command number");
					TLM_IEF_INVALID_CNUMBER(lineNum);
					return true;
				}
				if (msg == NULL){
					ACPPacket * opcode = new ACPPacket(SERVER_LOCATION_ACS, HARDWARE_LOCATION_ACS, cmd);
					ACPPacket * ret = DispatchPacket(opcode);
				}else{
					uint16 buffLength = sizeof(msg);
					ACPPacket * opcode = new ACPPacket(SERVER_LOCATION_ACS, HARDWARE_LOCATION_ACS, cmd, buffLength, msg);
					ACPPacket * ret = DispatchPacket(opcode);
				}
			}else if (strcmp(subsystem, "COM") == 0){
				if (cmd <= 0 || cmd >= COM_OPCODE_MAX) {
					logger->Warning("Invalid IEF command number");
					TLM_IEF_INVALID_CNUMBER(lineNum);
					return true;
				}
				if (msg == NULL){
					ACPPacket * opcode = new ACPPacket(SERVER_LOCATION_COM, HARDWARE_LOCATION_COM, cmd);
					ACPPacket * ret = DispatchPacket(opcode);
				}else{
					uint16 buffLength = sizeof(msg);
					ACPPacket * opcode = new ACPPacket(SERVER_LOCATION_COM, HARDWARE_LOCATION_COM, cmd, buffLength, msg);
					ACPPacket * ret = DispatchPacket(opcode);
				}
			}else if (strcmp(subsystem, "EPS") == 0){
				if (cmd <= 0 || cmd >= EPS_CMD_MAX) {
					logger->Warning("Invalid IEF command number");
				TLM_IEF_INVALID_CNUMBER(lineNum);
					return true;
				}
				if (msg == NULL){
					ACPPacket * opcode = new ACPPacket(SERVER_LOCATION_EPS, HARDWARE_LOCATION_EPS, cmd);
					ACPPacket * ret = DispatchPacket(opcode);
				}else{
					uint16 buffLength = sizeof(msg);
					ACPPacket * opcode = new ACPPacket(SERVER_LOCATION_EPS, HARDWARE_LOCATION_EPS, cmd, buffLength, msg);
					ACPPacket * ret = DispatchPacket(opcode);
				}
			}else if (strcmp(subsystem, "PLD") == 0){
				if (cmd <= 0 || cmd >= PLD_CMD_MAX) {
					logger->Warning("Invalid IEF command number");
					TLM_IEF_INVALID_CNUMBER(lineNum);
					return true;
				}
				if (msg == NULL){
					ACPPacket * opcode = new ACPPacket(SERVER_LOCATION_PLD, HARDWARE_LOCATION_PLD, cmd);
					ACPPacket * ret = DispatchPacket(opcode);
				}else{
					uint16 buffLength = sizeof(msg);
					ACPPacket * opcode = new ACPPacket(SERVER_LOCATION_PLD, HARDWARE_LOCATION_PLD, cmd, buffLength, msg);
					ACPPacket * ret = DispatchPacket(opcode);
				}
			}else{
				logger->Error("CMDStdTasks: Invalid IEF subsystem");
			}
		}else {
	 		logger->Warning("IEF: unknown command type");
	 		TLM_IEF_UNKNOWN_COMMAND(lineNum);
	 		return true;
	}
}

void parseDRF(void) {
	Logger * logger = static_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
	FILE * fp;
	char * line = NULL;
	size_t len = 0;
	ssize_t bytesRead;

	// check if a DRF file has been uplinked
	if(access(DRF_PATH, F_OK) != 0){
		logger->Debug("CMDStdTasks: no DRF");
		return;
	}

	// open the files to downlink file
	fp = fopen(DRF_PATH, "r");
	if (fp == NULL){
		logger->Error("CMDStdTasks: error opening DRF");
		TLM_DRF_BAD_OPEN();
		remove(DRF_PATH);
		return;
	}

	// check password
	bytesRead = getline(&line, &len, fp);
	if(strcmp(line,UPLK_PASSWORD) != 0){
		logger->Error("CMDStdTasks: invalid DRF password");
		TLM_DRF_BAD_PASSWORD();
		fclose(fp);
		remove(DRF_PATH);
		return;
	}

	// parse the file line by line
	uint8 lineNum = 0;
	char * token;
	char parseArray[200];
	while ((bytesRead = getline(&line, &len, fp)) != -1) {
		strcpy(parseArray, line);
		token = strtok(line,",");
		if (strcmp(token, "RAD") == 0) {
			prepRADDownlink(parseArray, lineNum++);
		} else {
			prepDataDownlink(parseArray, lineNum++);
		}
	}

	fclose(fp);

	logger->Info("Finished DRF");
}

void prepRADDownlink(char * line, uint8 lineNum) {
	Logger * logger = static_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
	char * rad;
	char * passNumStr;
	char * startStr;
	char * endStr;
	int passNum, startChunk, endChunk;

	// ---- Parse the line
	rad = strtok(line, ",");
	if (rad == NULL || strcmp(rad, "RAD") != 0) {
		logger->Warning("DRF: error at RAD");
		TLM_DRF_INCOMPLETE_RAD(lineNum);
		return;
	}

	passNumStr = strtok(NULL, ",");
	if (passNumStr == NULL) {
		logger->Warning("DRF: incomplete at pass num");
		TLM_DRF_INCOMPLETE_PASSNUM(lineNum);
		return;
	}

	startStr = strtok(NULL, ",");
	if (startStr == NULL) {
		logger->Warning("DRF: incomplete at start num");
		TLM_DRF_INCOMPLETE_STARTNUM(lineNum);
		return;
	}

	endStr = strtok(NULL, ",");
	if (endStr == NULL) {
		logger->Warning("DRF: incomplete at end num");
		TLM_DRF_INCOMPLETE_ENDNUM(lineNum);
		return;
	}

	// get rid of the newline
	string endc = trimNewline(string(endStr));
	endStr = (char *) endc.c_str();

	// convert to integers
	passNum = atoi(passNumStr);
	if(passNum < 1){
		logger->Warning("Invalid DRF line at pass num");
		TLM_DRF_INVALID_PASSNUM(lineNum);
		return; // skip to the next line
	}

	startChunk = atoi(startStr);
	if(startChunk < 0){
		logger->Warning("Invalid DRF line at start chunk");
		TLM_DRF_INVALID_STARTNUM(lineNum);
		return; // skip to the next line
	}

	endChunk = atoi(endStr);
	if(endChunk < 0){
		logger->Warning("Invalid DRF line at end chunk");
		TLM_DRF_INVALID_ENDNUM(lineNum);
		return; // skip to the next line
	}

	// copy the appropriate files
	char command[200];
	for (int i = startChunk; i <= endChunk; i++) {
		sprintf(command, "cp " RAD_FILE_PATH "/RAD_%d.tar.gz.%03d " DOWNLINK_DIRECTORY "/", passNum, i);
		system(command);
	}
}

void prepDataDownlink(char * line, uint8 lineNum) {
	Logger * logger = static_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
	char * arch;
	char * dir;
	char * num;
	int numFiles;
	char * regex;

	// ---- Parse the line
	arch = strtok(line,",");
	if(arch == NULL){
		logger->Warning("Incomplete DRF line at archive");
		TLM_DRF_INCOMPLETE_ARCHIVE(lineNum);
		return; // skip to the next line
	}

	dir = strtok(NULL,",");
	if(dir == NULL){
		logger->Warning("Incomplete DRF line at directory");
		TLM_DRF_INCOMPLETE_DIRECTORY(lineNum);
		return; // skip to the next line
	}

	num = strtok(NULL,",");
	if(num == NULL){
		logger->Warning("Incomplete DRF line at number");
		TLM_DRF_INCOMPLETE_NUMBER(lineNum);
		return; // skip to the next line
	}
	numFiles = atoi(num);
	if(numFiles < 1){
		logger->Warning("Invalid DRF line at number");
		TLM_DRF_INVALID_NUMBER(lineNum);
		return; // skip to the next line
	}

	regex = strtok(NULL,",");
	if(regex == NULL){
		logger->Warning("Incomplete DRF line at regex");
		TLM_DRF_INCOMPLETE_REGEX(lineNum);
		return; // skip to the next line
	}

	// get rid of the newline
	string rgx = trimNewline(string(regex));
	regex = (char *) rgx.c_str();

	// ---- Check if there is a regex
	if(strcmp(regex,"X") == 0)
		regex = (char *) "";

	// ---- Prepend the downlink directory path to the archive name
	string archive = string(DOWNLINK_DIRECTORY) + "/" + string(arch);

	// ---- Call the function to gather and create a tarball from the files
	packageFiles((char *) archive.c_str(), dir, regex, numFiles, lineNum); // TODO: error check this?

	// ---- Split the tarball into chunks
	CMDServer * cmdServer = static_cast<CMDServer *> (Factory::GetInstance(CMD_SERVER_SINGLETON));
	char chunkSize[16];
	sprintf(chunkSize, "%lu", cmdServer->CMDConfiguration.fileChunkSize);
	string split_cmd = "split -b " + string(chunkSize) + " -d -a 3 " + archive + " " + archive + ".";
	if(system(split_cmd.c_str()) == -1){
		logger->Error("Failed to split DRF archive into chunks");
		TLM_DRF_FAILED_SPLIT(lineNum);
	}

	// ---- Delete the full archive
	string del_cmd = "rm " + archive;
	if(system(del_cmd.c_str()) == -1){
		logger->Error("Failed to delete DRF archive");
		TLM_DRF_FAILED_DELETE(lineNum);
	}
}

void parseDLT(void) {
	Logger * logger = static_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
	FILE * fp;
	char * line = NULL;
	size_t len = 0;
	ssize_t bytesRead;

	// check if a DLT file has been uplinked
	if(access(DLT_PATH, F_OK) != 0){
		logger->Debug("CMDStdTasks: no DLT");
		return;
	}

	// open the files to downlink file
	fp = fopen(DLT_PATH, "r");
	if (fp == NULL){
		logger->Error("CMDStdTasks: error opening DLT");
		TLM_DLT_BAD_OPEN();
		remove(DLT_PATH);
		return;
	}

	// check password
	bytesRead = getline(&line, &len, fp);
	if(strcmp(line,UPLK_PASSWORD) != 0){
		logger->Error("CMDStdTasks: invalid DLT password");
		TLM_DLT_BAD_PASSWORD();
		fclose(fp);
		remove(DLT_PATH);
		return;
	}

	// parse the file line by line
	uint8 lineNum = 0;
	char * dir;
	char * num;
	int numFiles;
	char * regex;
	while ((bytesRead = getline(&line, &len, fp)) != -1) {
		lineNum++;

		// ---- Parse line
		dir = strtok(line,",");
		if(dir == NULL){
			logger->Warning("Incomplete DLT line at dir");
			TLM_DLT_INCOMPLETE_DIRECTORY(lineNum);
			continue; // skip to the next line
		}

		num = strtok(NULL,",");
		if(num == NULL){
			logger->Warning("Incomplete DLT line at number");
			TLM_DLT_INCOMPLETE_NUMBER(lineNum);
			continue; // skip to the next line
		}
		numFiles = atoi(num);
		if(numFiles < -1 || numFiles == 0){
			logger->Warning("Invalid DLT line at number");
			TLM_DLT_INVALID_NUMBER(lineNum);
			continue;
		}

		regex = strtok(NULL,",");
		if(regex == NULL){
			logger->Warning("Incomplete DLT line at regex");
			TLM_DLT_INCOMPLETE_REGEX(lineNum);
			continue; // skip to the next line
		}

		// get rid of the newline
		string rgx = trimNewline(string(regex));
		regex = (char *) rgx.c_str();

		// ---- find deletion scheme and delete
		if(strcmp(regex,"X") == 0 && numFiles != -1){
			deleteOldest(dir, numFiles, lineNum);
		}else if(!(strcmp(regex,"X") == 0) && numFiles == -1){
			deleteRegex(dir,regex, lineNum);
		}else{
			logger->Warning("DLT: Invalid deletion command");
			TLM_DLT_INVALID_COMMAND(lineNum);
		}
	}

	fclose(fp);

	logger->Info("Finished DLT");
}

void parsePPE(void) {
	CMDServer * cmdServer = static_cast<CMDServer *> (Factory::GetInstance(CMD_SERVER_SINGLETON));
	Logger * logger = static_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
	FILE * fp;
	char * line = NULL;
	size_t len = 0;
	ssize_t bytesRead;

	// check if a PPE file has been uplinked
	if(access(PPE_PATH, F_OK) != 0){
		logger->Debug("CMDStdTasks: no PPE");
		return;
	}

	// open the files to downlink file
	fp = fopen(PPE_PATH, "r");
	if (fp == NULL){
		logger->Error("CMDStdTasks: error opening PPE");
		TLM_PPE_BAD_OPEN();
		remove(PPE_PATH);
		return;
	}

	// check password
	bytesRead = getline(&line, &len, fp);
	if(strcmp(line,UPLK_PASSWORD) != 0){
		logger->Error("CMDStdTasks: invalid PPE password");
		TLM_PPE_BAD_PASSWORD();
		fclose(fp);
		remove(PPE_PATH);
		return;
	}

	// parse the file line by line
	uint8 lineNum = 0;
	char * type;
	char * command;
	while ((bytesRead = getline(&line, &len, fp)) != -1) {
		lineNum++;

		// ---- Parse line
		type = strtok(line,",");
		if(type == NULL){
			logger->Warning("Incomplete PPE line at type");
			TLM_PPE_INCOMPLETE_TYPE(lineNum);
			continue; // skip to the next line
		}

		// check the type of command and perform accordingly
		if(strcmp(type,"SYS") == 0){ // shell command
			command = strtok(NULL,",");
			if(command == NULL){
				logger->Warning("Incomplete PPE line at command");
				TLM_PPE_INCOMPLETE_COMMAND(lineNum);
				continue; // skip to the next line
			}

			// get rid of the newline
			string cmd = trimNewline(string(command));
			command = (char *) cmd.c_str();

			// execute the system command
			cmdServer->wdmAsleep(); // set to asleep due to asynchronous call
			int16 ret = system(command);
			cmdServer->wdmAlive();
			TLM_SYS_CALL(ret);
		}else if(strcmp(type,"FSW") == 0){ // fsw internal command
			command = strtok(NULL,",");
			if(command == NULL){
				logger->Warning("Incomplete PPE line at command");
				TLM_PPE_INCOMPLETE_COMMAND(lineNum);
				continue; // skip to the next line
			}

			int cmd = atoi(command);
			if(cmd == 0 || cmd < 0 || cmd >= FSW_CMD_MAX){
				logger->Warning("Invalid PPE command number");
				TLM_PPE_INVALID_NUMBER(lineNum);
				continue; // skip to the next line
			}

			// call a function to find and execute the command
			executeFSWCommand(cmd, lineNum);
		//if Subsystem command
		}else if (strcmp(type,"SUB") == 0) {
				char * subsystem = strtok(NULL,",");
				if (subsystem == NULL) {
					logger->Warning("Incomplete PPE line at command");
					TLM_IEF_INCOMPLETE_CMD(lineNum);
					continue;
				}
				command = strtok(NULL,",");
				if (command == NULL) {
					logger->Warning("Incomplete PPE line at command");
					TLM_IEF_INCOMPLETE_CMD(lineNum);
					continue;
				}
				int cmd = atoi(command);
				char * messagebuff = strtok(NULL,",");
				uint8 * msg = NULL;
				if (messagebuff != NULL) {
					msg = (uint8 *)atoi(messagebuff);
				}
				if (strcmp(subsystem,"ACS") == 0){
					//if (strcmp(command,""))
					if (cmd <= 0 || cmd >= ACS_OPCODE_MAX) {
						logger->Warning("Invalid PPE command number");
						TLM_IEF_INVALID_CNUMBER(lineNum);
						continue;
					}
					if (msg == NULL){
						ACPPacket * opcode = new ACPPacket(SERVER_LOCATION_ACS, HARDWARE_LOCATION_ACS, cmd);
						ACPPacket * ret = DispatchPacket(opcode);
					}else{
						uint16 buffLength = sizeof(messagebuff);
						ACPPacket * opcode = new ACPPacket(SERVER_LOCATION_ACS, HARDWARE_LOCATION_ACS, cmd, buffLength, msg);
						ACPPacket * ret = DispatchPacket(opcode);
					}
				}else if (strcmp(subsystem, "COM") == 0){
					if (cmd <= 0 || cmd >= COM_OPCODE_MAX) {
						logger->Warning("Invalid PPE command number");
						TLM_IEF_INVALID_CNUMBER(lineNum);
						continue;
					}
					if (msg == NULL){
						ACPPacket * opcode = new ACPPacket(SERVER_LOCATION_COM, HARDWARE_LOCATION_COM, cmd);
						ACPPacket * ret = DispatchPacket(opcode);
					}else{
						uint16 buffLength = sizeof(messagebuff);
						ACPPacket * opcode = new ACPPacket(SERVER_LOCATION_COM, HARDWARE_LOCATION_COM, cmd, buffLength, msg);
						ACPPacket * ret = DispatchPacket(opcode);
					}
				}else if (strcmp(subsystem, "EPS") == 0){
					if (cmd <= 0 || cmd >= EPS_CMD_MAX) {
						logger->Warning("Invalid PPE command number");
						TLM_IEF_INVALID_CNUMBER(lineNum);
						continue;
					}
					if (msg == NULL){
						ACPPacket * opcode = new ACPPacket(SERVER_LOCATION_EPS, HARDWARE_LOCATION_EPS, cmd);
						ACPPacket * ret = DispatchPacket(opcode);
					}else{
						uint16 buffLength = sizeof(messagebuff);
						ACPPacket * opcode = new ACPPacket(SERVER_LOCATION_EPS, HARDWARE_LOCATION_EPS, cmd, buffLength, msg);
						ACPPacket * ret = DispatchPacket(opcode);
					}
				}else if (strcmp(subsystem, "PLD") == 0){
					if (cmd <= 0 || cmd >= PLD_CMD_MAX) {
						logger->Warning("Invalid PPE command number");
						TLM_IEF_INVALID_CNUMBER(lineNum);
						continue;
					}
					if (msg == NULL){
						ACPPacket * opcode = new ACPPacket(SERVER_LOCATION_PLD, HARDWARE_LOCATION_PLD, cmd);
						ACPPacket * ret = DispatchPacket(opcode);
					}else{
						uint16 buffLength = sizeof(messagebuff);
						ACPPacket * opcode = new ACPPacket(SERVER_LOCATION_PLD, HARDWARE_LOCATION_PLD, cmd, buffLength, msg);
						ACPPacket * ret = DispatchPacket(opcode);
					}
				}else{
					logger->Error("CMDStdTasks: Invalid PPE subsystem");
				}
		}else{
			logger->Warning("PPE: unknown command type");
		}

	fclose(fp);

	logger->Info("Finished PPE");
}
}

void parseDFL(void){	CMDServer * cmdServer = static_cast<CMDServer *> (Factory::GetInstance(CMD_SERVER_SINGLETON));
	Logger * logger = static_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
	FILE * fp;
	char * line = NULL;
	size_t len = 0;
	ssize_t bytesRead;

	// check if a PPE file has been uplinked
	if(access(DFL_PATH, F_OK) != 0){
		logger->Debug("CMDStdTasks: no DFL");
		return;
	}

	// open the files to downlink file
	fp = fopen(DFL_PATH, "r");
	if (fp == NULL){
		logger->Error("CMDStdTasks: error opening DFL");
		//TLM_PPE_BAD_OPEN();
		remove(DFL_PATH);
		return;
	}

	// check password
	bytesRead = getline(&line, &len, fp);
	if(strcmp(line,UPLK_PASSWORD) != 0){
		logger->Error("CMDStdTasks: invalid DFL password");
		//TLM_PPE_BAD_PASSWORD();
		fclose(fp);
		remove(DFL_PATH);
		return;
	}

	remove(DFL_DOWN);

	// parse the file line by line
	while ((bytesRead = getline(&line, &len, fp)) != -1) {
		line = strtok(line,"\n");
		DIR * dir = opendir(line);
		cout << line << endl;
		if (dir){
			 getFilesList(line);
		}else if (ENOENT == errno){
			logger->Warning("DFL: unknown path to folder");
		}else {
			logger->Warning("DFL: unable to access path to folder");
		}
	}
	fclose(fp);

	logger->Info("Finished DFL preparation");

}

// A primary responsibility of this function is to deal with configuration updates.
// Before replacing old configurations, it checks the validity of the new configs.
void processUplinkFiles(void) {
	Logger * logger = static_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));

	// Move new schedule from the uplink directory to its final directory
	if (access(SCH_UP, F_OK) != -1) {
		rename(SCH_UP, SCHEDULE_FILE);
	} else {
		logger->Debug("CMDStdTasks: no SCH");
	}

	// Update Config Schedule
	if (access(SCH_CFG_UP, F_OK) != -1) {
		SCHServer * schServer = static_cast<SCHServer *> (Factory::GetInstance(SCH_SERVER_SINGLETON));
		if(schServer->LoadDefaultScheduleConfigurations((char *) SCH_CFG_UP)) {
			rename(SCH_CFG_UP, SCH_CONFIG);
		} else {
			remove(SCH_CFG_UP);
		}
	}

	// Update ACS config
	if (access(ACS_CFG_UP, F_OK) != -1) {
		ACSServer * acsServer = static_cast<ACSServer *> (Factory::GetInstance(ACS_SERVER_SINGLETON));
		if(acsServer->updateConfig()) {
			rename(ACS_CFG_UP, ACS_CONFIG);
		} else {
			remove(ACS_CFG_UP);
		}
	}

	// Update CDH config
	if (access(CDH_CFG_UP, F_OK) != -1) {
		remove(CDH_CFG_UP);
		//cdhServer->updateConfig();
		logger->Error("CMDStdTasks: CDH config not implemented");
	}

	// Update CMD config
	if (access(CMD_CFG_UP, F_OK) != -1) {
		CMDServer * cmdServer = static_cast<CMDServer *> (Factory::GetInstance(CMD_SERVER_SINGLETON));
		if(cmdServer->updateConfig()) {
			rename(CMD_CFG_UP, CMD_CONFIG);
		} else {
			remove(CMD_CFG_UP);
		}
	}

	// Update COM config
	if (access(COM_CFG_UP, F_OK) != -1) {
		remove(COM_CFG_UP);
		//comServer->updateConfig();
		logger->Error("CMDStdTasks: COM config not implemented");
	}

	// Update EPS config
	if (access(EPS_CFG_UP, F_OK) != -1) {
		EPSServer * epsServer = static_cast<EPSServer *> (Factory::GetInstance(EPS_SERVER_SINGLETON));
		if(epsServer->updateConfig()) {
			rename(EPS_CFG_UP, EPS_CONFIG);
		} else {
			remove(EPS_CFG_UP);
		}
	}

	// Update FMG config
	if (access(FMG_CFG_UP, F_OK) != -1) {
		remove(FMG_CFG_UP);
		//fmgServer->updateConfig();
		logger->Error("CMDStdTasks: FMG config not implemented");
	}

	// Update GPS config
	if (access(GPS_CFG_UP, F_OK) != -1) {
		GPSServer * gpsServer = static_cast<GPSServer *> (Factory::GetInstance(GPS_SERVER_SINGLETON));
		if(gpsServer->updateConfig()) {
			rename(GPS_CFG_UP, GPS_CONFIG);
		} else {
			remove(GPS_CFG_UP);
		}
	}

	// Update PLD config
	if (access(PLD_CFG_UP, F_OK) != -1) {
		remove(PLD_CFG_UP);
		//pldServer->updateConfig();
		logger->Error("CMDStdTasks: PLD config not implemented");
	}
}

string trimNewline(string buf) {
  // Remove the newline at the end of a string
  size_t len = buf.size();
  if (len && (buf[len-1] == '\n')) {
    buf[len-1] = '\0';
  }
  return buf;
}

const long getFileSize(const char * filePath, const char * regex, const int maxFiles){
    Logger * logger = static_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
	FILE * fd;
	int size = 0;

	// sh command to get the total size of all the files we want to compress
	char sh_cmd[248];
	sprintf(sh_cmd, "tot=0; for num in `ls -lr %s | grep ^- | grep \"%s\" | awk '{print $5}' | head -%d`; do tot=$(($tot+$num)); done; echo $tot", filePath, regex, maxFiles);

	// Execute an sh script and pipe the results back to the file descriptor fd
	if(!(fd = popen(sh_cmd, "r"))){
	    logger->Error("GetFileSize: could not verify file sizes");
	    return -1;
	}

	// Grab result of command
	int loop_count = 0;
	char buff[512];
	while(fgets(buff, sizeof(buff), fd)!=NULL){
		if (loop_count > 0) {
			logger->Error("GetFileSize: sh command produced too many results");
			return -2;
		}
	    size = atol(buff);
	    loop_count++;
	}

	if (pclose(fd) == -1){
		logger->Warning("GetFileSize: Error closing file stream");
	}

	return size;
}

const int packageFiles(const char * destination, const char * filePath, const char * regex, const int maxFiles, uint8 lineNum) {
	// This function will tar up `maxFiles` number of files in `filePath` matching the regular expression `regex` into a tarball named `destination`.
    Logger * logger = static_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
	FILE * fd;

	const long size = getFileSize(filePath, regex, maxFiles);

	CMDServer * cmdServer = static_cast<CMDServer *> (Factory::GetInstance(CMD_SERVER_SINGLETON));

	if (size < 0) {
		logger->Error("PackageFiles: Error detected, aborting packaging");
		TLM_DRF_FILESIZE_ERROR(lineNum);
		return -1;
	}
	else if (size > cmdServer->CMDConfiguration.maxDownlinkSize){
		logger->Error("PackageFiles: Total file size is too great to package");
		TLM_DRF_FILESIZE_ERROR(lineNum);
		return -2;
	}

	// sh command to tar the files to package
	char sh_cmd[256];
	sprintf(sh_cmd, "tar -czf %s -C %s `ls -lr %s | grep ^- | awk '{print $9}' | grep \"%s\" | head -%d`", destination, filePath, filePath, regex, maxFiles);

	// Execute a shell script and pipe the results back to the file descriptor fd
	if(!(fd = popen(sh_cmd, "r"))){
	    logger->Error("PackageFiles: Error packaging files");
	    TLM_DRF_PACKAGE_ERROR(lineNum);
	    return -3;
	}

	if (pclose(fd) == -1){
		logger->Warning("PackageFiles: Error closing file stream");
	}

	return 0;
}

int deleteOldest(char * filePath, int numFiles, uint8 lineNum) {
	Logger * logger = static_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
	FILE * fd;

	// sh command to remove the oldest numFiles from filePath
	char sh_cmd[256];
	sprintf(sh_cmd, "rm `ls -l %s | grep ^- | awk '{print \"%s\" \"/\" $9}' | head -%d`", filePath, filePath, numFiles);

	// Execute a shell script and pipe the results back to the file descriptor fd
	if(!(fd = popen(sh_cmd, "r"))){
		logger->Error("DeleteOldest: Error removing files");
		TLM_DLT_REMOVE_ERROR(lineNum);
		return -1;
	}

	if (pclose(fd) == -1){
		logger->Warning("DeleteOldest: Error closing file stream");
	}

	return 0;
}

int deleteRegex(char * filePath, char * regex, uint8 lineNum) {
	Logger * logger = static_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
	FILE * fd;

	// sh command to remove all files from filePath matching regex
	char sh_cmd[256];
	sprintf(sh_cmd, "rm `ls -l %s | grep ^- | awk '{print \"%s\" \"/\" $9}' | grep \"%s\"`", filePath, filePath, regex);

	// Execute a shell script and pipe the results back to the file descriptor fd
	if(!(fd = popen(sh_cmd, "r"))){
		logger->Error("DeleteRegex: Error removing files");
		TLM_DLT_REMOVE_ERROR(lineNum);
		return -1;
	}

	if (pclose(fd) == -1){
		logger->Warning("DeleteRegex: Error closing file stream");
	}

	return 0;
}

int getNumFiles(char * dir) {
	Logger * logger = static_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
	FILE * fd;
	int num = 0;

	// sh command to determine the number of files in the downlink directory
	char sh_cmd[256];
	sprintf(sh_cmd, "ls -l %s | grep ^- | wc -l", dir);

	// Execute a shell script and pipe the results back to the file descriptor fd
	if(!(fd = popen(sh_cmd, "r"))){
		logger->Error("GetNumFilesDWN: Error checking file size");
		return -1;
	}

	// get the number from the result of the command
	int loop_count = 0;
	char buff[512];
	while(fgets(buff, sizeof(buff), fd)!=NULL){
		if (loop_count > 0) {
			logger->Error("GetFileSize: sh command produced too many results");
			return -2;
		}
		num = atoi(buff);
		loop_count++;
	}

	if (pclose(fd) == -1){
		logger->Warning("GetNumFilesDWN: Error closing file stream");
	}

	return num;
}

string getDownlinkFile(int fileNum) {
	Logger * logger = static_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
	FILE * fd;
	char fileName[128];

	// sh command to get the filename for file number fileNum
	char sh_cmd[256];
	sprintf(sh_cmd, "ls -l %s | grep ^- | awk '{print \"%s\" \"/\" $9}' | head -%d | tail -1", DOWNLINK_DIRECTORY, DOWNLINK_DIRECTORY, fileNum);

	// Execute a shell script and pipe the results back to the file descriptor fd
	if(!(fd = popen(sh_cmd, "r"))){
		logger->Error("GetDownlinkFile: Error getting file name");
		TLM_DRF_FILENAME_ERROR();
		return "";
	}

	// get the number from the result of the command
	int loop_count = 0;
	while(fgets(fileName, 128, fd)!=NULL){
		if (loop_count > 0) {
			logger->Error("GetDownlinkFile: sh command produced too many results");
			TLM_DRF_FILENAME_ERROR();
			return "";
		}
		loop_count++;
	}

	if (pclose(fd) == -1){
		logger->Warning("GetNumFilesDWN: Error closing file stream");
	}

	string fn = trimNewline(string(fileName));

	return fn;
}

void executeFSWCommand(int command, uint8 lineNum) {
	Logger * logger = static_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
	COMServer * comServer = static_cast<COMServer *> (Factory::GetInstance(COM_SERVER_SINGLETON));
	SCHServer * schServer = static_cast<SCHServer *> (Factory::GetInstance(SCH_SERVER_SINGLETON));
	CDHServer * cdhServer = static_cast<CDHServer *> (Factory::GetInstance(CDH_SERVER_SINGLETON));

	int ret = 0;
	switch (command) {
	case FSW_CMD_REQUEST_RESET:
		logger->Info("Uplink reset requested");
		TLM_RESET_COMMANDED();
		schServer->RequestReset();
		break;
	case FSW_CMD_HARD_SATELLITE_RESET:
		logger->Error("PPE hard satellite reset, DOES NOT WORK");
		cdhServer->resetAssert(HARDWARE_LOCATION_EPS);
		break;
	case FSW_CMD_TX_SILENCE_START:
		logger->Warning("Commanding transmitter silence");
		TLM_TX_SILENCE_ENTERED();
		comServer->setTxSilence(true);
		break;
	case FSW_CMD_TX_SILENCE_END:
		logger->Warning("Ending transmitter silence");
		TLM_TX_SILENCE_EXITED();
		comServer->setTxSilence(false);
		break;
	case FSW_CMD_CLEAR_DWNLK:
		ret = system("rm -rf " DOWNLINK_DIRECTORY "/*");
		TLM_DWLK_CLEARED(ret);
		break;
	case FSW_CMD_CLEAR_IMMED:
		ret = system("rm -rf " IMMED_DIRECTORY "/*");
		TLM_IMMED_CLEARED(ret);
		break;
	case FSW_CMD_CLEAR_UPLK:
		ret = system("rm -rf " UPLINK_DIRECTORY "/*");
		TLM_UPLK_CLEARED(ret);
		break;
	default:
		logger->Error("Unknown FSW command (bit flip probable)");
		TLM_UNKNOWN_FSW_COMMAND(lineNum);
		break;
	}
}

int checkForSOT(void) {
	Logger * logger = static_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
	if(access(SOT_PATH,F_OK) == 0){
		char * line = NULL;
		size_t len = 0;
		ssize_t read;

		// open the files to downlink file
		FILE * fp = fopen(SOT_PATH, "r");
		if (fp == NULL) {
			logger->Error("CMDStdTasks: error opening SOT");
			remove(SOT_PATH);
			return 0;
		}

		// check the password
		read = getline(&line, &len, fp);
		if (strcmp(line, UPLK_PASSWORD_SKIP) == 0) {
			fclose(fp);
			return 1;
		} else if(strcmp(line, UPLK_PASSWORD_SEND) == 0) {
			fclose(fp);
			return 2;
		}

		logger->Error("CMDStdTasks: SOT bad password");
		fclose(fp);
		remove(SOT_PATH);
	}
	return false;
}

void getFilesList(string dir){
	Logger * logger = static_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));	//research dirent.h for finding file names in folder
	DIR *dp;
	struct dirent *entry;
	int count;

	dp  = opendir(dir.c_str());

	//return if unable to open
	if((dp  = opendir(dir.c_str())) == NULL) {
		logger->Error("CMDStdTasks: error opening DFL directory");
		return;
	}
	count = 0;

	FILE * dwlkDFL = fopen(DFL_DOWN, "a+");
	fwrite(dir.c_str(), strlen(dir.c_str()), 1, dwlkDFL);
	while((entry = readdir(dp)) != NULL){
		fwrite(", ", strlen(", "), 1, dwlkDFL);
		fwrite(entry->d_name, strlen(entry->d_name), 1, dwlkDFL);
		count++;
	}
	closedir(dp);
	fclose(dwlkDFL);

}


} // End of namespace Servers
} // End of namespace AllStar
