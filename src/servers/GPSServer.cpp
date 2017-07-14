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
	#include "util/propagator/OrbitalMotionAllStar.h"
}

using namespace std;
using namespace AllStar::Core;
using namespace AllStar::HAL;

namespace AllStar {
namespace Servers {

const char * GPSServer::portname = (char *) "/dev/ttyS1";
GPSLockType GPSServer::lastLock = {{0.0, 0.0, 0.0, 0.0, 0.0, 0.0}, -1.0};
GPSInertial GPSServer::GPSInertialCoords(0.0, 0.0, 0.0, 0.0, 0.0, 0.0, -1);
GPSPositionTime * GPSServer::GPSDataHolder = new GPSPositionTime(103, 1500.0, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 2);
GPSCoordinates * GPSServer::GPSCoordsHolder = new GPSCoordinates(1000.0, 1000.1);

GPSServer::GPSServer(string nameIn, LocationIDType idIn) :
		SubsystemServer(nameIn, idIn), Singleton(), propagating(false), noOE(true) { }

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

	logger->Info("GPSServer: entered subsystem loop");
	TLM_GPS_SERVER_STARTED();

	cdhServer->subPowerOn(HARDWARE_LOCATION_GPS);

	GPSReadType readStatus;
	int64 readTime = 0;
	int64 lastWakeTime = 0;
	int64 lastUpdate = 0;
	int64 lastData = getTimeInMillis(); // data from the GPS (valid or not)
	char buffer[350];
	int fd;

	fd = CreatePort();

	logger->Info("GPSServer: created port");

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

		readTime = getTimeInMillis();
		switch (readStatus) {
		case GPS_NO_DATA:
			if (readTime > lastData + 30000) { // if it's been 30 seconds since the last GPS data, reboot
				cdhServer->subPowerOff(HARDWARE_LOCATION_GPS);
				wdmAsleep();
				usleep(2000000);
				wdmAlive();
			}
			break;
		case GPS_NO_LOCK:
			lastData = readTime;

			if (noOE) {
				break; // we can't propagate yet
			}

			if (!propagating) {
				propagating = true;
				ECItoOE();
			}

			if (readTime > lastUpdate + 2000) { // update with the propagator if it's been 2 seconds
				lastUpdate = readTime;
				UpdateAndPropagate();
			}
			break;
		case GPS_LOCK:
			noOE = false;
			if (propagating) {
				propagating = false;
			}

			lastData = readTime;
			lastUpdate = readTime;
			GPSInertialCoords.sysTime = readTime;
			ECEFtoECI();
			break;
		default:
			logger->Warning("GPS invalid read status (bit flip)");
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
		logger->Fatal("GPSServer: Failed to open serial port!");
	}

	if (tcgetattr(portfd, &port) < 0) {
		logger->Error("GPSServer: Problem with initial port attributes.");
	}

	port.c_lflag &= ~ECHO; // turn echo off

	if (cfsetispeed(&port, B115200) < 0 || cfsetospeed(&port, B115200) < 0) {
		logger->Fatal("GPSServer: Problem setting the baud rate!");
	}

	if (tcsetattr(portfd, TCSANOW, &port) < 0) {
		logger->Error("GPSServer: Problem setting port attributes!");
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

	logger->SuperDebug("GPSServer: preparing to read data");

	// Check that there's data
	if (read(fd,&c,1) == 1) {
		// Check the first character
		if (c != 35 && c != 36) { // '#' or '$'
			logger->SuperDebug("GPSServer: Data doesn't start with '#' or '$'");
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
						logger->Warning("GPSServer: BESTXYZ too long!");
						return GPS_NO_LOCK;
					}

					for (uint8 i = 0; i < 10; i++) {
						readSuccess &= (read(fd,&c,1) == 1);
						buffer[counter++] = c;
					}

					if (readSuccess) {
						break;
					} else {
						logger->Warning("GPSServer: error reading CRC from serial");
						return GPS_NO_LOCK;
					}
				}

				// if GPRMC, read checksum and break
				if (c == '*' && c1 == '$') {
					if (counter > 345) {
						logger->Warning("GPSServer: GPRMC too long!");
						return GPS_NO_LOCK;
					}
					for (uint8 i = 0; i < 4; i++) {
						readSuccess &= (read(fd,&c,1) == 1);
						buffer[counter++] = c;
					}
					if (readSuccess) {
						break;
					} else {
						logger->Warning("GPSServer: error reading checksum from serial");
						return GPS_NO_LOCK;
					}
				}

				// Ensure that the buffer doesn't overflow
				if(counter == 350){
					logger->Warning("GPSServer: Data too long!");
					return GPS_NO_LOCK;
				}
			}

