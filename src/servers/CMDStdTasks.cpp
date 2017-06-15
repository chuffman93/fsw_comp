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
 */

#include "servers/CMDServer.h"
#include "servers/SCHServer.h"
#include "servers/CMDStdTasks.h"
#include "servers/COMServer.h"
#include "servers/CDHServer.h"
#include "servers/EPSServer.h"
#include "servers/ACSServer.h"
#include "servers/PLDServer.h"
#include "servers/DispatchStdTasks.h"
#include "servers/FileSystem.h"
#include "core/StdTypes.h"
#include "core/Singleton.h"
#include "core/Factory.h"
#include "core/WatchdogManager.h"
#include "core/ModeManager.h"
#include "util/Logger.h"
#include "util/Diagnostics.h"
#include <termios.h>
#include <string>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

using namespace AllStar::Core;
using namespace std;

namespace AllStar{
namespace Servers{

#define NUM_DIAGNOSTIC_TESTS 4

void uftpSetup(void) {
	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));

	char portname[20] = "/dev/ttyS2";
	struct termios port;
	int portfd = open(portname, O_RDWR | O_NOCTTY | O_NDELAY);

	if(portfd == -1){
		logger->Log(LOGGER_LEVEL_FATAL, "CMDServer: Failed to open serial port!");
	}

	if(tcgetattr(portfd, &port) < 0) {
		logger->Log(LOGGER_LEVEL_ERROR, "CMDServer: Problem with initial port attributes.");
	}

	port.c_iflag &= ~IXON;

	if(cfsetispeed(&port, B115200) < 0 || cfsetospeed(&port, B115200) < 0){
		logger->Log(LOGGER_LEVEL_FATAL, "GPSServer: Problem setting the baud rate!");
	}

	if(tcsetattr(portfd, TCSANOW, &port) < 0){
		logger->Log(LOGGER_LEVEL_ERROR, "GPSServer: Problem setting port attributes!");
	}



	// ---------------------------KISS------------------------------------
	system(KISSATTACH_PATH " /dev/ttyS2 radio 1.1.1.1");
	system("ifconfig ax0 multicast up");

	// send over uftp: /home/root/binaries/uftp -I ax0 file_name -H 1.1.1.2
	// the "-H 1.1.1.2" is an option that makes it so only that IP address can receive the multicast

	// ------------------------UFTP Daemon---------------------------------
	// http://uftp-multicast.sourceforge.net/client_usage.txt
	system(UFTPD_PATH " -I ax0 -E -D " UPLINK_DIRECTORY);
}

// note the filename should be the absolute path
void downlinkFile(string filename) {
	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));

	if(access(filename.c_str(), F_OK) != 0) {
		logger->Log(LOGGER_LEVEL_ERROR, "downlinkFile: file doesn't exist!");
		return;
	}

	char sh_cmd[256];
	sprintf(sh_cmd, "%s -Y aes256-gcm -h sha256 -I ax0 -H 1.1.1.2 -x 1 %s", UFTP_PATH, filename.c_str()); // can add "-H 1.1.1.2" to only downlink to one IP, "-x 1" decreases the log statement verboseness
	system(sh_cmd);
}

void runDiagnostic(void) {
	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
	logger->Log(LOGGER_LEVEL_INFO, "CMDServer: running diagnostics tests");
	FILE * fp = fopen("diagnostics.txt", "r");

	if(fp == NULL){
		logger->Log(LOGGER_LEVEL_WARN, "Error opening diagnostic tests file");

		// TODO: should we run a pre-specified test?
		FILE * fpr = fopen("diagnostic_results.txt", "a+");
		char results[] = "No tests to run\n";
		fwrite(results, 1, sizeof(results), fpr);
		fclose(fpr);
	}else{
		uint8 i = 0;
		bool isActive[NUM_DIAGNOSTIC_TESTS] = {false};
		char c;
		while(i < NUM_DIAGNOSTIC_TESTS){
			c = fgetc(fp);
			if(c == EOF){
				logger->Log(LOGGER_LEVEL_WARN, "Diagnostic tests file too short!");
				break;
			}
			isActive[i] = (c == '1') ? true : false;
			i++;
		}

		FILE * fpr = fopen("diagnostic_results.txt", "a+");
		fprintf(fpr, "Diagnostics Test @ %lld\n", getTimeInMillis());
		fclose(fpr);

		// Conditionally run tests here TODO: rewrite with function pointers
		BusLoadTest(isActive[0]);
		SPIStats(isActive[1]);
		SampleTest2(isActive[2]);
		SampleTest3(isActive[3]);

		fclose(fp);

		// FIXME: decide on deletion/transfer scheme
		//remove("diagnostics.txt");
	}
}

