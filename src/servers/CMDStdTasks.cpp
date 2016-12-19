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
//#include <regex>
//#include <experimental/filesystem>

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
		logger->Log("CMDServer: Failed to open serial port!", LOGGER_LEVEL_FATAL);
	}

	if(tcgetattr(portfd, &port) < 0) {
		logger->Log("CMDServer: Problem with initial port attributes.", LOGGER_LEVEL_ERROR);
	}

	port.c_iflag &= ~IXON;

	if(cfsetispeed(&port, B115200) < 0 || cfsetospeed(&port, B115200) < 0){
		logger->Log("GPSServer: Problem setting the baud rate!", LOGGER_LEVEL_FATAL);
	}

	if(tcsetattr(portfd, TCSANOW, &port) < 0){
		logger->Log("GPSServer: Problem setting port attributes!", LOGGER_LEVEL_ERROR);
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
	logger->Log("CMDServer: running diagnostics tests", LOGGER_LEVEL_INFO);
	FILE * fp = fopen("diagnostics.txt", "r");

	if(fp == NULL){
		logger->Log("Error opening diagnostic tests file", LOGGER_LEVEL_WARN);

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
				logger->Log("Diagnostic tests file too short!", LOGGER_LEVEL_WARN);
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

void updateDownlinkQueue(void){
	// Read in the files to downlink, and update the downlink priority queue
	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
	FILE * fp;
	char * line = NULL;
	size_t len = 0;
	ssize_t read;

	// open the files to downlink file
	fp = fopen(FILES_TO_DOWNLINK_UPLK, "r");
	if (fp == NULL){
		logger->Log("CMDStdTasks: error opening files to downlink file", LOGGER_LEVEL_ERROR);
		return;
	}

	// parse the file line by line
	char * archive;
	char * path;
	char * prio;
	int priority;
	downlinkFile qFile;
	CMDServer * cmdServer = dynamic_cast<CMDServer *> (Factory::GetInstance(CMD_SERVER_SINGLETON));
	while ((read = getline(&line, &len, fp)) != -1) {
		if(strcmp(line, "CLEAR\n") == 0){
			clearDownlinkQueue();
			logger->Log("Clearing downlink queue", LOGGER_LEVEL_WARN);
		}else{
			// parse and compress the file to downlink
			archive = strtok(line,",");
			if(archive != NULL){
				path = strtok(NULL,",");
				if(path != NULL){
					prio = strtok(NULL,",");
					if(prio != NULL){
						priority = atoi(prio);

						string cmd = "tar -czvf " + string(DOWNLINK_DIRECTORY) + string(archive) + " " + string(path);
						system(cmd.c_str());

						qFile.fileName = archive;
						qFile.pathName = path;
						qFile.priority = priority;
						cmdServer->downlinkPriorityQueue.push(qFile);
					}
				}
			}
		}
	}

	fclose(fp);
}

void clearDownlinkQueue(void){
	CMDServer * cmdServer = dynamic_cast<CMDServer *> (Factory::GetInstance(CMD_SERVER_SINGLETON));

	while(!cmdServer->downlinkPriorityQueue.empty()){
		string cmd = "rm " + string(DOWNLINK_DIRECTORY) + string(cmdServer->downlinkPriorityQueue.top().fileName);
		system(cmd.c_str());
		cmdServer->downlinkPriorityQueue.pop();
	}
}

void postPassExecution(void){
	// parse and execute post-pass execution script
}

void processUplinkFiles(void){
	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));

	// Move new schedule from the uplink directory to its final directory
	if(access(SCHEDULE_UPLK, F_OK) != -1){
		rename(SCHEDULE_UPLK, SCH_SCHEDULE_FILE);
	}else{
		logger->Log("ProcessUplinkFiles(): No new schedule file!", LOGGER_LEVEL_WARN);
	}
}

} // End of namespace Servers
} // End of namespace AllStar
