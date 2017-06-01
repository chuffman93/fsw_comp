/*
 * PayloadServer.cpp
 *
 *  Created on: May 27, 2011
 *      Author: Michael
 *
 *  Last Updated by: Robert Belter
 */

#include "servers/PLDServer.h"
#include "servers/PLDStdTasks.h"
#include "servers/DispatchStdTasks.h"
#include "servers/FileSystem.h"
#include "core/Singleton.h"
#include "core/Factory.h"
#include "core/WatchdogManager.h"
#include "core/StdTypes.h"
#include "core/ModeManager.h"
#include "util/FileHandler.h"
#include "util/Logger.h"
#include "servers/CDHServer.h"

using namespace std;
using namespace AllStar::Core;

namespace AllStar{
namespace Servers{

PLDConfig PLDServer::PLDConfiguration(0);

PLDServer::PLDServer(string nameIn, LocationIDType idIn) :
		SubsystemServer(nameIn, idIn, 1000, 20), Singleton() {
		//PLDState(0,0,{0,0,0,0,0,0,0,0,0,0},0,0,0){
	PLDState.byteSize = 12*sizeof(uint8) + 2*sizeof(uint16);
}

PLDServer::~PLDServer(){
}

PLDServer & PLDServer::operator=(const PLDServer & source){
	if (this == &source){
		return *this;
	}

	SubsystemServer::operator =(source);

	return *this;
}

bool PLDServer::IsFullyInitialized(void){
	return(Singleton::IsFullyInitialized());
}

// ------------------------------------ State Machine ------------------------------------------------------------
void PLDServer::loopInit(){
	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
	logger->Log(LOGGER_LEVEL_INFO, "PLDServer: initializing");

	bootConfig();

	// TODO: synchronize clock with GPS

	//No need to do anything
	ModeManager * modeManager = dynamic_cast<ModeManager *> (Factory::GetInstance(MODE_MANAGER_SINGLETON));
	if(modeManager->GetMode() != MODE_STARTUP){
		//Wait until we are not in startup mode, then transition to IDLE
		currentState = ST_IDLE;
	}
}

void PLDServer::loopIdle(){
	ModeManager * modeManager = dynamic_cast<ModeManager *> (Factory::GetInstance(MODE_MANAGER_SINGLETON));
	SystemModeEnum currentMode = modeManager->GetMode();
	switch(currentMode){
	case MODE_PLD_PRIORITY:
		currentState = ST_STARTUP;
		break;
	case MODE_RESET:
		currentState = ST_RESET;
		break;
	default:
		break;
	}
}

void PLDServer::loopStartup(){
	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
	CDHServer * cdhServer = dynamic_cast<CDHServer *> (Factory::GetInstance(CDH_SERVER_SINGLETON));
	cdhServer->resetAssert(HARDWARE_LOCATION_PLD);
	cdhServer->subPowerOn(HARDWARE_LOCATION_PLD);
	usleep(300000);
	cdhServer->resetDeassert(HARDWARE_LOCATION_PLD);

	usleep(1000000);

	if(PLDTestAlive()){
		if(!PLDSelfCheck()){
			logger->Log(LOGGER_LEVEL_FATAL, "PLD failed self check!");
			cdhServer->subPowerOff(HARDWARE_LOCATION_PLD);
			currentState = ST_IDLE;
			return;
		}
		logger->Log(LOGGER_LEVEL_INFO, "PLD passed self check");
	}else{
		logger->Log(LOGGER_LEVEL_FATAL, "PLD non-responsive in init loop");
		cdhServer->subPowerOff(HARDWARE_LOCATION_PLD);
		currentState = ST_IDLE;
		return;
	}

	ACPPacket * turnOnScience = new ACPPacket(SERVER_LOCATION_PLD, HARDWARE_LOCATION_PLD, PLD_START_SCIENCE);
	ACPPacket * ret = DispatchPacket(turnOnScience);
	if(ret->getOpcode() != PLD_START_SCIENCE){
		//TODO Error handling
		currentState = ST_IDLE;
	}
	delete ret;
	//PLD is on and in science mode. Goto science loop
	sleepTime = 3000;
	currentState = ST_SCIENCE;
}

// Graceful shutdown
void PLDServer::loopShutdown(){
	CDHServer * cdhServer = dynamic_cast<CDHServer *> (Factory::GetInstance(CDH_SERVER_SINGLETON));

	ACPPacket * turnOffScience = new ACPPacket(SERVER_LOCATION_PLD, HARDWARE_LOCATION_PLD, SUBSYSTEM_RESET_CMD);
	ACPPacket * ret = DispatchPacket(turnOffScience);
	delete ret;

	wdmAsleep();
	usleep(3000000);
	wdmAlive();

	cdhServer->subPowerOff(HARDWARE_LOCATION_PLD);
	//PLD is off. Goto idle loop
	sleepTime = 1000;
	currentState = ST_IDLE;

}
void PLDServer::loopScience(){
	// Make sure PLD hasn't been turned off due to a fault, if it has, go back into startup
	CDHServer * cdhServer = dynamic_cast<CDHServer *> (Factory::GetInstance(CDH_SERVER_SINGLETON));
	if(cdhServer->subsystemPowerStates[HARDWARE_LOCATION_PLD] == false){
		currentState = ST_STARTUP;
	}

	// Stay in science mode until we move out of PLD_PRIORITY
	ModeManager * modeManager = dynamic_cast<ModeManager *> (Factory::GetInstance(MODE_MANAGER_SINGLETON));
	if(modeManager->GetMode() != MODE_PLD_PRIORITY){
		currentState = ST_SHUTDOWN;
	}

	ACPPacket * dataRequest = new ACPPacket(SERVER_LOCATION_PLD, HARDWARE_LOCATION_PLD, PLD_BACKUP_SEND_SCIENCE);
	ACPPacket * data = DispatchPacket(dataRequest);

	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
	logger->Log(LOGGER_LEVEL_INFO, "PLDServer: received data %d", data->getLength());
}

void PLDServer::loopReset(){
	PLDPrepReset();

	for(uint8 i = 0; i < 60; i++){
		usleep(1000000);
	}

	currentState = ST_IDLE;
}

// -------------------------------------------- PLD Methods ---------------------------------------------
bool PLDServer::CheckHealthStatus(){
	ModeManager * modeManager = dynamic_cast<ModeManager *> (Factory::GetInstance(MODE_MANAGER_SINGLETON));

	if(modeManager->GetMode() == MODE_PLD_PRIORITY){
		Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));