void parseIEF(void) {
	CMDServer * cmdServer = dynamic_cast<CMDServer *> (Factory::GetInstance(CMD_SERVER_SINGLETON));
	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
	FILE * fp;
	char * line = NULL;
	size_t len = 0;
	ssize_t bytesRead;

	// check if an IEF file has been uplinked
	if(access(IEF_PATH, F_OK) != 0){
		logger->Log(LOGGER_LEVEL_ERROR, "CMDStdTasks: no IEF");
		return;
	}

	// open the files to downlink file
	fp = fopen(IEF_PATH, "r");
	if (fp == NULL){
		logger->Log(LOGGER_LEVEL_ERROR, "CMDStdTasks: error opening IEF");
		remove(IEF_PATH);
		return;
	}

	// check password
	bytesRead = getline(&line, &len, fp);
	if(strcmp(line,UPLK_PASSWORD) != 0){
		logger->Log(LOGGER_LEVEL_ERROR, "CMDStdTasks: invalid IEF password");
		fclose(fp);
		remove(IEF_PATH);
		return;
	}

	// parse the file line by line
	char * type;
	char * command;
	char * arch;
	char * dir;
	char * num;
	char * regex;
	int numFiles;
	while ((bytesRead = getline(&line, &len, fp)) != -1) {
		// ---- Parse line
		type = strtok(line,",");
		if(type == NULL){
			logger->Log(LOGGER_LEVEL_WARN, "Incomplete IEF line at type");
			continue; // skip to the next line
		}

		// check the type of command and perform accordingly
		// SYS command
		if(strcmp(type,"SYS") == 0){ // shell command
			command = strtok(NULL,",");
			if(command == NULL){
				logger->Log(LOGGER_LEVEL_WARN, "Incomplete IEF line at command");
				continue; // skip to the next line
			}

			// get rid of the newline
			string cmd = trimNewline(string(command));
			command = (char *) cmd.c_str();

			// execute the system command
			cmdServer->wdmAsleep(); // set to asleep due to asynchronous call
			if(system(command) == -1){
				logger->Log(LOGGER_LEVEL_ERROR, "IEF unable to execute shell command");
			}
			cmdServer->wdmAlive();

		// FSW command
		}else if(strcmp(type,"FSW") == 0){ // fsw internal command
			command = strtok(NULL,",");
			if(command == NULL){
				logger->Log(LOGGER_LEVEL_WARN, "Incomplete IEF line at command");
				continue; // skip to the next line
			}

			int cmd = atoi(command);
			if(cmd <= 0 || cmd > FSW_CMD_MAX){
				logger->Log(LOGGER_LEVEL_WARN, "Invalid IEF command number");
				continue; // skip to the next line
			}

			// call a function to find and execute the command
			executeFSWCommand(cmd);

		// Downlink
		}else if(strcmp(type,"DWN") == 0){
			// ---- Parse line
			arch = strtok(NULL,",");
			if(arch == NULL){
				logger->Log(LOGGER_LEVEL_WARN, "Incomplete IEF line at archive");
				continue; // skip to the next line
			}

			dir = strtok(NULL,",");
			if(dir == NULL){
				logger->Log(LOGGER_LEVEL_WARN, "Incomplete IEF line at directory");
				continue; // skip to the next line
			}

			num = strtok(NULL,",");
			if(num == NULL){
				logger->Log(LOGGER_LEVEL_WARN, "Incomplete IEF line at number");
				continue; // skip to the next line
			}
			numFiles = atoi(num);
			if(numFiles < 1){
				logger->Log(LOGGER_LEVEL_WARN, "Invalid IEF line at number");
				continue; // skip to the next line
			}

			regex = strtok(NULL,",");
			if(regex == NULL){
				logger->Log(LOGGER_LEVEL_WARN, "Incomplete IEF line at regex");
				continue; // skip to the next line
			}

			// get rid of the newline
			string rgx = trimNewline(string(regex));
			regex = (char *) rgx.c_str();

			// ---- Check if there is a regex
			if(strcmp(regex,"X") == 0)
				regex = (char *) "";

			// ---- Prepend the downlink directory path to the archive name
			string archive = string(IMMED_DIRECTORY) + "/" + string(arch);

			// ---- Call the function to gather and create a tarball from the files
			packageFiles((char *) archive.c_str(), dir, regex, numFiles); // TODO: error check this?

			cmdServer->wdmAsleep(); // set to asleep due to asynchronous call
			downlinkFile(archive);
			cmdServer->wdmAlive();
		}else{
			logger->Log(LOGGER_LEVEL_WARN, "IEF: unknown command type");
		}
	}

	fclose(fp);

	logger->Log(LOGGER_LEVEL_INFO, "Finished IEF");
}

