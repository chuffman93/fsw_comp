/*
 * COMServer.cpp
 *
 *  Created on: Feb 21, 2011
 *      Author: Michael
 */

#include "servers/COMServer.h"
#include "servers/COMStdTasks.h"
#include "servers/CDHServer.h"
#include "servers/CMDServer.h"
#include "servers/ERRServer.h"
#include "servers/DispatchStdTasks.h"
#include "core/Singleton.h"
#include "core/Factory.h"
#include "core/WatchdogManager.h"
#include "core/StdTypes.h"
#include "core/ModeManager.h"
#include "util/FileHandler.h"
#include "util/Logger.h"

using namespace std;
using namespace AllStar::Core;

namespace AllStar{
namespace Servers{

COMConfig COMServer::COMConfiguration(0);

// -------------------------------------- Necessary Methods --------------------------------------
COMServer::COMServer(string nameIn, LocationIDType idIn) :
		SubsystemServer(nameIn, idIn), Singleton(), TXSilence(false) {
}

COMServer::~COMServer(){
}

COMServer & COMServer::operator=(const COMServer & source){
	if (this == &source){
		return *this;
	}

	SubsystemServer::operator =(source);

	return *this;
}

bool COMServer::IsFullyInitialized(void){
	return(Singleton::IsFullyInitialized());
}

// TODO: ultimately remove Half and Full Duplex states and have a struct to track those COM states through H&S
// -------------------------------------------- State Machine ---------------------------------------------
void COMServer::loopInit(){
	ERRServer * errServer = dynamic_cast<ERRServer *> (Factory::GetInstance(ERR_SERVER_SINGLETON));
	CDHServer * cdhServer = dynamic_cast<CDHServer *> (Factory::GetInstance(CDH_SERVER_SINGLETON));
	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));

	if (!cdhServer->subsystemPowerStates[HARDWARE_LOCATION_COM]) {
		cdhServer->subPowerOn(HARDWARE_LOCATION_COM);

		wdmAsleep();
		usleep(15000000); // wait 15s for boot
		wdmAlive();
	}

	if (COMTestAlive()) {
		if (!COMSelfCheck()) {
			errServer->SendError(ERR_COM_SELFCHECK);
			wdmAsleep();
			usleep(3000000);
			wdmAlive();
			return;
		}

		logger->Log(LOGGER_LEVEL_INFO, "COM passed self check");

		bootConfig();

		currentState = ST_BEACON;
	} else {
		errServer->SendError(ERR_COM_NOTALIVE);
		wdmAsleep();
		usleep(3000000);
		wdmAlive();
	}
}

void COMServer::loopBeacon(){
	CDHServer * cdhServer = dynamic_cast<CDHServer *> (Factory::GetInstance(CDH_SERVER_SINGLETON));

	if(!cdhServer->subsystemPowerStates[HARDWARE_LOCATION_COM]){
		currentState = ST_INIT;
	}

	ModeManager * modeManager = dynamic_cast<ModeManager *>(Factory::GetInstance(MODE_MANAGER_SINGLETON));
	SystemModeEnum currentMode = modeManager->GetMode();
	switch(currentMode){
	case MODE_RESET:
		currentState = ST_RESET;
		break;
	default:
		break;
	}

	if(TXSilence){
		currentState = ST_ENTER_TX_SILENCE;
	}

	CMDServer * cmdServer = dynamic_cast<CMDServer *> (Factory::GetInstance(CMD_SERVER_SINGLETON));
	if(cmdServer->CheckForBeacon()){
		COMSendBeacon();
	}
}

void COMServer::loopEnterTXSilence(){
	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));

	if(!COMSimplex()){
		uint8 i = 0;
		while(i++ < 10){
			if(COMSimplex()){
				i = 20;
			}
		}
		if(i != 20){
			logger->Log(LOGGER_LEVEL_FATAL, "COM can't enter TX silence, serious problem");
		}
	}
	logger->Log(LOGGER_LEVEL_INFO, "COM entering TX silence");
	currentState = ST_TX_SILENCE;
}

void COMServer::loopTXSilence(){
	if(!TXSilence){
		currentState = ST_BEACON;
	}
}

void COMServer::loopReset(){
	COMPrepReset();

	for(uint8 i = 0; i < 60; i++){
		usleep(1000000);
	}

	currentState = ST_BEACON;
}

void COMServer::bootConfig() {
	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));

	FILE * fp = fopen(COM_CONFIG, "r");
	uint8 buffer[COMConfiguration.size];

	// make sure we get a valid file pointer
	if (fp == NULL) {
		logger->Log(LOGGER_LEVEL_ERROR, "COMServer: NULL COM config file pointer, cannot boot");
		return;
	}

	// read and update the configs
	if (fread(buffer, sizeof(uint8), COMConfiguration.size, fp) == COMConfiguration.size) {
		COMConfiguration.update(buffer, COMConfiguration.size, 0, 0);
		COMConfiguration.deserialize();
		logger->Log(LOGGER_LEVEL_INFO, "COMServer: successfully booted COM configs");
		fclose(fp);
		return;
	} else {
		logger->Log(LOGGER_LEVEL_ERROR, "COMServer: error reading COM config file, cannot boot");
		fclose(fp);
		return;
	}
}

bool COMServer::updateConfig() {
	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));

	FILE * fp = fopen(COM_CFG_UP, "r");
	uint8 buffer[COMConfiguration.size];

	// make sure we get a valid file pointer
	if (fp == NULL) {
		logger->Log(LOGGER_LEVEL_ERROR, "COMServer: NULL COM config file pointer, cannot update");
		return false;
	}

	// read and update the configs
	if (fread(buffer, sizeof(uint8), COMConfiguration.size, fp) == COMConfiguration.size) {
		COMConfiguration.update(buffer, COMConfiguration.size, 0, 0);
		COMConfiguration.deserialize();
		logger->Log(LOGGER_LEVEL_INFO, "COMServer: successfully updated COM configs");
		fclose(fp);
		return true;
	} else {
		logger->Log(LOGGER_LEVEL_ERROR, "COMServer: error reading COM config file, cannot update");
		fclose(fp);
		return false;
	}
}

}
}


