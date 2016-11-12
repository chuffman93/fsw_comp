/*
 * ACSStdTasks.cpp
 * Written by: Conrad Hougen
 * Created: 7/10/12
 *
 * Updated: Alex St. Clair
 */

#include "servers/ACSStdTasks.h"
#include "servers/DispatchStdTasks.h"
#include "servers/ACSServer.h"
#include "servers/GPSServer.h"

#include "util/Logger.h"

using namespace std;
using namespace AllStar::Core;
using namespace AllStar::Servers;

namespace AllStar{
namespace Servers{

// Debug
bool ACSToggleLED(bool state){
	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));

	uint8 * buffer = (uint8 *) malloc(sizeof(uint8));
	uint8 stateOut;
	if(state){
		stateOut = 1;
		memcpy(buffer, &stateOut, 1);
	}else{
		stateOut = 0;
		memcpy(buffer, &stateOut, 1);
	}

	ACPPacket * query = new ACPPacket(SERVER_LOCATION_ACS, HARDWARE_LOCATION_ACS, LED_ON_CMD, 1, buffer);
	ACPPacket * ret = DispatchPacket(query);

	if(ret == NULL){
		logger->Log("ACSStdTasks: NULL LED Toggle return", LOGGER_LEVEL_WARN);
		return false;
	}

	if(ret->isSuccess()){
		logger->Log("ACS LED Toggle successful", LOGGER_LEVEL_DEBUG);
		return true;
	}else{
		logger->Log("ACS LED Toggle failed", LOGGER_LEVEL_WARN);
		return false;
	}
}

bool ACSBlinkRate(uint16 rate){
	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));

	uint8 * bufferOut = (uint8 *) malloc(sizeof(uint16));
	AddUInt16(bufferOut, rate);

	ACPPacket * query = new ACPPacket(SERVER_LOCATION_ACS, HARDWARE_LOCATION_ACS, LED_RATE_CONFIG, 2, bufferOut);
	ACPPacket * ret = DispatchPacket(query);

	if(ret == NULL){
		logger->Log("ACSStdTasks: NULL blink rate return", LOGGER_LEVEL_WARN);
		return false;
	}

	if(ret->isSuccess()){
		logger->Log("ACS LED set rate successful", LOGGER_LEVEL_DEBUG);
		return true;
	}else{
		logger->Log("ACS LED set rate failed", LOGGER_LEVEL_WARN);
		return false;
	}
}

int ACSLEDData(){
	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));

	ACPPacket * query = new ACPPacket(SERVER_LOCATION_ACS, HARDWARE_LOCATION_ACS, LED_RATE_DATA);
	ACPPacket * ret = DispatchPacket(query);

	if(ret == NULL){
		logger->Log("ACSStdTasks: NULL LED Data return", LOGGER_LEVEL_WARN);
		return false;
	}

	if(ret->getLength() != 3){
		logger->Log("ACSStdTasks: Incorrect LED data return length", LOGGER_LEVEL_WARN);
		return false;
	}else{
		if(ret->getMessageBuff() == NULL){
			logger->Log("ACSStdTasks: NULL LED Data message buffer", LOGGER_LEVEL_WARN);
			return false;
		}

		uint8 * msgPtr = ret->getMessageBuff();
		uint8 powerStatus = GetUInt8(msgPtr++);
		uint16 result = GetUInt16(msgPtr);
		logger->Log("ACS LED power state: %u", powerStatus, LOGGER_LEVEL_DEBUG);
		logger->Log("ACS LED blink rate:  %u", result, LOGGER_LEVEL_DEBUG);
		return (powerStatus + result);
	}
}

// Diagnostic
bool ACSTestAlive(){
	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));

	ACPPacket * query = new ACPPacket(SERVER_LOCATION_ACS, HARDWARE_LOCATION_ACS, TEST_ALIVE_CMD);
	ACPPacket * ret = DispatchPacket(query);

	if(ret->isSuccess()){
		logger->Log("ACS is alive", LOGGER_LEVEL_DEBUG);
		return true;
	}else{
		logger->Log("ACS is not alive", LOGGER_LEVEL_FATAL);
		return false;
	}
}