void parseDRF(void) {
	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
	FILE * fp;
	char * line = NULL;
	size_t len = 0;
	ssize_t bytesRead;

	// check if a DRF file has been uplinked
	if(access(DRF_PATH, F_OK) != 0){
		logger->Log(LOGGER_LEVEL_DEBUG, "CMDStdTasks: no DRF");
		return;
	}

	// open the files to downlink file
	fp = fopen(DRF_PATH, "r");
	if (fp == NULL){
		logger->Log(LOGGER_LEVEL_ERROR, "CMDStdTasks: error opening DRF");
		remove(DRF_PATH);
		return;
	}

	// check password
	bytesRead = getline(&line, &len, fp);
	if(strcmp(line,UPLK_PASSWORD) != 0){
		logger->Log(LOGGER_LEVEL_ERROR, "CMDStdTasks: invalid DRF password");
		fclose(fp);
		remove(DRF_PATH);
		return;
	}

	// parse the file line by line
	char * token;
	char parseArray[200];
	while ((bytesRead = getline(&line, &len, fp)) != -1) {
		strcpy(parseArray, line);
		token = strtok(line,",");
		if (strcmp(token, "RAD") == 0) {
			prepRADDownlink(parseArray);
		} else {
			prepDataDownlink(parseArray);
		}
	}

	fclose(fp);

	logger->Log(LOGGER_LEVEL_INFO, "Finished DRF");
}

void prepRADDownlink(char * line) {
	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
	char * rad;
	char * passNumStr;
	char * startStr;
	char * endStr;
	int passNum, startChunk, endChunk;

	// ---- Parse the line
	rad = strtok(line, ",");
	if (rad == NULL || strcmp(rad, "RAD") != 0) {
		logger->Log(LOGGER_LEVEL_WARN, "DRF: error at RAD");
		return;
	}

	passNumStr = strtok(NULL, ",");
	if (passNumStr == NULL) {
		logger->Log(LOGGER_LEVEL_WARN, "DRF: incomplete at pass num");
		return;
	}

	startStr = strtok(NULL, ",");
	if (startStr == NULL) {
		logger->Log(LOGGER_LEVEL_WARN, "DRF: incomplete at start num");
		return;
	}

	endStr = strtok(NULL, ",");
	if (endStr == NULL) {
		logger->Log(LOGGER_LEVEL_WARN, "DRF: incomplete at end num");
		return;
	}

	// get rid of the newline
	string endc = trimNewline(string(endStr));
	endStr = (char *) endc.c_str();

	// convert to integers
	passNum = atoi(passNumStr);
	if(passNum < 1){
		logger->Log(LOGGER_LEVEL_WARN, "Invalid DRF line at pass num");
		return; // skip to the next line
	}

	startChunk = atoi(startStr);
	if(startChunk < 0){
		logger->Log(LOGGER_LEVEL_WARN, "Invalid DRF line at start chunk");
		return; // skip to the next line
	}

	endChunk = atoi(endStr);
	if(endChunk < 0){
		logger->Log(LOGGER_LEVEL_WARN, "Invalid DRF line at end chunk");
		return; // skip to the next line
	}

	// copy the appropriate files
	char command[200];
	for (int i = startChunk; i <= endChunk; i++) {
		sprintf(command, "cp " RAD_FILE_PATH "/RAD_%d.tar.gz.%03d " DOWNLINK_DIRECTORY "/", passNum, i);
		system(command);
	}
}

