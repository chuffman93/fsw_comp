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
#include "servers/FileSystem.h"
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
GPSInertial * GPSServer::GPSInertialCoords = new GPSInertial(0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0, 0, 0);
GPSPositionTime * GPSServer::GPSDataHolder = new GPSPositionTime(0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0, 0);

GPSServer::GPSServer(string nameIn, LocationIDType idIn) :
		SubsystemServer(nameIn, idIn), Singleton(), propagating(true), noOE(true), timeKnown(false),
		numTracked(0), inertialTime(0), GPSFileDescriptor(0) { }

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
	wdmAsleep();
	sleep(7);
	wdmAlive();

	// if the system clock time is high enough, assume we updated the time on reboot
	if (getTimeInSec() > 1500000000) {
		timeKnown = false;
	} else {
		timeKnown = true;
	}

	bootConfig();
	bootLastLock();

	GPSReadType readStatus;
	int64 readTime = 0;
	int64 lastWakeTime = 0;
	int64 lastUpdate = 0;
	int64 lastLock = getTimeInMillis();
	int64 lastData = getTimeInMillis(); // data from the GPS (valid or not)
	char buffer[350];

	GPSFileDescriptor = CreatePort();
	ConfigureGPS();

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

		readStatus = ReadData(buffer, GPSFileDescriptor);

		readTime = getTimeInMillis();
		switch (readStatus) {
		case GPS_NO_DATA:
			if (readTime > lastData + 30000) { // if it's been 30 seconds since the last GPS data (valid or not), reboot
				logger->Warning("GPSServer: no data, rebooting");
				RestartGPS();
				lastData = readTime;
				lastLock = readTime;
			}

			if (readTime > lastUpdate + 2000) { // update with the propagator if it's been 2 seconds
				if (noOE || !timeKnown) {
					break; // we can't propagate yet
				}

				if (!propagating) {
					logger->Info("GPSServer: no data, starting propagation");
					TLM_BEGIN_PROPAGATION();
					propagating = true;
					ECItoOE();
				}

				lastUpdate = readTime;
				UpdateAndPropagate();
				ECItoECEF();
			}
			break;
		case GPS_NO_LOCK:
			lastData = readTime;

			if (readTime > lastLock + 5*60*1000) { // if it's been 15 minutes since our last lock, reboot
				logger->Warning("GPSServer: no lock, rebooting");
				RestartGPS();
				lastData = readTime;
				lastLock = readTime;
			}

			if (readTime > lastUpdate + 2000) { // update with the propagator if it's been 2 seconds
				if (noOE || !timeKnown) {
					break; // we can't propagate yet
				}

				if (!propagating) {
					logger->Info("GPSServer: no lock, starting propagation");
					TLM_BEGIN_PROPAGATION();
					propagating = true;
					ECItoOE();
				}

				lastUpdate = readTime;
				UpdateAndPropagate();
				ECItoECEF();
			}
			break;
		case GPS_LOCK:
			noOE = false;
			if (propagating) {
				logger->Info("GPSServer: lock, ending propagation");
				TLM_END_PROPAGATION();
				propagating = false;
			}

			lastLock = readTime;
			lastData = readTime;
			lastUpdate = readTime;
			inertialTime = readTime;
			ECEFtoECI();
			WriteLockToFile();
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

void GPSServer::RestartGPS(void) {
	CDHServer * cdhServer = static_cast<CDHServer *> (Factory::GetInstance(CDH_SERVER_SINGLETON));
	Logger * logger = static_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));

	// turn the GPS off and wait 2 seconds
	cdhServer->subPowerOff(HARDWARE_LOCATION_GPS);
	wdmAsleep();
	sleep(2);
	wdmAlive();

	// turn the GPS back on and wait 5 seconds
	cdhServer->subPowerOn(HARDWARE_LOCATION_GPS);
	wdmAsleep();
	sleep(7);
	wdmAlive();

	// configure the GPS to log what we want over serial
	ConfigureGPS();
}