			logger->Debug("GPSServer: Read data from GPS, now processing...");

			if (c1 == '#') {
				if (!BESTXYZProcess(buffer, counter)) {
					logger->Debug("GPSServer: Bad BESTXYZ");
					return GPS_NO_LOCK;
				}
			} else if (c1 == '$') {
				if (!GPRMCProcess(buffer, counter)) {
					logger->Debug("GPSServer: Bad GPRMC");
					return GPS_NO_LOCK;
				}
			} else {
				logger->Warning("GPSServer: error with first character!");
				return GPS_NO_LOCK;
			}
			return GPS_LOCK;
		}
	} else {
		return GPS_NO_DATA;
	}
}

GPSPositionTime * GPSServer::GetGPSDataPtr(void) {
	if (true == this->TakeLock(MAX_BLOCK_TIME)) {
		this->GiveLock();
		return GPSDataHolder;
	}
	return NULL;
}

GPSCoordinates * GPSServer::GetGPSCoordsPtr(void) {
	if (true == this->TakeLock(MAX_BLOCK_TIME)) {
		this->GiveLock();
		return GPSCoordsHolder;
	}
	return NULL;
}

void GPSServer::UpdateAndPropagate() {
	float eciPos[3];
	float eciVel[3];
	int64 currTime = getTimeInMillis();
	float propTime = currTime/1000.0 - lastLock.sysTime;

	propagatePositionVelocity(lastLock.elements, propTime, eciPos, eciVel);

	GPSInertialCoords.posX = eciPos[0];
	GPSInertialCoords.posY = eciPos[1];
	GPSInertialCoords.posZ = eciPos[2];
	GPSInertialCoords.velX = eciVel[0];
	GPSInertialCoords.velY = eciVel[1];
	GPSInertialCoords.velZ = eciVel[2];
	GPSInertialCoords.sysTime = currTime;
}

void GPSServer::ECItoOE() {
	float r[4];
	float v[4];
	r[1] = GPSInertialCoords.posX;
	r[2] = GPSInertialCoords.posY;
	r[3] = GPSInertialCoords.posZ;
	v[1] = GPSInertialCoords.velX;
	v[2] = GPSInertialCoords.velY;
	v[3] = GPSInertialCoords.velZ;

	lastLock.sysTime = GPSInertialCoords.sysTime / 1000.0;

	rv2elem(MU_EARTH, r, v, &(lastLock.elements));// Get the orbital elements using the last position and velocity
}

void GPSServer::ECEFtoECI() {
	// arrays for the rotation
	float posECEF[3];
	float velECEF[3];
	float posECI[3];
	float velECI[3];
	float gpsTime[2];

	posECEF[0] = GPSDataHolder->posX;
	posECEF[1] = GPSDataHolder->posY;
	posECEF[2] = GPSDataHolder->posZ;
	velECEF[0] = GPSDataHolder->velX;
	velECEF[1] = GPSDataHolder->velY;
	velECEF[2] = GPSDataHolder->velZ;
	gpsTime[0] = GPSDataHolder->GPSWeek;
	gpsTime[1] = GPSDataHolder->GPSSec;

	wgs2gcrf(posECEF, velECEF, gpsTime, posECI, velECI);

	GPSInertialCoords.posX = posECI[0];
	GPSInertialCoords.posY = posECI[1];
	GPSInertialCoords.posZ = posECI[2];
	GPSInertialCoords.velX = velECI[0];
	GPSInertialCoords.velY = velECI[1];
	GPSInertialCoords.velZ = velECI[2];
}

}
}