void prepDataDownlink(char * line) {
	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
	char * arch;
	char * dir;
	char * num;
	int numFiles;
	char * regex;

	// ---- Parse the line
	arch = strtok(line,",");
	if(arch == NULL){
		logger->Log(LOGGER_LEVEL_WARN, "Incomplete DRF line at archive");
		return; // skip to the next line
	}

	dir = strtok(NULL,",");
	if(dir == NULL){
		logger->Log(LOGGER_LEVEL_WARN, "Incomplete DRF line at directory");
		return; // skip to the next line
	}

	num = strtok(NULL,",");
	if(num == NULL){
		logger->Log(LOGGER_LEVEL_WARN, "Incomplete DRF line at number");
		return; // skip to the next line
	}
	numFiles = atoi(num);
	if(numFiles < 1){
		logger->Log(LOGGER_LEVEL_WARN, "Invalid DRF line at number");
		return; // skip to the next line
	}

	regex = strtok(NULL,",");
	if(regex == NULL){
		logger->Log(LOGGER_LEVEL_WARN, "Incomplete DRF line at regex");
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
	packageFiles((char *) archive.c_str(), dir, regex, numFiles); // TODO: error check this?

	// ---- Split the tarball into chunks
	CMDServer * cmdServer = dynamic_cast<CMDServer *> (Factory::GetInstance(CMD_SERVER_SINGLETON));
	char chunkSize[16];
	sprintf(chunkSize, "%lu", cmdServer->CMDConfiguration.fileChunkSize);
	string split_cmd = "split -b " + string(chunkSize) + " -d -a 3 " + archive + " " + archive + ".";
	if(system(split_cmd.c_str()) == -1){
		logger->Log(LOGGER_LEVEL_ERROR, "Failed to split DRF archive into chunks");
	}

	// ---- Delete the full archive
	string del_cmd = "rm " + archive;
	if(system(del_cmd.c_str()) == -1){
		logger->Log(LOGGER_LEVEL_ERROR, "Failed to delete DRF archive");
	}
}

void parseDLT(void) {
	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
	FILE * fp;
	char * line = NULL;
	size_t len = 0;
	ssize_t bytesRead;

	// check if a DLT file has been uplinked
	if(access(DLT_PATH, F_OK) != 0){
		logger->Log(LOGGER_LEVEL_DEBUG, "CMDStdTasks: no DLT");
		return;
	}

	// open the files to downlink file
	fp = fopen(DLT_PATH, "r");
	if (fp == NULL){
		logger->Log(LOGGER_LEVEL_ERROR, "CMDStdTasks: error opening DLT");
		remove(DLT_PATH);
		return;
	}

	// check password
	bytesRead = getline(&line, &len, fp);
	if(strcmp(line,UPLK_PASSWORD) != 0){
		logger->Log(LOGGER_LEVEL_ERROR, "CMDStdTasks: invalid DLT password");
		fclose(fp);
		remove(DLT_PATH);
		return;
	}

	// parse the file line by line
	char * dir;
	char * num;
	int numFiles;
	char * regex;
	while ((bytesRead = getline(&line, &len, fp)) != -1) {
		// ---- Parse line
		dir = strtok(line,",");
		if(dir == NULL){
			logger->Log(LOGGER_LEVEL_WARN, "Incomplete DLT line at dir");
			continue; // skip to the next line
		}

		num = strtok(NULL,",");
		if(num == NULL){
			logger->Log(LOGGER_LEVEL_WARN, "Incomplete DLT line at number");
			continue; // skip to the next line
		}
		numFiles = atoi(num);
		if(numFiles < -1 || numFiles == 0){
			logger->Log(LOGGER_LEVEL_WARN, "Invalid DLT line at number");
			continue;
		}

		regex = strtok(NULL,",");
		if(regex == NULL){
			logger->Log(LOGGER_LEVEL_WARN, "Incomplete DLT line at regex");
			continue; // skip to the next line
		}

		// get rid of the newline
		string rgx = trimNewline(string(regex));
		regex = (char *) rgx.c_str();

		// ---- find deletion scheme and delete
		if(strcmp(regex,"X") == 0 && numFiles != -1){
			deleteOldest(dir,numFiles);
		}else if(!(strcmp(regex,"X") == 0) && numFiles == -1){
			deleteRegex(dir,regex);
		}else{
			logger->Log(LOGGER_LEVEL_WARN, "DLT: Invalid deletion command");
		}
	}

	fclose(fp);

	logger->Log(LOGGER_LEVEL_INFO, "Finished DLT");
}

void parsePPE(void) {
	CMDServer * cmdServer = dynamic_cast<CMDServer *> (Factory::GetInstance(CMD_SERVER_SINGLETON));
	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
	FILE * fp;
	char * line = NULL;
	size_t len = 0;
	ssize_t bytesRead;

	// check if a PPE file has been uplinked
	if(access(PPE_PATH, F_OK) != 0){
		logger->Log(LOGGER_LEVEL_DEBUG, "CMDStdTasks: no PPE");
		return;
	}

	// open the files to downlink file
	fp = fopen(PPE_PATH, "r");
	if (fp == NULL){
		logger->Log(LOGGER_LEVEL_ERROR, "CMDStdTasks: error opening PPE");
		remove(PPE_PATH);
		return;
	}

	// check password
	bytesRead = getline(&line, &len, fp);
	if(strcmp(line,UPLK_PASSWORD) != 0){
		logger->Log(LOGGER_LEVEL_ERROR, "CMDStdTasks: invalid PPE password");
		fclose(fp);
		remove(PPE_PATH);
		return;
	}

	// parse the file line by line
	char * type;
	char * command;
	while ((bytesRead = getline(&line, &len, fp)) != -1) {
		// ---- Parse line
		type = strtok(line,",");
		if(type == NULL){
			logger->Log(LOGGER_LEVEL_WARN, "Incomplete PPE line at type");
			continue; // skip to the next line
		}

		// check the type of command and perform accordingly
		if(strcmp(type,"SYS") == 0){ // shell command
			command = strtok(NULL,",");
			if(command == NULL){
				logger->Log(LOGGER_LEVEL_WARN, "Incomplete PPE line at command");
				continue; // skip to the next line
			}

			// get rid of the newline
			string cmd = trimNewline(string(command));
			command = (char *) cmd.c_str();

			// execute the system command
			cmdServer->wdmAsleep(); // set to asleep due to asynchronous call
			if(system(command) == -1){
				logger->Log(LOGGER_LEVEL_ERROR, "PPE unable to execute shell command");
			}
			cmdServer->wdmAlive();
		}else if(strcmp(type,"FSW") == 0){ // fsw internal command
			command = strtok(NULL,",");
			if(command == NULL){
				logger->Log(LOGGER_LEVEL_WARN, "Incomplete PPE line at command");
				continue; // skip to the next line
			}

			int cmd = atoi(command);
			if(cmd == 0 || cmd < 0 || cmd > FSW_CMD_MAX){
				logger->Log(LOGGER_LEVEL_WARN, "Invalid PPE command number");
				continue; // skip to the next line
			}

			// call a function to find and execute the command
			executeFSWCommand(cmd);
		}else{
			logger->Log(LOGGER_LEVEL_WARN, "PPE: unknown command type");
		}
	}

	fclose(fp);

	logger->Log(LOGGER_LEVEL_INFO, "Finished PPE");
}

// A primary responsibility of this function is to deal with configuration updates.
// Before replacing old configurations, it checks the validity of the new configs.
void processUplinkFiles(void) {
	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));

	// Move new schedule from the uplink directory to its final directory
	if (access(SCH_UP, F_OK) != -1) {
		rename(SCH_UP, SCHEDULE_FILE);
	} else {
		logger->Log(LOGGER_LEVEL_DEBUG, "CMDStdTasks: no SCH");
	}

	// Update Config Schedule
	if (access(SCH_CFG_UP, F_OK) != -1) {
		SCHServer * schServer = dynamic_cast<SCHServer *> (Factory::GetInstance(SCH_SERVER_SINGLETON));
		if(schServer->LoadDefaultScheduleConfigurations((char *) SCH_CFG_UP)) {
			rename(SCH_CFG_UP, SCH_CONFIG);
		} else {
			remove(SCH_CFG_UP);
		}
	}

	// Update ACS config
	if (access(ACS_CFG_UP, F_OK) != -1) {
		remove(ACS_CFG_UP);
		//acsServer->updateConfig();
		logger->Log(LOGGER_LEVEL_ERROR, "CMDStdTasks: ACS config not implemented");
	}

	// Update CDH config
	if (access(CDH_CFG_UP, F_OK) != -1) {
		remove(CDH_CFG_UP);
		//cdhServer->updateConfig();
		logger->Log(LOGGER_LEVEL_ERROR, "CMDStdTasks: CDH config not implemented");
	}

	// Update CMD config
	if (access(CMD_CFG_UP, F_OK) != -1) {
		CMDServer * cmdServer = dynamic_cast<CMDServer *> (Factory::GetInstance(CMD_SERVER_SINGLETON));
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
		logger->Log(LOGGER_LEVEL_ERROR, "CMDStdTasks: COM config not implemented");
	}

	// Update EPS config
	if (access(EPS_CFG_UP, F_OK) != -1) {
		EPSServer * epsServer = dynamic_cast<EPSServer *> (Factory::GetInstance(EPS_SERVER_SINGLETON));
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
		logger->Log(LOGGER_LEVEL_ERROR, "CMDStdTasks: FMG config not implemented");
	}

	// Update GPS config
	if (access(GPS_CFG_UP, F_OK) != -1) {
		remove(GPS_CFG_UP);
		//gpsServer->updateConfig();
		logger->Log(LOGGER_LEVEL_ERROR, "CMDStdTasks: GPS config not implemented");
	}

	// Update PLD config
	if (access(PLD_CFG_UP, F_OK) != -1) {
		remove(PLD_CFG_UP);
		//pldServer->updateConfig();
		logger->Log(LOGGER_LEVEL_ERROR, "CMDStdTasks: PLD config not implemented");
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
    Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
	FILE * fd;
	int size = 0;

	// sh command to get the total size of all the files we want to compress
	char sh_cmd[248];
	sprintf(sh_cmd, "tot=0; for num in `ls -lr %s | grep ^- | grep \"%s\" | awk '{print $5}' | head -%d`; do tot=$(($tot+$num)); done; echo $tot", filePath, regex, maxFiles);

	// Execute an sh script and pipe the results back to the file descriptor fd
	if(!(fd = popen(sh_cmd, "r"))){
	    logger->Log(LOGGER_LEVEL_ERROR, "GetFileSize: could not verify file sizes");
	    return -1;
	}

	// Grab result of command
	int loop_count = 0;
	char buff[512];
	while(fgets(buff, sizeof(buff), fd)!=NULL){
		if (loop_count > 0) {
			logger->Log(LOGGER_LEVEL_ERROR, "GetFileSize: sh command produced too many results");
			return -2;
		}
	    size = atol(buff);
	    loop_count++;
	}

	if (pclose(fd) == -1){
		logger->Log(LOGGER_LEVEL_WARN, "GetFileSize: Error closing file stream");
	}

	return size;
}