// TODO: add GPS unlock commands from QB50
bool GPSServer::ConfigureGPS(void) {
	char unlog[50] = "UNLOGALL true\n"; // turn off all current logs
	char bestxyz[50] = "log bestxyza ontime 1\n"; // log BESTXYZ every second
	char saveconfig[50] = "SAVECONFIG\n"; // save the configurations
	bool success = true;
	success &= (write(GPSFileDescriptor, unlog, 14) == 14);
	usleep(1000);
	success &= (write(GPSFileDescriptor, bestxyz, 22) == 22);
	usleep(1000);
	success &= (write(GPSFileDescriptor, saveconfig, 11) == 11);
	usleep(1000);
	if (!success) {
		TLM_GPS_CONFIG_FAIL();
	}
	return success;
}

// determine the distance of the satellite to a target in ECEF coordinates
double GPSServer::DistanceTo(double target[3]) {
	double current[3];
	double difference[3];
	if (this->TakeLock(MAX_BLOCK_TIME)) {
		current[0] = GPSDataHolder->posX;
		current[1] = GPSDataHolder->posY;
		current[2] = GPSDataHolder->posZ;
		this->GiveLock();
	} else {
		return 10000.0; // arbitrarily large
	}

	// subtract the position vectors to find the distance vector
	for (uint8 i = 0; i < 3; i++) {
		difference[i] = current[i] - target[i];
	}

	// r = sqrt(x^2 + y^2 + z^2)
	double distance = sqrt(pow(difference[0],2) + pow(difference[1],2) + pow(difference[2],2));
	return distance;
}

GPSReadType GPSServer::ReadData(char * buffer, int fd) {
	Logger * logger = static_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));

	char c;
	uint16 counter = 0;
	bool readSuccess = true;

	logger->SuperDebug("GPSServer: preparing to read data");

	// Check that there's data
	if (read(fd,&c,1) == 1) {
		while (c != '#' && read(fd, &c, 1) == 1);

		if (c != '#') {
			logger->Debug("GPSServer: character is not '#'");
			return GPS_NO_DATA;
		}

		while (read(fd,&c,1) == 1) {
			buffer[counter++] = c;

			// if we've reached the CRC
			if (c == '*') {
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

			// Ensure that the buffer doesn't overflow
			if(counter == 350){
				logger->Warning("GPSServer: Data too long!");
				return GPS_NO_LOCK;
			}
		}

		logger->Debug("GPSServer: Read data from GPS, now processing...");

		if (!BESTXYZProcess(buffer, counter)) {
			logger->Debug("GPSServer: Bad BESTXYZ");
			return GPS_NO_LOCK;
		}
		return GPS_LOCK;
//		}
	} else {
		return GPS_NO_DATA;
	}
}

GPSPositionTime * GPSServer::GetGPSDataPtr(void) {
	return GPSDataHolder;
}

void GPSServer::UpdateAndPropagate() {
	float eciPos[3];
	float eciVel[3];
	int64 currTime = getTimeInMillis();
	float propTime = currTime/1000.0 - lastLock.sysTime;

	propagatePositionVelocity(lastLock.elements, propTime, eciPos, eciVel);

	if (this->TakeLock(MAX_BLOCK_TIME)) {
		GPSInertialCoords->posX = eciPos[0];
		GPSInertialCoords->posY = eciPos[1];
		GPSInertialCoords->posZ = eciPos[2];
		GPSInertialCoords->velX = eciVel[0];
		GPSInertialCoords->velY = eciVel[1];
		GPSInertialCoords->velZ = eciVel[2];
		ConvertToGPSTime(currTime/1000, &(GPSInertialCoords->GPSWeek), &(GPSInertialCoords->GPSSec));
		GPSInertialCoords->isAccurate = 1;
		inertialTime = currTime;
		this->GiveLock();
	} else {
		Logger * logger = static_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
		logger->Warning("GPSServer::UpdateAndPropagate unable to take lock");
	}
}

void GPSServer::ECItoOE() {
	float r[4];
	float v[4];
	r[1] = GPSInertialCoords->posX;
	r[2] = GPSInertialCoords->posY;
	r[3] = GPSInertialCoords->posZ;
	v[1] = GPSInertialCoords->velX;
	v[2] = GPSInertialCoords->velY;
	v[3] = GPSInertialCoords->velZ;

	if (this->TakeLock(MAX_BLOCK_TIME)) {
		lastLock.sysTime = inertialTime / 1000.0;
		rv2elem(MU_EARTH, r, v, &(lastLock.elements));// Get the orbital elements using the last position and velocity
		this->GiveLock();
	} else {
		Logger * logger = static_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
		logger->Warning("GPSServer::ECItoOE unable to take lock");
	}
}

