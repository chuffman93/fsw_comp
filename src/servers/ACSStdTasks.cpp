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

	ACPPacket * query = new ACPPacket(SERVER_LOCATION_ACS, HARDWARE_LOCATION_ACS, ACS_TOGGLE_LED_CMD, 1, buffer);
	ACPPacket * ret = DispatchPacket(query);

	if(ret == NULL){
		logger->Log(LOGGER_LEVEL_WARN, "ACSStdTasks: NULL LED Toggle return");
		return false;
	}

	if(ret->getLength() != sizeof(uint8)){
		logger->Log(LOGGER_LEVEL_WARN, "ACSStdTasks: Incorrect Toggle LED return length");
		return false;
	}else{
		if(ret->getMessageBuff() == NULL){
			logger->Log(LOGGER_LEVEL_WARN, "ACSStdTasks: NULL LED Toggle message buffer");
			return false;
		}

		uint8 result = GetUInt8(ret->getMessageBuff());
		if(result == 1){
			logger->Log(LOGGER_LEVEL_INFO, "ACS LED Toggle successful");
			return true;
		}else{
			logger->Log(LOGGER_LEVEL_WARN, "ACS LED Toggle failed");
			return false;
		}
	}
}

bool ACSBlinkRate(uint16 rate){
	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));

	uint8 * bufferOut = (uint8 *) malloc(sizeof(uint16));
	AddUInt16(bufferOut, rate);

	ACPPacket * query = new ACPPacket(SERVER_LOCATION_ACS, HARDWARE_LOCATION_ACS, ACS_LED_BLINK_RATE_CMD, 2, bufferOut);
	ACPPacket * ret = DispatchPacket(query);

	if(ret == NULL){
		logger->Log(LOGGER_LEVEL_WARN, "ACSStdTasks: NULL blink rate return");
		return false;
	}

	if(ret->getLength() != sizeof(uint8)){
		logger->Log(LOGGER_LEVEL_WARN, "ACSStdTasks: Incorrect blink rate return length");
		return false;
	}else{
		if(ret->getMessageBuff() == NULL){
			logger->Log(LOGGER_LEVEL_WARN, "ACSStdTasks: NULL blink rate message buffer");
			return false;
		}

		uint8 result = GetUInt8(ret->getMessageBuff());
		if(result == 1){
			logger->Log(LOGGER_LEVEL_INFO, "ACS blink rate successful");
			return true;
		}else{
			logger->Log(LOGGER_LEVEL_WARN, "ACS blink rate failed");
			return false;
		}
	}
}

bool ACSLEDData(){
	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));

	ACPPacket * query = new ACPPacket(SERVER_LOCATION_ACS, HARDWARE_LOCATION_ACS, ACS_LED_DATA);
	ACPPacket * ret = DispatchPacket(query);

	if(ret == NULL){
		logger->Log(LOGGER_LEVEL_WARN, "ACSStdTasks: NULL LED Data return");
		return false;
	}

	if(ret->getLength() != sizeof(uint16)){
		logger->Log(LOGGER_LEVEL_WARN, "ACSStdTasks: Incorrect LED data return length");
		return false;
	}else{
		if(ret->getMessageBuff() == NULL){
			logger->Log(LOGGER_LEVEL_WARN, "ACSStdTasks: NULL LED Data message buffer");
			return false;
		}

		uint16 result = GetUInt16(ret->getMessageBuff());
		logger->Log(LOGGER_LEVEL_INFO, "LED blink rate: %u", result);
		return true;
	}
}

// Diagnostic
ACPPacket * ACSTestAlive(){
	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
	logger->Log(LOGGER_LEVEL_ERROR, "ACSTestAlive: unfinished!");
	return NULL;
}

ACPPacket * ACSNoReturn(){
	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
	logger->Log(LOGGER_LEVEL_ERROR, "ACSNoReturn: unfinished!");
	return NULL;
}

// Command/Data
void ACSHealthStatus(void){
	ACPPacket * HSQuery = new ACPPacket(SERVER_LOCATION_ACS, HARDWARE_LOCATION_ACS, ACS_HS_CMD);
	ACPPacket * HSRet = DispatchPacket(HSQuery);
	//PacketProcess(SERVER_LOCATION_ACS, HSRet);
}

void ACSSendGPS(){
	GPSServer * gpsServer = dynamic_cast<GPSServer *> (Factory::GetInstance(GPS_SERVER_SINGLETON));
	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
	logger->Log(LOGGER_LEVEL_DEBUG, "ACSStdTasks: ACSSendGPS(): Entered");

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
	logger->Log(LOGGER_LEVEL_INFO, "ACS: commanding ACS to point towards the sun");

	ACPPacket * command = new ACPPacket(SERVER_LOCATION_ACS, HARDWARE_LOCATION_ACS, ACS_POINT_SUN);
	ACPPacket * response = DispatchPacket(command);

	return(response->getOpcode() == ACS_POINT_SUN);
}

bool ACSPointNadir(){
	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
	logger->Log(LOGGER_LEVEL_INFO, "ACS: commanding ACS to point towards nadir");

	ACPPacket * command = new ACPPacket(SERVER_LOCATION_ACS, HARDWARE_LOCATION_ACS, ACS_POINT_NADIR);
	ACPPacket * response = DispatchPacket(command);

	return(response->getOpcode() == ACS_POINT_NADIR);
}

bool ACSPointGND(){
	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
	logger->Log(LOGGER_LEVEL_INFO, "ACS: commanding ACS to point towards the ground station");

	ACPPacket * command = new ACPPacket(SERVER_LOCATION_ACS, HARDWARE_LOCATION_ACS, ACS_POINT_GND);
	ACPPacket * response = DispatchPacket(command);

	return(response->getOpcode() == ACS_POINT_GND);
}

bool ACSPointDest(){
	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
	logger->Log(LOGGER_LEVEL_INFO, "ACS: commanding ACS to point towards a given destination");

	ACPPacket * command = new ACPPacket(SERVER_LOCATION_ACS, HARDWARE_LOCATION_ACS, ACS_POINT_DEST);
	ACPPacket * response = DispatchPacket(command);

	return(response->getOpcode() == ACS_POINT_DEST);
}

}
}
