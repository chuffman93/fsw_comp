/*
 * CMDStdTasks.cpp
 *
 *  Created on: May 21, 2013
 *      Author: Brian Campuzano
 */

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
#include <cstdio>
#include <iostream>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <list>

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
		BusLoadTest(false); // fixme: note, this will cause CDH to reboot for some reason
		SPIStats(isActive[1]);
		SampleTest2(isActive[2]);
		SampleTest3(isActive[3]);

		fclose(fp);

		// FIXME: decide on deletion/transfer scheme
		//remove("diagnostics.txt");
	}
}

} // End of namespace Servers
} // End of namespace AllStar
