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
#include "util/Logger.h"
#include "util/TLM.h"
#include <math.h>
#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>

extern "C" {
	#include "util/propagator/AllStarOrbitProp.h"
	#include "util/propagator/gpsFrameRotation.h"
}

using namespace std;
using namespace AllStar::Core;
using namespace AllStar::HAL;

namespace AllStar {
namespace Servers {

const char * GPSServer::portname = (char *) "/dev/ttyS1";

GPSServer::GPSServer(string nameIn, LocationIDType idIn) :
		SubsystemServer(nameIn, idIn), Singleton() {
	GPSDataHolder = new GPS_BESTXYZ();
	GPSCoordsHolder = new GPS_GPRMC();

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

GPSServer::~GPSServer() { }

GPSServer & GPSServer::operator=(const GPSServer & source) {
	if (this == &source){
		return *this;
	}

	SubsystemServer::operator =(source);

	return *this;
}

bool GPSServer::IsFullyInitialized(void) {
	return(Singleton::IsFullyInitialized());
}

void GPSServer::SubsystemLoop(void) {
	Logger * logger = static_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
	CDHServer * cdhServer = static_cast<CDHServer *> (Factory::GetInstance(CDH_SERVER_SINGLETON));

	logger->Log(LOGGER_LEVEL_INFO, "GPSServer: entered subsystem loop");
	TLM_GPS_SERVER_STARTED();

	cdhServer->subPowerOn(HARDWARE_LOCATION_GPS);

	GPSReadType readStatus;
	int64 currTime = 0;
	int64 lastWakeTime = 0;
	int64 lastUpdate = 0;
	int64 lastData = getTimeInMillis(); // data from the GPS (valid or not)
	char buffer[350];
	int fd;

	fd = CreatePort();

	logger->Log(LOGGER_LEVEL_INFO, "GPSServer: created port");

	while (1) {
		wdmAlive();
		lastWakeTime = getTimeInMillis();

		// if the GPS has been powered off due to a fault, restart it.
		if (!cdhServer->subsystemPowerStates[HARDWARE_LOCATION_GPS]) {
			cdhServer->subPowerOn(HARDWARE_LOCATION_GPS);
			wdmAsleep();
			usleep(5000000); // give it time to restart
			wdmAlive();
			lastData = getTimeInMillis();
		}

		readStatus = ReadData(buffer, fd);

		currTime = getTimeInMillis();
		switch (readStatus) {
		case GPS_NO_DATA:
			if (currTime > lastData + 10000) { // if it's been 10 seconds since the last GPS data, reboot
				cdhServer->subPowerOff(HARDWARE_LOCATION_GPS);
				wdmAsleep();
				usleep(2000000);
				wdmAlive();
			}
			break;
		case GPS_NO_LOCK:
			lastData = currTime;
			if (currTime > lastUpdate + 2000) { // update with the propagator if it's been 2 seconds
				lastUpdate = currTime;
				// TODO: update and propagate
			}
			break;
		case GPS_LOCK:
			lastData = currTime;
			lastUpdate = currTime;
			// TODO: update
			break;
		default:
			logger->Log(LOGGER_LEVEL_WARN, "GPS invalid read status (bit flip)");
			break;
		}

		waitUntil(lastWakeTime, 100);
	}
}

// TODO: error handling
int GPSServer::CreatePort(void) {
	Logger * logger = static_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));


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

GPSReadType GPSServer::ReadData(char * buffer, int fd) {
	Logger * logger = static_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));

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
			return GPS_NO_LOCK;
		} else {
			c1 = c;

			// read while there's more data, and ensure we don't overflow the buffer
			while(read(fd,&c,1) == 1){
				buffer[counter++] = c;

				// if BESTXYZ, read crc and break
				if (c == '*' && c1 == '#') {
					if (counter > 339) {
						logger->Log(LOGGER_LEVEL_WARN, "GPSServer: BESTXYZ too long!");
						return GPS_NO_LOCK;
					}

					for (uint8 i = 0; i < 10; i++) {
						readSuccess &= (read(fd,&c,1) == 1);
						buffer[counter++] = c;
					}

					if (readSuccess) {
						break;
					} else {
						logger->Log(LOGGER_LEVEL_WARN, "GPSServer: error reading CRC from serial");
						return GPS_NO_LOCK;
					}
				}

				// if GPRMC, read checksum and break
				if (c == '*' && c1 == '$') {
					if (counter > 345) {
						logger->Log(LOGGER_LEVEL_WARN, "GPSServer: GPRMC too long!");
						return GPS_NO_LOCK;
					}
					for (uint8 i = 0; i < 4; i++) {
						readSuccess &= (read(fd,&c,1) == 1);
						buffer[counter++] = c;
					}
					if (readSuccess) {
						break;
					} else {
						logger->Log(LOGGER_LEVEL_WARN, "GPSServer: error reading checksum from serial");
						return GPS_NO_LOCK;
					}
				}

				// Ensure that the buffer doesn't overflow
				if(counter == 350){
					logger->Log(LOGGER_LEVEL_WARN, "GPSServer: Data too long!");
					return GPS_NO_LOCK;
				}
			}

			logger->Log(LOGGER_LEVEL_DEBUG, "GPSServer: Read data from GPS, now processing...");

			if (c1 == '#') {
				if (!BESTXYZProcess(buffer, counter)) {
					logger->Log(LOGGER_LEVEL_DEBUG, "GPSServer: Bad BESTXYZ");
					return GPS_NO_LOCK;
				}
			} else if (c1 == '$') {
				if (!GPRMCProcess(buffer, counter)) {
					logger->Log(LOGGER_LEVEL_DEBUG, "GPSServer: Bad GPRMC");
					return GPS_NO_LOCK;
				}
			} else {
				logger->Log(LOGGER_LEVEL_WARN, "GPSServer: error with first character!");
				return GPS_NO_LOCK;
			}
			return GPS_LOCK;
		}
	} else {
		return GPS_NO_DATA;
	}
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
