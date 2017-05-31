/*
 * GPSServer.cpp
 *
 *    Created: 7/6/12
 * Written by: Conrad Hougen
 *
 *    Updated: 7/7/16
 *     Author: Alex St. Clair
 *
 */

#include "servers/GPSServer.h"
#include "servers/GPSStdTasks.h"
#include "servers/DispatchStdTasks.h"
#include "servers/CDHServer.h"
#include "core/Singleton.h"
#include "core/Factory.h"
#include "core/ModeManager.h"
#include "core/StdTypes.h"
#include "util/FileHandler.h"
#include "util/Logger.h"
#include <math.h>
#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>

using namespace std;
using namespace AllStar::Core;
using namespace AllStar::HAL;

namespace AllStar{
namespace Servers{

const char * GPSServer::portname = (char *) "/dev/ttyS1";

GPSServer::GPSServer(string nameIn, LocationIDType idIn) :
		SubsystemServer(nameIn, idIn), Singleton(), gpsResponsive(false) {
	GPSDataHolder = new GPS_BESTXYZ();
	GPSCoordsHolder = new GPS_GPRMC();

	GPSDataHolder->round_seconds = 30;
	GPSDataHolder->GPSSec = 7.0;
	GPSDataHolder->GPSWeek = 8;
	GPSDataHolder->posX = 1.0;
	GPSDataHolder->posY = 2.0;
	GPSDataHolder->posZ = 3.0;
	GPSDataHolder->velX = 4.0;
	GPSDataHolder->velY = 5.0;
	GPSDataHolder->velZ = 6.0;

	GPSCoordsHolder->latitude = 1000.0;
	GPSCoordsHolder->longitude = 1000.0;
}

GPSServer::~GPSServer(){
}

GPSServer & GPSServer::operator=(const GPSServer & source) {
	if (this == &source){
		return *this;
	}

	SubsystemServer::operator =(source);

	return *this;
}

void GPSServer::Initialize(void) {
}

#ifdef TEST
void GPSServer::Destroy(void) {
}
#endif

bool GPSServer::IsFullyInitialized(void) {
	return(Singleton::IsFullyInitialized());
}

void GPSServer::SubsystemLoop(void) {
	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
	CDHServer * cdhServer = dynamic_cast<CDHServer *> (Factory::GetInstance(CDH_SERVER_SINGLETON));

	logger->Log(LOGGER_LEVEL_INFO, "GPSServer: entered subsystem loop");
	gpsResponsive = false;

	cdhServer->subPowerOn(HARDWARE_LOCATION_GPS);

	bool check;
	uint32 downCount = 0;
	char buffer[350];
	int fd;

	fd = CreatePort();

	logger->Log(LOGGER_LEVEL_INFO, "GPSServer: created port");

	while (1) {
		wdmAlive();
		int64 LastWakeTime = getTimeInMillis();

		// if the GPS has been powered off due to a fault, restart it.
		if (!cdhServer->subsystemPowerStates[HARDWARE_LOCATION_GPS]) {
			cdhServer->subPowerOn(HARDWARE_LOCATION_GPS);
			usleep(2000000); // give it time to restart
		}

		check = ReadData(buffer, fd);

		// if we haven't been reading data, set a flag
		if (check) {
			downCount = 0;
		} else {
			downCount++;
			if(downCount > 5000){ // TODO: formalize this value (currently ~10 min)
				logger->Log(LOGGER_LEVEL_ERROR, "GPSServer: no data, resetting GPS!");
				cdhServer->subPowerOff(HARDWARE_LOCATION_GPS);
				usleep(1000000);
				cdhServer->subPowerOn(HARDWARE_LOCATION_GPS);
				usleep(2000000);
				downCount = 0;
			}
		}
		waitUntil(LastWakeTime, 100);
	}
}

// TODO: error handling
int GPSServer::CreatePort(void) {
	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));


	int portfd = open(portname, O_RDWR | O_NOCTTY | O_NDELAY);
	if (portfd == -1) {
		logger->Log(LOGGER_LEVEL_FATAL, "GPSServer: Failed to open serial port!");
	}

	if (tcgetattr(portfd, &port) < 0) {
		logger->Log(LOGGER_LEVEL_ERROR, "GPSServer: Problem with initial port attributes.");
	}

	port.c_lflag &= ~ECHO; // turn echo off

	if (cfsetispeed(&port, B115200) < 0 || cfsetospeed(&port, B115200) < 0) {
		logger->Log(LOGGER_LEVEL_FATAL, "GPSServer: Problem setting the baud rate!");
	}

	if (tcsetattr(portfd, TCSANOW, &port) < 0) {
		logger->Log(LOGGER_LEVEL_ERROR, "GPSServer: Problem setting port attributes!");
	}

