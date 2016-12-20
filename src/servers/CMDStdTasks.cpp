/*
 * CMDStdTasks.cpp
 *
 *  Created on: May 21, 2013
 *      Author: Brian Campuzano
 */

#include "servers/CMDServer.h"
#include "servers/SCHServer.h"
#include "servers/CMDStdTasks.h"
#include "servers/DispatchStdTasks.h"
#include "core/StdTypes.h"
#include "core/Singleton.h"
#include "core/Factory.h"
#include "core/WatchdogManager.h"
#include "util/FileHandler.h"
#include "util/Logger.h"
#include "util/Diagnostics.h"
#include <termios.h>
#include <string>

using namespace AllStar::Core;
using namespace std;

namespace AllStar{
namespace Servers{

#define NUM_DIAGNOSTIC_TESTS 4

void portSetup(void){
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
}

void uftpSetup(void){
	char cmdString[100];

	//---------------------------SLIP------------------------------------
	sprintf(cmdString, "slattach -L -m -p slip \"/dev/ttyS2\" &");
	printf("%s\n", cmdString);
	system(cmdString);

	sprintf(cmdString, "ifconfig sl0 1.1.1.1 pointopoint 1.1.1.2 up");
	printf("%s\n", cmdString);
	system(cmdString);

	//---------------------------UFTP-----------------------------------
	//TODO add encryption
	char tmpDir[20] = "~/tmp";
	char fileDir[20] = "~/dow";
	//char prvKey[20] = "~/rsakey.pem";
	//sprintf(cmdString, "./uftpd -T %s -I sl0 -D %s -k %s -E", tmpDir, fileDir, prvKey);
	sprintf(cmdString, "./uftpd -T %s -I sl0 -D %s", tmpDir, fileDir);
	printf("%s\n", cmdString);
	system(cmdString);
}

void runDiagnostic(void){
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

void parseDRF(void){
	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
	FILE * fp;
	char * line = NULL;
	size_t len = 0;
	ssize_t read;

	// open the files to downlink file
	fp = fopen(DRF_PATH, "r");
	if (fp == NULL){
		logger->Log(LOGGER_LEVEL_ERROR, "CMDStdTasks: error opening DRF");
		return;
	}

	// parse the file line by line
	char * archive;
	char * dir;
	char * num;
	int numFiles;
	char * regex;
	while ((read = getline(&line, &len, fp)) != -1) {
		// ---- Parse line
		archive = strtok(line,",");
		if(archive == NULL){
			logger->Log(LOGGER_LEVEL_WARN, "Incomplete DRF line at archive");
			continue; // skip to the next line
		}

		dir = strtok(NULL,",");
		if(dir == NULL){
			logger->Log(LOGGER_LEVEL_WARN, "Incomplete DRF line at directory");
			continue; // skip to the next line
		}

		num = strtok(NULL,",");
		if(num == NULL){
			logger->Log(LOGGER_LEVEL_WARN, "Incomplete DRF line at number");
			continue; // skip to the next line
		}
		numFiles = atoi(num);
		if(numFiles < 1){
			logger->Log(LOGGER_LEVEL_WARN, "Invalid DRF line at number");
			continue; // skip to the next line
		}

		regex = strtok(NULL,",");
		if(regex == NULL){
			logger->Log(LOGGER_LEVEL_WARN, "Incomplete DRF line at regex");
			continue; // skip to the next line
		}

		// ---- Call the function to gather and compress
		string lsResults[numFiles];
		if(strcmp(regex,"X\n") == 0 || strcmp(regex,"X") == 0){
			logger->Log(LOGGER_LEVEL_DEBUG, "No regex");
			// gather and compress
		}else{
			logger->Log(LOGGER_LEVEL_DEBUG, "regex");
			// gather and compress
		}
	}

	fclose(fp);

	logger->Log(LOGGER_LEVEL_INFO, "Finished DRF");
}

void parseDLT(void){
	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
	FILE * fp;
	char * line = NULL;
	size_t len = 0;
	ssize_t read;

	// open the files to downlink file
	fp = fopen(DLT_PATH, "r");
	if (fp == NULL){
		logger->Log(LOGGER_LEVEL_ERROR, "CMDStdTasks: error opening DLT\n");
		return;
	}

	// parse the file line by line
	char * dir;
	char * num;
	int numFiles;
	char * regex;
	while ((read = getline(&line, &len, fp)) != -1) {
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

		// ---- find deletion scheme and delete
		if((strcmp(regex,"X\n") == 0 || strcmp(regex,"X") == 0) && numFiles != -1){
			logger->Log(LOGGER_LEVEL_WARN, "---- No regex");
		}else if(!(strcmp(regex,"X\n") == 0 || strcmp(regex,"X") == 0) && numFiles == -1){
			logger->Log(LOGGER_LEVEL_WARN, "---- Regex");
		}else{
			logger->Log(LOGGER_LEVEL_WARN, "---- Invalid combination");
		}
	}

	fclose(fp);

	logger->Log(LOGGER_LEVEL_INFO, "Finished DLT");
}

void postPassExecution(void){
	// parse and execute post-pass execution script
}

void processUplinkFiles(void){
	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));

	// Move new schedule from the uplink directory to its final directory
	if(access(SCHEDULE_PATH, F_OK) != -1){
		rename(SCHEDULE_PATH, SCH_SCHEDULE_FILE);
	}else{
		logger->Log(LOGGER_LEVEL_WARN, "ProcessUplinkFiles(): No new schedule file!");
	}
}

} // End of namespace Servers
} // End of namespace AllStar