const int packageFiles(const char * destination, const char * filePath, const char * regex, const int maxFiles) {
	// This function will tar up `maxFiles` number of files in `filePath` matching the regular expression `regex` into a tarball named `destination`.
    Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
	FILE * fd;

	const long size = getFileSize(filePath, regex, maxFiles);

	CMDServer * cmdServer = dynamic_cast<CMDServer *> (Factory::GetInstance(CMD_SERVER_SINGLETON));

	if (size < 0) {
		logger->Log(LOGGER_LEVEL_ERROR, "PackageFiles: Error detected, aborting packaging");
		return -1;
	}
	else if (size > cmdServer->CMDConfiguration.maxDownlinkSize){
		logger->Log(LOGGER_LEVEL_ERROR, "PackageFiles: Total file size is too great to package");
		return -2;
	}

	// sh command to tar the files to package
	char sh_cmd[256];
	sprintf(sh_cmd, "tar -czf %s -C %s `ls -lr %s | grep ^- | awk '{print $9}' | grep \"%s\" | head -%d`", destination, filePath, filePath, regex, maxFiles);

	// Execute a shell script and pipe the results back to the file descriptor fd
	if(!(fd = popen(sh_cmd, "r"))){
	    logger->Log(LOGGER_LEVEL_ERROR, "PackageFiles: Error packaging files");
	    return -3;
	}

	if (pclose(fd) == -1){
		logger->Log(LOGGER_LEVEL_WARN, "PackageFiles: Error closing file stream");
	}

	return 0;
}