void GPSServer::ECItoECEF() {
	// arrays for the rotation
	double posECI[3];
	double velECI[3];
	double posECEF[3];
	double velECEF[3];
	double gpsTime[2];

	posECI[0] = GPSInertialCoords->posX;
	posECI[1] = GPSInertialCoords->posY;
	posECI[2] = GPSInertialCoords->posZ;
	velECI[0] = GPSInertialCoords->velX;
	velECI[1] = GPSInertialCoords->velY;
	velECI[2] = GPSInertialCoords->velZ;
	gpsTime[0] = GPSInertialCoords->GPSWeek;
	gpsTime[1] = GPSInertialCoords->GPSSec;

	gcrf2wgs(posECI, velECI, gpsTime, posECEF, velECEF);

	if (this->TakeLock(MAX_BLOCK_TIME)) {
		GPSDataHolder->posX = posECEF[0];
		GPSDataHolder->posY = posECEF[1];
		GPSDataHolder->posZ = posECEF[2];
		GPSDataHolder->velX = velECEF[0];
		GPSDataHolder->velY = velECEF[1];
		GPSDataHolder->velZ = velECEF[2];
		GPSDataHolder->GPSWeek = GPSInertialCoords->GPSWeek;
		GPSDataHolder->GPSSec = GPSInertialCoords->GPSSec;
		this->GiveLock();
	} else {
		Logger * logger = static_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
		logger->Warning("GPSServer::ECItoECEF unable to take lock");
	}
}

void GPSServer::ECEFtoECI() {
	// arrays for the rotation
	double posECEF[3];
	double velECEF[3];
	double posECI[3];
	double velECI[3];
	double gpsTime[2];

	posECEF[0] = GPSDataHolder->posX;
	posECEF[1] = GPSDataHolder->posY;
	posECEF[2] = GPSDataHolder->posZ;
	velECEF[0] = GPSDataHolder->velX;
	velECEF[1] = GPSDataHolder->velY;
	velECEF[2] = GPSDataHolder->velZ;
	gpsTime[0] = GPSDataHolder->GPSWeek;
	gpsTime[1] = GPSDataHolder->GPSSec;

	wgs2gcrf(posECEF, velECEF, gpsTime, posECI, velECI);

	if (this->TakeLock(MAX_BLOCK_TIME)) {
		GPSInertialCoords->posX = posECI[0];
		GPSInertialCoords->posY = posECI[1];
		GPSInertialCoords->posZ = posECI[2];
		GPSInertialCoords->velX = velECI[0];
		GPSInertialCoords->velY = velECI[1];
		GPSInertialCoords->velZ = velECI[2];
		GPSInertialCoords->GPSWeek = GPSDataHolder->GPSWeek;
		GPSInertialCoords->GPSSec = GPSDataHolder->GPSSec;
		GPSInertialCoords->isAccurate = 1;
		this->GiveLock();
	} else {
		Logger * logger = static_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
		logger->Warning("GPSServer::ECEFtoECI unable to take lock");
	}
}

void GPSServer::UpdateECEFData(GPSPositionTime * data) {
	if (this->TakeLock(MAX_BLOCK_TIME)) {
		GPSDataHolder->posX = data->posX;
		GPSDataHolder->posY = data->posY;
		GPSDataHolder->posZ = data->posZ;
		GPSDataHolder->velX = data->velX;
		GPSDataHolder->velY = data->velY;
		GPSDataHolder->velZ = data->velZ;
		GPSDataHolder->GPSWeek = data->GPSWeek;
		GPSDataHolder->GPSSec = data->GPSSec;
		this->GiveLock();
	} else {
		Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
		logger->Warning("GPSServer::UpdateECEFData unable to take lock");
	}
}

bool GPSServer::GetECIData(uint8 buffer[GPSInertial::size]) {
	if (this->TakeLock(MAX_BLOCK_TIME)) {
		GPSInertialCoords->update(buffer, GPSInertial::size);
		GPSInertialCoords->serialize();
		this->GiveLock();
		return true;
	} else {
		Logger * logger = static_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
		logger->Warning("GPSServer::GetECIData unable to take lock");
		return false;
	}
}