	return portfd;
}

double GPSServer::DistanceTo(double latitude1, double longitude1) {
	double R = 6371000.0; //Radius of earth in meters
	double latitude2 = this->GPSCoordsHolder->latitude;
	double longitude2 = this->GPSCoordsHolder->longitude;

	//to radians
	latitude1 = latitude1*M_PI/180.0;
	longitude1 = longitude1*M_PI/180.0;
	latitude2 = latitude2*M_PI/180.0;
	longitude2 = longitude2*M_PI/180.0;

	double dlat = latitude2 - latitude1;
	double dlong = longitude2 - longitude1;

	double a = sin(dlat/2) * sin(dlat/2) + cos(latitude1)*cos(latitude2)*sin(dlong/2)*sin(dlong/2);
	double c = 2*atan2(sqrt(a), sqrt(1-a));
	return c*R;
}

int GPSServer::GetWeek(void) {
	return GPSDataHolder->GPSWeek;
}

float GPSServer::GetSeconds(void) {
	return GPSDataHolder->GPSSec;
}

uint32 GPSServer::GetRoundSeconds(void) {
	return GPSDataHolder->round_seconds;
}

bool GPSServer::ReadData(char * buffer, int fd) {
	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));

	char c, c1;
	uint16 counter = 0;
	bool readSuccess = true;

	logger->Log(LOGGER_LEVEL_SUPER_DEBUG, "GPSServer: preparing to read data");

	// Check that there's data
	if (read(fd,&c,1) == 1) {
		// Check the first character
		if (c != 35 && c != 36) { // '#' or '$'
			logger->Log(LOGGER_LEVEL_SUPER_DEBUG, "GPSServer: Data doesn't start with '#' or '$'");
			while(read(fd,&c,1) == 1); // if wrong char, clear the buffer (ensures that we stay up-to-date and aligned with the data stream)
		} else {
			c1 = c;

			// read while there's more data, and ensure we don't overflow the buffer
			while(read(fd,&c,1) == 1){
				buffer[counter++] = c;

				// if BESTXYZ, read crc and return
				if (c == '*' && c1 == '#') {
					if (counter > 339) {
						logger->Log(LOGGER_LEVEL_WARN, "GPSServer: BESTXYZ too long!");
						return false;
					}
					for (uint8 i = 0; i < 10; i++) {
						readSuccess &= (read(fd,&c,1) == 1);
						buffer[counter++] = c;
					}
					if (readSuccess) {
						break;
					} else {
						logger->Log(LOGGER_LEVEL_WARN, "GPSServer: error reading CRC from serial");
						return false;
					}
				}

				// if GPRMC, read checksum and return
				if (c == '*' && c1 == '$') {
					if (counter > 345) {
						logger->Log(LOGGER_LEVEL_WARN, "GPSServer: GPRMC too long!");
						return false;
					}
					for (uint8 i = 0; i < 4; i++) {
						readSuccess &= (read(fd,&c,1) == 1);
						buffer[counter++] = c;
					}
					if (readSuccess) {
						break;
					} else {
						logger->Log(LOGGER_LEVEL_WARN, "GPSServer: error reading checksum from serial");
						return false;
					}
				}

				// Ensure that the buffer doesn't overflow
				if(counter == 350){
					logger->Log(LOGGER_LEVEL_WARN, "GPSServer: Data too long!");
					return false;
				}
			}

			logger->Log(LOGGER_LEVEL_DEBUG, "GPSServer: Read data from GPS, now processing...");

			if (c1 == '#') {
				if (!BESTXYZProcess(buffer, counter)) {
					logger->Log(LOGGER_LEVEL_DEBUG, "GPSServer: Bad BESTXYZ");
					return false;
				}
			} else if (c1 == '$') {
				if (!GPRMCProcess(buffer, counter)) {
					logger->Log(LOGGER_LEVEL_DEBUG, "GPSServer: Bad GPRMC");
					return false;
				}
			} else {
				logger->Log(LOGGER_LEVEL_WARN, "GPSServer: error with first character!");
				return false;
			}
			return true;
		}
	} else {
		return false;
	}
	return false;
}

GPS_BESTXYZ * GPSServer::GetGPSDataPtr(void) {
	if (true == this->TakeLock(MAX_BLOCK_TIME)) {
		this->GiveLock();
		return GPSDataHolder;
	}
	return NULL;
}

GPS_GPRMC * GPSServer::GetGPSCoordsPtr(void) {
	if (true == this->TakeLock(MAX_BLOCK_TIME)) {
		this->GiveLock();
		return GPSCoordsHolder;
	}
	return NULL;
}

}
}
