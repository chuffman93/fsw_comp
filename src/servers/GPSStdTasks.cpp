/*
 * GPSStdTasks.cpp
 * Written by: Conrad Hougen
 * Created: 7/10/12
 *
 * Modified: Alex St. Clair
 */

#include "HAL/GPS.h"
#include "HAL/RTC.h"
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
using namespace AllStar::HAL;

namespace AllStar{
namespace Servers{

/* ------------- To configure GPS -------------
 * echo "log bestxyz ontime 1" > /dev/ttyS1/
 * echo "log gprmc ontime 1" > /dev/ttyS1/
 * echo "SAVECONFIG" > /dev/ttyS1/
 */


ACPPacket * GPSReset(){
	//TODO: Figure out how to reset the GPS chip
	return NULL;
}

bool BESTXYZProcess(char * buffer,const size_t size)
{
	GPSServer * gpsServer = dynamic_cast<GPSServer *> (Factory::GetInstance(GPS_SERVER_SINGLETON));
	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
	GPS_BESTXYZ * gpsData = gpsServer->GetGPSDataPtr();
	char * token;
	char * buffPtr = buffer;
	bool solSuccess = true;

	logger->Log("GPSStdTasks: Processing BESTXYZ", LOGGER_LEVEL_DEBUG);

	bool containsDelimiter = false;
	while((buffPtr - buffer != 350) && (*buffPtr != '\0')){
		if(*buffPtr++ == '*'){
			containsDelimiter = true;
			break;
		}
	}

	if(!containsDelimiter){
		logger->Log("GPSStdTasks: BESTXYZ doesn't contain '*'", LOGGER_LEVEL_WARN);
		return false;
	}

	// split into message and crc
	char * message = strtok(buffer, "*");
	char * crcStr = strtok(NULL, "*");
	uint32 crc = strtoul(crcStr, &token, 16);


	// validate crc
	if(crc != CalculateCRC_GPS(message)){
		logger->Log("GPSStdTasks: invalid CRC!", LOGGER_LEVEL_WARN);
		return false;
	}

	// parse the message into header and log
	char * header = strtok(message, ";");
	char * log = strtok(NULL, ";");

	// parse the message header
	if(strcmp("BESTXYZA",strtok(header, ",")) != 0){
		logger->Log("GPSStdTasks: Wrong message, expecting BESTXYZA", LOGGER_LEVEL_WARN);
		return false;
	}
	token = strtok(NULL, ","); // (UNUSED) port
	token = strtok(NULL, ","); // (UNUSED) sequence num
	token = strtok(NULL, ","); // (UNUSED) idle time
	token = strtok(NULL, ","); // (UNUSED) time status
	gpsData->GPSWeek = strtoul(strtok(NULL, ","), NULL, 10);
	gpsData->GPSSec = strtof(strtok(NULL, ","), NULL);
	token = strtok(NULL, ","); // (UNUSED) receiver status
	token = strtok(NULL, ","); // (UNUSED) port
	token = strtok(NULL, ","); // (UNUSED) reserved for vendor
	token = strtok(NULL, ","); // (UNUSED) software build

	// parse the position log part of the message
	if(strcmp("SOL_COMPUTED",strtok(log, ",")) != 0){
		logger->Log("GPSStdTasks: No position solution computed!", LOGGER_LEVEL_WARN);
		solSuccess = false;
	}else{
		logger->Log("GPSStdTasks: Valid position solution computed!", LOGGER_LEVEL_DEBUG);
		token = strtok(NULL,","); // (UNUSED) position type
		gpsData->posX = strtod(strtok(NULL, ","), NULL);
		gpsData->posY = strtod(strtok(NULL, ","), NULL);
		gpsData->posZ = strtod(strtok(NULL, ","), NULL);
		gpsData->stdDevPX = strtod(strtok(NULL, ","), NULL);
		gpsData->stdDevPY = strtod(strtok(NULL, ","), NULL);
		gpsData->stdDevPZ = strtod(strtok(NULL, ","), NULL);
	}

	// parse the velocity log part of the message
	if(strcmp("SOL_COMPUTED",strtok(NULL, ",")) != 0){
		logger->Log("GPSStdTasks: No velocity solution computed!", LOGGER_LEVEL_WARN);
		solSuccess = false;
	}else{
		logger->Log("GPSStdTasks: Valid velocity solution computed!", LOGGER_LEVEL_DEBUG);
		token = strtok(NULL,","); // (UNUSED) velocity type
		gpsData->velX = strtod(strtok(NULL, ","), NULL);
		gpsData->velY = strtod(strtok(NULL, ","), NULL);
		gpsData->velZ = strtod(strtok(NULL, ","), NULL);
		gpsData->stdDevVX = strtod(strtok(NULL, ","), NULL);
		gpsData->stdDevVY = strtod(strtok(NULL, ","), NULL);
		gpsData->stdDevVZ = strtod(strtok(NULL, ","), NULL);
	}

	// parse the rest of the log
	token = strtok(NULL, ","); // (UNUSED) base station
	gpsData->latency = strtof(strtok(NULL, ","), NULL);
	gpsData->diffAge = strtof(strtok(NULL, ","), NULL);
	gpsData->solAge = strtof(strtok(NULL, ","), NULL);
	gpsData->numTracked = (uint8) atoi(strtok(NULL, ","));
	gpsData->numSolution = (uint8) atoi(strtok(NULL, ","));
	token = strtok(NULL, ","); // (UNUSED) GPS + GLONASS L1
	token = strtok(NULL, ","); // (UNUSED) GPS + GLONASS L1/L2
	token = strtok(NULL, ","); // (UNUSED) reserved for vendor
	token = strtok(NULL, ","); // (UNUSED) extended solution status
	token = strtok(NULL, ","); // (UNUSED) reserved for vendor
	token = strtok(NULL, ","); // (UNUSED) signals used

	gpsData->round_seconds = (uint16) lroundf(gpsData->GPSSec);

	if(!solSuccess){
		logger->Log("GPSStdTasks: Number of satellites tracked: %d", gpsData->numTracked, LOGGER_LEVEL_DEBUG);
	}

	logger->Log("GPSStdTasks: Successfully processed BESTXYZ data", LOGGER_LEVEL_INFO);
	return true;
}

bool GPRMCProcess(char * buffer, const size_t size){
	GPSServer * gpsServer = dynamic_cast<GPSServer *> (Factory::GetInstance(GPS_SERVER_SINGLETON));
	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
	double doubleHolder;
	char * token;
	char * buffPtr = buffer;

	logger->Log("GPSStdTasks: Processing GPRMC", LOGGER_LEVEL_DEBUG);

	bool containsDelimiter = false;
	while((buffPtr - buffer != 350) && (*buffPtr != '\0')){
		if(*buffPtr++ == '*'){
			containsDelimiter = true;
			break;
		}
	}

	if(!containsDelimiter){
		logger->Log("GPSStdTasks: GPRMC doesn't contain '*'", LOGGER_LEVEL_WARN);
		return false;
	}

	// split into message and checksum
	char * message = strtok(buffer, "*");
	char * checkStr = strtok(NULL, "*");
	long check = strtol(checkStr, NULL, 16);

	// validate checksum
	if(check != CalculateNMEAChecksum(message)){
		logger->Log("GPSStdTasks: invalid checksum!", LOGGER_LEVEL_WARN);
		return false;
	}

	// parse the message
	token = strtok(message, ","); // GPRMC
	token = strtok(NULL, ","); // UTC
	token = strtok(NULL, ","); // status
	if(*token != 'A'){
		logger->Log("GPSStdTasks: invalid data from GPRMC", LOGGER_LEVEL_WARN);
		return false;
	}

	// latitude
	token = strtok(NULL, ",");
	doubleHolder = nmea_to_deg(token);

	// latitude direction
	token = strtok(NULL, ",");
	if(*token == 'N'){
		gpsServer->GPSCoordsHolder->latitude = doubleHolder;
	}else{
		gpsServer->GPSCoordsHolder->latitude = -1.0 * doubleHolder;
	}

	// longitude
	token = strtok(NULL, ",");
	doubleHolder = nmea_to_deg(token);

	// longitude direction
	token = strtok(NULL, ",");
	if(*token == 'E'){
		gpsServer->GPSCoordsHolder->longitude = doubleHolder;
	}else{
		gpsServer->GPSCoordsHolder->longitude = -1.0 * doubleHolder;
	}

	token = strtok(NULL, ","); // speed (knots)
	token = strtok(NULL, ","); // track true
	token = strtok(NULL, ","); // date
	token = strtok(NULL, ","); // magnetic variation
	token = strtok(NULL, ","); // magnetic variation direction
	token = strtok(NULL, ","); // mode indicator AND checksum

	logger->Log("GPSStdTasks: Successfully processed GPRMC data", LOGGER_LEVEL_INFO);
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
uint32 CRCValue_GPS(int i){
	int j;
	uint32 ulCRC;

	ulCRC = i;
	for(j = 8 ; j > 0; j--){
		if(ulCRC & 1){
			ulCRC = (ulCRC >> 1) ^ CRC32_POLYNOMIAL;
		}else{
			ulCRC >>= 1;
		}
	}
	return ulCRC;
}

// modified slightly from page 32 of http://www.novatel.com/assets/Documents/Manuals/om-20000094.pdf
uint32 CalculateCRC_GPS(char * buffer){
	uint32 temp1;
	uint32 temp2;
	uint32 CRC = 0;

	while(*buffer != '\0'){
		temp1 = (CRC >> 8) & 0x00FFFFFFL;
		temp2 = CRCValue_GPS(((int) CRC ^ ((uint8) *buffer++)) & 0xff);
		CRC = temp1 ^ temp2;
	}

	return CRC;
}

uint8 CalculateNMEAChecksum(char * buffer){
	uint32 len = strlen(buffer);
	uint8 checksum = 0;

	for(uint8 it = 0; it < len; it++){
		checksum ^= (uint8) *(buffer+it);
	}

	return checksum;
}
}
}