int deleteOldest(char * filePath, int numFiles) {
	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
	FILE * fd;

	// sh command to remove the oldest numFiles from filePath
	char sh_cmd[256];
	sprintf(sh_cmd, "rm `ls -l %s | grep ^- | awk '{print \"%s\" \"/\" $9}' | head -%d`", filePath, filePath, numFiles);

	// Execute a shell script and pipe the results back to the file descriptor fd
	if(!(fd = popen(sh_cmd, "r"))){
		logger->Log(LOGGER_LEVEL_ERROR, "DeleteOldest: Error removing files");
		return -1;
	}

	if (pclose(fd) == -1){
		logger->Log(LOGGER_LEVEL_WARN, "DeleteOldest: Error closing file stream");
	}

	return 0;
}

int deleteRegex(char * filePath, char * regex) {
	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
	FILE * fd;

	// sh command to remove all files from filePath matching regex
	char sh_cmd[256];
	sprintf(sh_cmd, "rm `ls -l %s | grep ^- | awk '{print \"%s\" \"/\" $9}' | grep \"%s\"`", filePath, filePath, regex);

	// Execute a shell script and pipe the results back to the file descriptor fd
	if(!(fd = popen(sh_cmd, "r"))){
		logger->Log(LOGGER_LEVEL_ERROR, "DeleteRegex: Error removing files");
		return -1;
	}

	if (pclose(fd) == -1){
		logger->Log(LOGGER_LEVEL_WARN, "DeleteRegex: Error closing file stream");
	}

	return 0;
}