		ACPPacket * HSQuery = new ACPPacket(SERVER_LOCATION_PLD, HARDWARE_LOCATION_PLD, HEALTH_STATUS_CMD);
		ACPPacket * HSRet = DispatchPacket(HSQuery);

		if(HSRet == NULL){
			logger->Log(LOGGER_LEVEL_ERROR, "PLDServer: NULL HSRet");
			return false;
		}

		if(HSRet->getLength() != PLDState.byteSize){
			logger->Log(LOGGER_LEVEL_WARN, "PLDServer: CheckHealthStatus(): incorrect message length!");
			printf("Length: %u\n", HSRet->getLength());

			//TODO: return error?
			return false;
		}else{
			logger->Log(LOGGER_LEVEL_INFO, "PLDServer: CheckHealthStatus(): packet dispatched, HSRet acquired");

			// Parse buffer
			uint8 * msgPtr = HSRet->getMessageBuff();
			if(msgPtr==NULL){
				logger->Log(LOGGER_LEVEL_ERROR, "PLDServer: CheckHealthStatus(): NULL msgPtr");
				return false;
			}

			// deserialize the message
			PLDState.update(msgPtr, PLDState.size, 0, 0);
			PLDState.deserialize();

			logger->Log(LOGGER_LEVEL_DEBUG, "PLD H&S: Power fault:     %u", PLDState.powerFault);
			logger->Log(LOGGER_LEVEL_DEBUG, "PLD H&S: Motor speed:     %u", PLDState.motorSpeed);
			for(uint8 i = 0; i < 10; i++){
				logger->Log(LOGGER_LEVEL_DEBUG, "PLD H&S: Thermistor %d:     %u", i, PLDState.thermistors[i]);
			}
			logger->Log(LOGGER_LEVEL_DEBUG, "PLD H&S: ACS Working:     %u", PLDState.adcDataWorking);
			logger->Log(LOGGER_LEVEL_DEBUG, "PLD H&S: Control:         %u", PLDState.control);

			return true;
		}
	}
}

void PLDServer::bootConfig() {
	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));

	FILE * fp = fopen(PLD_CONFIG, "r");
	uint8 buffer[PLDConfiguration.size];

	// make sure we get a valid file pointer
	if (fp == NULL) {
		logger->Log(LOGGER_LEVEL_ERROR, "PLDServer: NULL PLD config file pointer, cannot boot");
		return;
	}

	// read and update the configs
	if (fread(buffer, sizeof(uint8), PLDConfiguration.size, fp) == PLDConfiguration.size) {
		PLDConfiguration.update(buffer, PLDConfiguration.size, 0, 0);
		PLDConfiguration.deserialize();
		logger->Log(LOGGER_LEVEL_INFO, "PLDServer: successfully booted PLD configs");
		fclose(fp);
		return;
	} else {
		logger->Log(LOGGER_LEVEL_ERROR, "PLDServer: error reading PLD config file, cannot boot");
		fclose(fp);
		return;
	}
}

bool PLDServer::updateConfig() {
	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));

	FILE * fp = fopen(PLD_CFG_UP, "r");
	uint8 buffer[PLDConfiguration.size];

	// make sure we get a valid file pointer
	if (fp == NULL) {
		logger->Log(LOGGER_LEVEL_ERROR, "PLDServer: NULL PLD config file pointer, cannot update");
		return false;
	}

	// read and update the configs
	if (fread(buffer, sizeof(uint8), PLDConfiguration.size, fp) == PLDConfiguration.size) {
		PLDConfiguration.update(buffer, PLDConfiguration.size, 0, 0);
		PLDConfiguration.deserialize();
		logger->Log(LOGGER_LEVEL_INFO, "PLDServer: successfully updated PLD configs");
		fclose(fp);
		return true;
	} else {
		logger->Log(LOGGER_LEVEL_ERROR, "PLDServer: error reading PLD config file, cannot update");
		fclose(fp);
		return false;
	}
}

} // end namespace servers
} // end namespace AllStar
