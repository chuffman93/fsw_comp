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

namespace AllStar {
namespace Servers {

// Debug
bool ACSToggleLED(bool state) {
	Logger * logger = static_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));

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
		logger->Log(LOGGER_LEVEL_WARN, "ACSStdTasks: NULL LED Toggle return");
		return false;
	}

	if(ret->isSuccess()){
		logger->Log(LOGGER_LEVEL_DEBUG, "ACS LED Toggle successful");
		return true;
	}else{
		logger->Log(LOGGER_LEVEL_WARN, "ACS LED Toggle failed");
		return false;
	}
}

bool ACSBlinkRate(uint16 rate) {
	Logger * logger = static_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));

	uint8 * bufferOut = (uint8 *) malloc(sizeof(uint16));
	AddUInt16(bufferOut, rate);

	ACPPacket * query = new ACPPacket(SERVER_LOCATION_ACS, HARDWARE_LOCATION_ACS, LED_RATE_CONFIG, 2, bufferOut);
	ACPPacket * ret = DispatchPacket(query);

	if(ret == NULL){
		logger->Log(LOGGER_LEVEL_WARN, "ACSStdTasks: NULL blink rate return");
		return false;
	}

	if(ret->isSuccess()){
		logger->Log(LOGGER_LEVEL_DEBUG, "ACS LED set rate successful");
		return true;
	}else{
		logger->Log(LOGGER_LEVEL_WARN, "ACS LED set rate failed");
		return false;
	}
}

int ACSLEDData() {
	Logger * logger = static_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));

	ACPPacket * query = new ACPPacket(SERVER_LOCATION_ACS, HARDWARE_LOCATION_ACS, LED_RATE_DATA);
	ACPPacket * ret = DispatchPacket(query);

	if(ret == NULL){
		logger->Log(LOGGER_LEVEL_WARN, "ACSStdTasks: NULL LED Data return");
		return false;
	}

	if(ret->getLength() != 3){
		logger->Log(LOGGER_LEVEL_WARN, "ACSStdTasks: Incorrect LED data return length");
		return false;
	}else{
		if(ret->getMessageBuff() == NULL){
			logger->Log(LOGGER_LEVEL_WARN, "ACSStdTasks: NULL LED Data message buffer");
			return false;
		}

		uint8 * msgPtr = ret->getMessageBuff();
		uint8 powerStatus = GetUInt8(msgPtr++);
		uint16 result = GetUInt16(msgPtr);
		logger->Log(LOGGER_LEVEL_DEBUG, "ACS LED power state: %u", powerStatus);
		logger->Log(LOGGER_LEVEL_DEBUG, "ACS LED blink rate:  %u", result);
		return (powerStatus + result);
	}
}

// Standard
void ACSPrepReset() {
	ACPPacket * cmd = new ACPPacket(SERVER_LOCATION_ACS, HARDWARE_LOCATION_ACS, SUBSYSTEM_RESET_CMD);
	ACPPacket * ret = DispatchPacket(cmd); // no need to check the return, we're restarting anyway
}

// Diagnostic
bool ACSTestAlive() {
	Logger * logger = static_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));

	ACPPacket * query = new ACPPacket(SERVER_LOCATION_ACS, HARDWARE_LOCATION_ACS, TEST_ALIVE_CMD);
	ACPPacket * ret = DispatchPacket(query);

	if(ret->isSuccess()){
		return true;
	}else{
		return false;
	}
}

bool ACSTestDriver(uint8 driverID, float rwTorque, float trTorque) {
	Logger * logger = static_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
	logger->Log(LOGGER_LEVEL_INFO, "ACSStdTasks: running test driver");

	uint8 * bufferOut = (uint8 *) malloc(1+2*sizeof(float));
	bufferOut[0] = driverID;
	AddFloat(bufferOut+1,rwTorque);
	AddFloat(bufferOut+5,trTorque);

	ACPPacket * query = new ACPPacket(SERVER_LOCATION_ACS, HARDWARE_LOCATION_ACS, ACS_TEST_DRIVER, 1+2*sizeof(float), bufferOut);
	ACPPacket * ret = DispatchPacket(query);

	if(ret == NULL){
		logger->Log(LOGGER_LEVEL_WARN, "ACSStdTasks: NULL test driver return");
		return false;
	}

	if(ret->isSuccess()){
		logger->Log(LOGGER_LEVEL_INFO, "ACS test driver successful");
		return true;
	}else{
		logger->Log(LOGGER_LEVEL_WARN, "ACS test driver failed");
		logger->Log(LOGGER_LEVEL_WARN, "Error: %u", ret->getErrorStatus());
		return false;
	}
}

// Command/Data
bool ACSSendGPS() {
	GPSServer * gpsServer = static_cast<GPSServer *> (Factory::GetInstance(GPS_SERVER_SINGLETON));
	Logger * logger = static_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
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

	return (ret->isSuccess());
}

bool ACSPointSun() {
	Logger * logger = static_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
	logger->Log(LOGGER_LEVEL_INFO, "ACS: commanding ACS to point towards the sun");

	ACPPacket * command = new ACPPacket(SERVER_LOCATION_ACS, HARDWARE_LOCATION_ACS, ACS_POINT_SUN);
	ACPPacket * response = DispatchPacket(command);

	return (response->getOpcode() == ACS_POINT_SUN && response->isSuccess());
}

bool ACSPointNadir() {
	Logger * logger = static_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
	logger->Log(LOGGER_LEVEL_INFO, "ACS: commanding ACS to point towards nadir");

	ACPPacket * command = new ACPPacket(SERVER_LOCATION_ACS, HARDWARE_LOCATION_ACS, ACS_POINT_NADIR);
	ACPPacket * response = DispatchPacket(command);

	return (response->isSuccess());
}

bool ACSPointGND() {
	Logger * logger = static_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
	logger->Log(LOGGER_LEVEL_INFO, "ACS: commanding ACS to point towards the ground station");

	ACPPacket * command = new ACPPacket(SERVER_LOCATION_ACS, HARDWARE_LOCATION_ACS, ACS_POINT_GND);
	ACPPacket * response = DispatchPacket(command);

	return (response->isSuccess());
}

bool ACSPointDest() {
	Logger * logger = static_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
	logger->Log(LOGGER_LEVEL_INFO, "ACS: commanding ACS to point towards a given destination");

	ACPPacket * command = new ACPPacket(SERVER_LOCATION_ACS, HARDWARE_LOCATION_ACS, ACS_POINT_DEST);
	ACPPacket * response = DispatchPacket(command);

	return (response->isSuccess());
}

bool ACSDetumble() {
	Logger * logger = static_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
	logger->Log(LOGGER_LEVEL_INFO, "ACS: commanding ACS to detumble");

	ACPPacket * command = new ACPPacket(SERVER_LOCATION_ACS, HARDWARE_LOCATION_ACS, ACS_DETUMBLE);
	ACPPacket * response = DispatchPacket(command);

	return (response->isSuccess());
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