int getNumFiles(char * dir) {
	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
	FILE * fd;
	int num = 0;

	// sh command to determine the number of files in the downlink directory
	char sh_cmd[256];
	sprintf(sh_cmd, "ls -l %s | grep ^- | wc -l", dir);

	// Execute a shell script and pipe the results back to the file descriptor fd
	if(!(fd = popen(sh_cmd, "r"))){
		logger->Log(LOGGER_LEVEL_ERROR, "GetNumFilesDWN: Error checking file size");
		return -1;
	}

	// get the number from the result of the command
	int loop_count = 0;
	char buff[512];
	while(fgets(buff, sizeof(buff), fd)!=NULL){
		if (loop_count > 0) {
			logger->Log(LOGGER_LEVEL_ERROR, "GetFileSize: sh command produced too many results");
			return -2;
		}
		num = atoi(buff);
		loop_count++;
	}

	if (pclose(fd) == -1){
		logger->Log(LOGGER_LEVEL_WARN, "GetNumFilesDWN: Error closing file stream");
	}

	return num;
}

string getDownlinkFile(int fileNum) {
	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
	FILE * fd;
	char fileName[128];

	// sh command to get the filename for file number fileNum
	char sh_cmd[256];
	sprintf(sh_cmd, "ls -l %s | grep ^- | awk '{print \"%s\" \"/\" $9}' | head -%d | tail -1", DOWNLINK_DIRECTORY, DOWNLINK_DIRECTORY, fileNum);

	// Execute a shell script and pipe the results back to the file descriptor fd
	if(!(fd = popen(sh_cmd, "r"))){
		logger->Log(LOGGER_LEVEL_ERROR, "GetDownlinkFile: Error getting file name");
		return "";
	}

	// get the number from the result of the command
	int loop_count = 0;
	while(fgets(fileName, 128, fd)!=NULL){
		if (loop_count > 0) {
			logger->Log(LOGGER_LEVEL_ERROR, "GetDownlinkFile: sh command produced too many results");
			return "";
		}
		loop_count++;
	}

	if (pclose(fd) == -1){
		logger->Log(LOGGER_LEVEL_WARN, "GetNumFilesDWN: Error closing file stream");
	}

	string fn = trimNewline(string(fileName));

	return fn;
}

