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

#define CRC32_POLYNOMIAL 0xEDB88320L

using namespace std;
using namespace AllStar::Core;

namespace AllStar{
namespace Servers{

/* ------------- To configure GPS -------------
 * echo "log bestxyz ontime 1" > /dev/ttyS1/
 * echo "log gprmc ontime 1" > /dev/ttyS1/
 * echo "SAVECONFIG" > /dev/ttyS1/
 */

bool BESTXYZProcess(char * buffer, const size_t size) {
	GPSServer * gpsServer = static_cast<GPSServer *>(Factory::GetInstance(GPS_SERVER_SINGLETON));
	Logger * logger = static_cast<Logger *>(Factory::GetInstance(LOGGER_SINGLETON));
	char * token;
	char * buffPtr = buffer;
	bool solSuccess = true;

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
		logger->Warning("GPSStdTasks: invalid CRC!");
		return false;
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
	token = strtok(NULL, ","); // (UNUSED) time status
	uint16 tempWeek = (uint16) strtoul(strtok(NULL, ","), NULL, 10);
	float tempSec = strtof(strtok(NULL, ","), NULL);
	token = strtok(NULL, ","); // (UNUSED) receiver status
	token = strtok(NULL, ","); // (UNUSED) port
	token = strtok(NULL, ","); // (UNUSED) reserved for vendor
	token = strtok(NULL, ","); // (UNUSED) software build

	// parse the position log part of the message
	double tempPX = 0;
	double tempPY = 0;
	double tempPZ = 0;
	if (strcmp("SOL_COMPUTED", strtok(log, ",")) != 0) {
		logger->Debug("GPSStdTasks: No position solution computed!");
		solSuccess = false;
	} else {
		logger->Debug("GPSStdTasks: Valid position solution computed!");
		token = strtok(NULL, ","); // (UNUSED) position type
		tempPX = strtod(strtok(NULL, ","), NULL);
		tempPY = strtod(strtok(NULL, ","), NULL);
		tempPZ = strtod(strtok(NULL, ","), NULL);
		token = strtok(NULL, ","); // (UNUSED) std dev x
		token = strtok(NULL, ","); // (UNUSED) std dev y
		token = strtok(NULL, ","); // (UNUSED) std dev z
	}

	// parse the velocity log part of the message
	double tempVX = 0;
	double tempVY = 0;
	double tempVZ = 0;
	if (strcmp("SOL_COMPUTED", strtok(NULL, ",")) != 0) {
		logger->Debug("GPSStdTasks: No velocity solution computed!");
		solSuccess = false;
	} else {
		logger->Debug("GPSStdTasks: Valid velocity solution computed!");
		token = strtok(NULL, ","); // (UNUSED) velocity type
		tempVX = strtod(strtok(NULL, ","), NULL);
		tempVY = strtod(strtok(NULL, ","), NULL);
		tempVZ = strtod(strtok(NULL, ","), NULL);
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

	if (!solSuccess) {
		logger->Debug("GPSStdTasks: Invalid BESTXYZ, numTracked: %d", gpsServer->GPSDataHolder->numTracked);
		return false;
	}

	logger->Debug("GPSStdTasks: Good BESTXYZ data");
	gpsServer->GPSDataHolder->GPSWeek = tempWeek;
	gpsServer->GPSDataHolder->GPSSec = tempSec;
	gpsServer->GPSDataHolder->posX = tempPX;
	gpsServer->GPSDataHolder->posY = tempPY;
	gpsServer->GPSDataHolder->posZ = tempPZ;
	gpsServer->GPSDataHolder->velX = tempVX;
	gpsServer->GPSDataHolder->velY = tempVY;
	gpsServer->GPSDataHolder->velZ = tempVZ;
	gpsServer->GPSDataHolder->numTracked = tempTracked;

	return true;
}

bool GPRMCProcess(char * buffer, const size_t size) {
	GPSServer * gpsServer = static_cast<GPSServer *>(Factory::GetInstance(GPS_SERVER_SINGLETON));
	Logger * logger = static_cast<Logger *>(Factory::GetInstance(LOGGER_SINGLETON));
	double doubleHolder;
	char * token;
	char * buffPtr = buffer;

	logger->Debug("GPSStdTasks: Processing GPRMC");

	bool containsDelimiter = false;
	while ((buffPtr - buffer != 350) && (*buffPtr != '\0')) {
		if (*buffPtr++ == '*') {
			containsDelimiter = true;
			break;
		}
	}

	if (!containsDelimiter) {
		logger->Warning("GPSStdTasks: GPRMC doesn't contain '*'");
		return false;
	}

	// split into message and checksum
	char * message = strtok(buffer, "*");
	char * checkStr = strtok(NULL, "*");
	long check = strtol(checkStr, NULL, 16);

	// validate checksum
	if (check != CalculateNMEAChecksum(message)) {
		logger->Warning("GPSStdTasks: invalid checksum!");
		return false;
	}

	// parse the message
	token = strtok(message, ","); // GPRMC
	token = strtok(NULL, ","); // (UNUSED) UTC
	token = strtok(NULL, ","); // status
	if (*token != 'A') {
		logger->Debug("GPSStdTasks: invalid data from GPRMC");
		return false;
	}

	// latitude
	token = strtok(NULL, ",");
	doubleHolder = nmea_to_deg(token);

	// latitude direction
	token = strtok(NULL, ",");
	if (*token == 'N') {
		gpsServer->GPSCoordsHolder->latitude = doubleHolder;
	} else {
		gpsServer->GPSCoordsHolder->latitude = -1.0 * doubleHolder;
	}

	// longitude
	token = strtok(NULL, ",");
	doubleHolder = nmea_to_deg(token);

	// longitude direction
	token = strtok(NULL, ",");
	if (*token == 'E') {
		gpsServer->GPSCoordsHolder->longitude = doubleHolder;
	} else {
		gpsServer->GPSCoordsHolder->longitude = -1.0 * doubleHolder;
	}

	token = strtok(NULL, ","); // (UNUSED) speed (knots)
	token = strtok(NULL, ","); // (UNUSED) track true
	token = strtok(NULL, ","); // (UNUSED) date
	token = strtok(NULL, ","); // (UNUSED) magnetic variation
	token = strtok(NULL, ","); // (UNUSED) magnetic variation direction
	token = strtok(NULL, ","); // (UNUSED) mode indicator AND checksum

	logger->Info("GPSStdTasks: Successfully processed GPRMC data");
	return true;
}

double nmea_to_deg(char *nmea) {
	int deg;
	double result, raw;

	raw = strtod(nmea, &nmea);

	deg = raw/100;
	result = (raw/100 - deg)*100 ;
	result /= 60;
	result += deg;

	return result;
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

uint8 CalculateNMEAChecksum(char * buffer) {
	uint32 len = strlen(buffer);
	uint8 checksum = 0;

	for (uint8 it = 0; it < len; it++) {
		checksum ^= (uint8) *(buffer+it);
	}

	return checksum;
}

}
}
