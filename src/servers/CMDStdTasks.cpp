/*
 * CMDStdTasks.cpp
 *
 *  Created on: May 21, 2013
 *      Author: Brian Campuzano
 */

#include <list>

#include "servers/SCHServer.h"
#include "servers/CMDStdTasks.h"
#include "servers/DispatchStdTasks.h"

#include "core/StdTypes.h"
#include "core/Singleton.h"
#include "core/Factory.h"
#include "core/WatchdogManager.h"

#include "util/FileHandler.h"
#include "util/Logger.h"

#include <cstdio>
#include <iostream>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>

//#include "boards/backplane/dbg_led.h"

using namespace AllStar::Core;
using namespace std;

namespace AllStar{
namespace Servers{

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

} // End of namespace Servers
} // End of namespace AllStar
