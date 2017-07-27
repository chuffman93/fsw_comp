/*
 * GPSStdTasks.cpp
 * Written by: Conrad Hougen
 * Created: 7/10/12
 *
 * Modified: Alex St. Clair
 */

#include "servers/GPSStdTasks.h"
#include "servers/GPSServer.h"
#include "servers/DispatchStdTasks.h"
#include "core/ModeManager.h"
#include "core/Singleton.h"
#include "core/Factory.h"
#include "util/Logger.h"
#include <string.h>
#include <math.h>

#define CRC32_POLYNOMIAL	0xEDB88320L
#define UTC_OFFSET			315964800
#define SECONDS_PER_WEEK	604800

using namespace std;
using namespace AllStar::Core;

namespace AllStar {
namespace Servers {

/* ------------- To configure GPS -------------
 * echo "log bestxyz ontime 1" > /dev/ttyS1/
 * echo "log gprmc ontime 1" > /dev/ttyS1/
 * echo "SAVECONFIG" > /dev/ttyS1/
 */

bool BESTXYZProcess(char * buffer, const size_t size) {
	GPSServer * gpsServer = static_cast<GPSServer *>(Factory::GetInstance(GPS_SERVER_SINGLETON));
	Logger * logger = static_cast<Logger *>(Factory::GetInstance(LOGGER_SINGLETON));
	GPSPositionTime tempData(0,0,0,0,0,0,0,0);
	char * token;
	char * buffPtr = buffer;
	bool solSuccess = true;
	bool updateTime = false;

	logger->Debug("GPSStdTasks: Processing BESTXYZ");

	bool containsDelimiter = false;
	while ((buffPtr - buffer != 350) && (*buffPtr != '\0')) {
		if (*buffPtr++ == '*') {
			containsDelimiter = true;
			break;
		}
	}

	if (!containsDelimiter) {
		logger->Warning("GPSStdTasks: BESTXYZ doesn't contain '*'");
		return false;
	}

	// split into message and crc
	char * message = strtok(buffer, "*");
	char * crcStr = strtok(NULL, "*");
	uint32 crc = strtoul(crcStr, &token, 16);

	// validate crc
	if (crc != CalculateCRC_GPS(message)) {
		logger->Debug("GPSStdTasks: invalid CRC!");
		//return false; FIXME: when working with the simulator, we need to ignore the CRC
	}

	// parse the message into header and log
	char * header = strtok(message, ";");
	char * log = strtok(NULL, ";");

	// parse the message header
	if (strcmp("BESTXYZA", strtok(header, ",")) != 0) {
		logger->Warning("GPSStdTasks: Wrong message, expecting BESTXYZA");
		return false;
	}

	token = strtok(NULL, ","); // (UNUSED) port
	token = strtok(NULL, ","); // (UNUSED) sequence num
	token = strtok(NULL, ","); // (UNUSED) idle time
	if (strcmp("UNKNOWN", strtok(NULL, ",")) != 0) { // time status is ok
		updateTime = true;
	}
	tempData.GPSWeek = (uint16) strtoul(strtok(NULL, ","), NULL, 10);
	tempData.GPSSec = strtof(strtok(NULL, ","), NULL);
	token = strtok(NULL, ","); // (UNUSED) receiver status
	token = strtok(NULL, ","); // (UNUSED) port
	token = strtok(NULL, ","); // (UNUSED) reserved for vendor
	token = strtok(NULL, ","); // (UNUSED) software build

	// parse the position log part of the message
	if (strcmp("SOL_COMPUTED", strtok(log, ",")) != 0) {
		logger->Debug("GPSStdTasks: No position solution computed!");
		solSuccess = false;
	} else {
		logger->Debug("GPSStdTasks: Valid position solution computed!");
		token = strtok(NULL, ","); // (UNUSED) position type
		tempData.posX = strtod(strtok(NULL, ","), NULL) / 1000.0; // convert from m to km
		tempData.posY = strtod(strtok(NULL, ","), NULL) / 1000.0; // convert from m to km
		tempData.posZ = strtod(strtok(NULL, ","), NULL) / 1000.0; // convert from m to km
		token = strtok(NULL, ","); // (UNUSED) std dev x
		token = strtok(NULL, ","); // (UNUSED) std dev y
		token = strtok(NULL, ","); // (UNUSED) std dev z
	}

	// parse the velocity log part of the message
	if (strcmp("SOL_COMPUTED", strtok(NULL, ",")) != 0) {
		logger->Debug("GPSStdTasks: No velocity solution computed!");
		solSuccess = false;
	} else {
		logger->Debug("GPSStdTasks: Valid velocity solution computed!");
		token = strtok(NULL, ","); // (UNUSED) velocity type
		tempData.velX = strtod(strtok(NULL, ","), NULL) / 1000.0; // convert from m to km
		tempData.velY = strtod(strtok(NULL, ","), NULL) / 1000.0; // convert from m to km
		tempData.velZ = strtod(strtok(NULL, ","), NULL) / 1000.0; // convert from m to km
		token = strtok(NULL, ","); // (UNUSED) std dev x
		token = strtok(NULL, ","); // (UNUSED) std dev y
		token = strtok(NULL, ","); // (UNUSED) std dev z
	}

	// parse the rest of the log
	token = strtok(NULL, ","); // (UNUSED) base station
	token = strtok(NULL, ","); // (UNUSED) latency
	token = strtok(NULL, ","); // (UNUSED) diff age
	token = strtok(NULL, ","); // (UNUSED) sol age
	uint8 tempTracked = (uint8) atoi(strtok(NULL, ","));
	token = strtok(NULL, ","); // (UNUSED) num used in solution
	token = strtok(NULL, ","); // (UNUSED) GPS + GLONASS L1
	token = strtok(NULL, ","); // (UNUSED) GPS + GLONASS L1/L2
	token = strtok(NULL, ","); // (UNUSED) reserved for vendor
	token = strtok(NULL, ","); // (UNUSED) extended solution status
	token = strtok(NULL, ","); // (UNUSED) reserved for vendor
	token = strtok(NULL, ","); // (UNUSED) signals used

	if (updateTime) {
		UpdateTime(tempData.GPSWeek, tempData.GPSSec);
	}

	if (!solSuccess) {
		logger->Debug("GPSStdTasks: Invalid BESTXYZ, numTracked: %d", gpsServer->numTracked);
		return false;
	}

	logger->Debug("GPSStdTasks: Good BESTXYZ data");
	gpsServer->UpdateECEFData(&tempData);
	gpsServer->numTracked = tempTracked;

	return true;
}

// modified slightly from page 32 of http://www.novatel.com/assets/Documents/Manuals/om-20000094.pdf
uint32 CRCValue_GPS(int i) {
	int j;
	uint32 ulCRC;

	ulCRC = i;
	for (j = 8 ; j > 0; j--) {
		if (ulCRC & 1) {
			ulCRC = (ulCRC >> 1) ^ CRC32_POLYNOMIAL;
		} else {
			ulCRC >>= 1;
		}
	}
	return ulCRC;
}

// modified slightly from page 32 of http://www.novatel.com/assets/Documents/Manuals/om-20000094.pdf
uint32 CalculateCRC_GPS(char * buffer) {
	uint32 temp1;
	uint32 temp2;
	uint32 CRC = 0;

	while (*buffer != '\0') {
		temp1 = (CRC >> 8) & 0x00FFFFFFL;
		temp2 = CRCValue_GPS(((int) CRC ^ ((uint8) *buffer++)) & 0xff);
		CRC = temp1 ^ temp2;
	}

	return CRC;
}

// update the time if it's off by >2s
void UpdateTime(uint16 GPSWeek, float GPSSec) {
	uint32 secondsFromEpoch;
	ConvertToEpochTime(GPSWeek, GPSSec, &secondsFromEpoch);

	// if we're off by at least two seconds, update the time
	if (abs(getTimeInSec() - secondsFromEpoch) > 2) {
		char cmd[100];
		sprintf(cmd, "date -s \"@%u\"", secondsFromEpoch);
		system(cmd);
	}
}

void ConvertToEpochTime(uint16 GPSWeek, float GPSSec, uint32 * secondsFromEpoch) {
	uint32 seconds = UTC_OFFSET;
	seconds += GPSWeek * SECONDS_PER_WEEK;
	seconds += (uint32) roundf(GPSSec);
	*secondsFromEpoch = seconds;
}

void ConvertToGPSTime(uint32 currTimeSec, uint16 * GPSWeek, float * GPSSec) {
	uint32 seconds = currTimeSec - UTC_OFFSET;
	*GPSWeek = seconds / SECONDS_PER_WEEK;
	*GPSSec = seconds % SECONDS_PER_WEEK;
}

}
}