void GPSServer::WriteLockToFile(void) {
	uint8 buffer[GPSInertial::size];
	if (GetECIData(buffer)) {
		FILE * fp = fopen(GPS_LOCK_FILE, "w");

		if (fp != NULL) {
			fwrite(buffer, sizeof(uint8), GPSInertial::size, fp);
			fclose(fp);
		}
	}
}

void GPSServer::bootLastLock(void) {
	if (access(GPS_LOCK_FILE, F_OK) != 0) {
		return;
	}

	uint8 buffer[GPSInertial::size];
	FILE * fp = fopen(GPS_LOCK_FILE, "r");

	if (fp == NULL) {
		return;
	}

	if (fread(buffer, sizeof(uint8), GPSInertial::size, fp) == GPSInertial::size) {
		GPSInertial tempStruct;
		tempStruct.update(buffer, GPSInertial::size);
		tempStruct.deserialize();

		uint32 secondsFromEpoch = 0;
		ConvertToEpochTime(tempStruct.GPSWeek, tempStruct.GPSSec, &secondsFromEpoch);

		if ((float) secondsFromEpoch > lastLock.sysTime) { // ie. if the last lock is newer than the config
			GPSInertialCoords->GPSSec = tempStruct.GPSSec;
			GPSInertialCoords->GPSWeek = tempStruct.GPSWeek;
			GPSInertialCoords->posX = tempStruct.posX;
			GPSInertialCoords->posY = tempStruct.posY;
			GPSInertialCoords->posZ = tempStruct.posZ;
			GPSInertialCoords->velX = tempStruct.velX;
			GPSInertialCoords->velY = tempStruct.velY;
			GPSInertialCoords->velZ = tempStruct.velZ;
			ECItoOE();
			noOE = false;
		}
	} else {
		remove(GPS_LOCK_FILE);
	}
	fclose(fp);
}

void GPSServer::bootConfig(void) {
	Logger * logger = static_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));

	FILE * fp = fopen(GPS_CONFIG, "r");
	uint8 buffer[GPSConfiguration.size];

	// make sure we get a valid file pointer
	if (fp == NULL) {
		logger->Error("GPSServer: NULL GPS config file pointer, cannot boot");
		return;
	}

	// read and update the configs
	if (fread(buffer, sizeof(uint8), GPSConfiguration.size, fp) == GPSConfiguration.size) {
		GPSConfiguration.update(buffer, GPSConfiguration.size, 0, 0);
		GPSConfiguration.deserialize();
		logger->Info("GPSServer: successfully booted GPS configs");
		fclose(fp);
		UpdateOEfromConfig();
		return;
	} else {
		logger->Error("GPSServer: error reading GPS config file, cannot boot");
		fclose(fp);
		return;
	}
}

bool GPSServer::updateConfig(void) {
	Logger * logger = static_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));

	FILE * fp = fopen(GPS_CFG_UP, "r");
	uint8 buffer[GPSConfiguration.size];

	// make sure we get a valid file pointer
	if (fp == NULL) {
		logger->Error("GPSServer: NULL GPS config file pointer, cannot update");
		return false;
	}

	// read and update the configs
	if (fread(buffer, sizeof(uint8), GPSConfiguration.size, fp) == GPSConfiguration.size) {
		GPSConfiguration.update(buffer, GPSConfiguration.size, 0, 0);
		GPSConfiguration.deserialize();
		logger->Info("GPSServer: successfully updated GPS configs");
		fclose(fp);
		UpdateOEfromConfig();
		return true;
	} else {
		logger->Error("GPSServer: error reading GPS config file, cannot update");
		fclose(fp);
		return false;
	}
}

void GPSServer::UpdateOEfromConfig() {
	if (this->TakeLock(MAX_BLOCK_TIME)) {
		lastLock.elements.a = GPSConfiguration.a;
		lastLock.elements.e = GPSConfiguration.e;
		lastLock.elements.i = GPSConfiguration.i;
		lastLock.elements.omega = GPSConfiguration.omega;
		lastLock.elements.Omega = GPSConfiguration.Omega;
		lastLock.elements.anom = GPSConfiguration.anom;
		lastLock.sysTime = GPSConfiguration.epochSeconds;
		this->GiveLock();
		noOE = false;
	} else {
		Logger * logger = static_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
		logger->Warning("GPSServer::UpdateOEfromConfig unable to take lock");
	}
}

}
}