ACPPacket * ACSNoReturn(){
	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
	logger->Log("ACSNoReturn: unfinished!", LOGGER_LEVEL_ERROR);
	return NULL;
}

// Command/Data
void ACSHealthStatus(void){
	ACPPacket * HSQuery = new ACPPacket(SERVER_LOCATION_ACS, HARDWARE_LOCATION_ACS, HEALTH_STATUS_CMD);
	ACPPacket * HSRet = DispatchPacket(HSQuery);
	//PacketProcess(SERVER_LOCATION_ACS, HSRet);
}

void ACSSendGPS(){
	GPSServer * gpsServer = dynamic_cast<GPSServer *> (Factory::GetInstance(GPS_SERVER_SINGLETON));
	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
	logger->Log("ACSStdTasks: ACSSendGPS(): Entered", LOGGER_LEVEL_DEBUG);

	uint8 * buffer = (uint8 *) malloc(6*sizeof(double)+sizeof(float)+sizeof(int));
	AddDouble(buffer, gpsServer->GetGPSDataPtr()->posX);
	AddDouble(buffer + 8, gpsServer->GetGPSDataPtr()->posY);
	AddDouble(buffer + 16, gpsServer->GetGPSDataPtr()->posZ);
	AddDouble(buffer + 24, gpsServer->GetGPSDataPtr()->velX);
	AddDouble(buffer + 32, gpsServer->GetGPSDataPtr()->velY);
	AddDouble(buffer + 40, gpsServer->GetGPSDataPtr()->velZ);
	AddUInt32(buffer + 48, gpsServer->GetGPSDataPtr()->GPSWeek);
	AddFloat(buffer + 52, gpsServer->GetGPSDataPtr()->GPSSec);

	ACPPacket * send = new ACPPacket(SERVER_LOCATION_ACS, HARDWARE_LOCATION_ACS, ACS_GPS_CMD, 6*sizeof(double)+sizeof(float)+sizeof(int), buffer);
	ACPPacket * ret = DispatchPacket(send);
	//PacketProcess(SERVER_LOCATION_ACS, ret);
}

bool ACSPointSun(){
	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
	logger->Log("ACS: commanding ACS to point towards the sun", LOGGER_LEVEL_INFO);

	ACPPacket * command = new ACPPacket(SERVER_LOCATION_ACS, HARDWARE_LOCATION_ACS, ACS_POINT_SUN);
	ACPPacket * response = DispatchPacket(command);

	return(response->getOpcode() == ACS_POINT_SUN);
}

bool ACSPointNadir(){
	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
	logger->Log("ACS: commanding ACS to point towards nadir", LOGGER_LEVEL_INFO);

	ACPPacket * command = new ACPPacket(SERVER_LOCATION_ACS, HARDWARE_LOCATION_ACS, ACS_POINT_NADIR);
	ACPPacket * response = DispatchPacket(command);

	return(response->isSuccess());
}

bool ACSPointGND(){
	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
	logger->Log("ACS: commanding ACS to point towards the ground station", LOGGER_LEVEL_INFO);

	ACPPacket * command = new ACPPacket(SERVER_LOCATION_ACS, HARDWARE_LOCATION_ACS, ACS_POINT_GND);
	ACPPacket * response = DispatchPacket(command);

	return(response->isSuccess());
}

bool ACSPointDest(){
	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
	logger->Log("ACS: commanding ACS to point towards a given destination", LOGGER_LEVEL_INFO);

	ACPPacket * command = new ACPPacket(SERVER_LOCATION_ACS, HARDWARE_LOCATION_ACS, ACS_POINT_DEST);
	ACPPacket * response = DispatchPacket(command);

	return(response->isSuccess());
}

// Non-opcode tasks
bool ACSSelfCheck(){
	if(!ACSToggleLED(true))
		return false;
	usleep(1000000);
	if(!ACSBlinkRate(1000))
		return false;
	usleep(1000000);
	if(!(ACSLEDData() == 1001))
		return false;
	return true;
}

}
}