void executeFSWCommand(int command) {
	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
	COMServer * comServer = dynamic_cast<COMServer *> (Factory::GetInstance(COM_SERVER_SINGLETON));
	SCHServer * schServer = dynamic_cast<SCHServer *> (Factory::GetInstance(SCH_SERVER_SINGLETON));
	CDHServer * cdhServer = dynamic_cast<CDHServer *> (Factory::GetInstance(CDH_SERVER_SINGLETON));

	switch(command){
	case FSW_CMD_REQUEST_RESET:
		logger->Log(LOGGER_LEVEL_INFO, "PPE reset requested");
		schServer->RequestReset();
		break;
	case FSW_CMD_HARD_SATELLITE_RESET:
		logger->Log(LOGGER_LEVEL_ERROR, "PPE hard satellite reset, DOES NOT WORK");
		cdhServer->resetAssert(HARDWARE_LOCATION_EPS);
		break;
	case FSW_CMD_TX_SILENCE_START:
		logger->Log(LOGGER_LEVEL_WARN, "Commanding transmitter silence");
		comServer->setTxSilence(true);
		break;
	case FSW_CMD_TX_SILENCE_END:
		logger->Log(LOGGER_LEVEL_WARN, "Ending transmitter silence");
		comServer->setTxSilence(false);
		break;
	default:
		logger->Log(LOGGER_LEVEL_ERROR, "Unknown FSW command (bit flip probable)");
		break;
	}
}

int checkForSOT(void) {
	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
	if(access(SOT_PATH,F_OK) == 0){
		char * line = NULL;
		size_t len = 0;
		ssize_t read;

		// open the files to downlink file
		FILE * fp = fopen(SOT_PATH, "r");
		if (fp == NULL){
			logger->Log(LOGGER_LEVEL_ERROR, "CMDStdTasks: error opening SOT");
			remove(SOT_PATH);
			return 0;
		}

		// check the password
		read = getline(&line, &len, fp);
		if(strcmp(line, UPLK_PASSWORD_SKIP) == 0) {
			fclose(fp);
			return 1;
		} else if(strcmp(line, UPLK_PASSWORD_SEND) == 0) {
			fclose(fp);
			return 2;
		}

		logger->Log(LOGGER_LEVEL_ERROR, "CMDStdTasks: SOT bad password");
		fclose(fp);
		remove(SOT_PATH);
	}
	return false;
}

} // End of namespace Servers
} // End of namespace AllStar
