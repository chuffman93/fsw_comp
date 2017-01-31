/*
 * EPSServer.cpp
 *
 *  Created on: Feb 21, 2011
 *      Author: Michael
 *  Modified by: Conrad Hougen
 *  Modified by: Alex St. Clair
 */

#include "core/Singleton.h"
#include "core/Factory.h"
#include "core/WatchdogManager.h"
#include "core/StdTypes.h"
#include "servers/EPSHandlers.h"
#include "servers/EPSServer.h"
#include "servers/EPSStdTasks.h"
#include "servers/FMGServer.h"
#include "servers/DispatchStdTasks.h"
#include "servers/CDHServer.h"
#include "util/Logger.h"

using namespace std;
using namespace AllStar::Core;

namespace AllStar{
namespace Servers{

// Instantiate static message handlers
static EPSHSHandler * epsHSHandler;
static EPSPowerCycleHandler * epsPowerCycleHandler;

// -------------------------------------- Necessary Methods --------------------------------------
EPSServer::EPSServer(string nameIn, LocationIDType idIn) :
		SubsystemServer(nameIn, idIn), Singleton(), arby(idIn) {
	EPSConfiguration.numItems = 3;
}

EPSServer & EPSServer::operator=(const EPSServer & source){
	if(this == &source){
		return *this;
	}

	SubsystemServer::operator =(source);

	return *this;
}

void EPSServer::Initialize(void){
	//Initialize handlers
//	epsHSHandler = new EPSHSHandler();
//	epsPowerCycleHandler = new EPSPowerCycleHandler();
}

#ifdef TEST
void EPSServer::Destroy(void){
	//delete handlers
	delete epsHSHandler;
	delete epsPowerCycleHandler;
}
#endif

bool EPSServer::IsFullyInitialized(void){
	return(Singleton::IsFullyInitialized());
}

bool EPSServer::RegisterHandlers() {
	bool success = true;

	Dispatcher * dispatcher = dynamic_cast<Dispatcher *>(Factory::GetInstance(DISPATCHER_SINGLETON));

	success &= reg.RegisterHandler(MessageIdentifierType(SERVER_LOCATION_EPS, HEALTH_STATUS_CMD),epsHSHandler);
	success &= reg.RegisterHandler(MessageIdentifierType(SERVER_LOCATION_EPS, SUBSYSTEM_RESET_CMD),epsPowerCycleHandler);

	success &= dispatcher->AddRegistry(id, &reg, &arby);

	return success;
}

// -------------------------------------------- State Machine ---------------------------------------------
void EPSServer::loopInit(){
	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));

	if(EPSTestAlive()){
		if(!EPSSelfCheck()){
			logger->Log(LOGGER_LEVEL_FATAL, "EPS failed self check!");
		}
		logger->Log(LOGGER_LEVEL_INFO, "EPS passed self check");

		currentState = ST_MONITOR;
	}else{
		logger->Log(LOGGER_LEVEL_FATAL, "EPS non-responsive in init loop");
		usleep(3000000);
	}
}

void EPSServer::loopMonitor(){
	ModeManager * modeManager = dynamic_cast<ModeManager *> (Factory::GetInstance(MODE_MANAGER_SINGLETON));
	SystemModeEnum currentMode = modeManager->GetMode();
	switch(currentMode){
	case MODE_RESET:
		currentState = ST_RESET;
		break;
	default:
		break;
	}
}

void EPSServer::loopReset(){
	FMGServer * fmgServer = dynamic_cast<FMGServer *> (Factory::GetInstance(FMG_SERVER_SINGLETON));
	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));

	logger->Log(LOGGER_LEVEL_INFO, "EPS ready for reset");

	for(uint8 i = 0; i < 60; i++){
		if(fmgServer->isResetReady()){
			EPSPowerCycle();
		}
		usleep(1000000);
	}

	currentState = ST_MONITOR;
}

// -------------------------------------------- EPS Methods ---------------------------------------------
bool EPSServer::CheckHealthStatus(){
	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));

	ACPPacket * HSQuery = new ACPPacket(SERVER_LOCATION_EPS, HARDWARE_LOCATION_EPS, HEALTH_STATUS_CMD);
	ACPPacket * HSRet = DispatchPacket(HSQuery);

	if(HSRet == NULL){
		logger->Log(LOGGER_LEVEL_ERROR, "EPSServer: NULL HSRet");
		return false;
	}

	if(HSRet->getLength() != 18*sizeof(uint16)){
		logger->Log(LOGGER_LEVEL_WARN, "EPSServer: CheckHealthStatus(): incorrect message length!");

		//TODO: return error?
		return false;
	}else{
		logger->Log(LOGGER_LEVEL_INFO, "EPSServer: CheckHealthStatus(): packet dispatched, HSRet acquired");
		// Parse buffer
		uint8 * msgPtr = HSRet->getMessageBuff();
		if(msgPtr==NULL){
			//Error
			return false;
		}

		uint16 outputArray[18];
		for(uint8 i = 0; i < 18; i++){
			outputArray[i] = GetUInt16(msgPtr);
			msgPtr += 2;
		}

		EPSState.current3v3			= outputArray[0];
		EPSState.voltage3v3			= outputArray[1];
		EPSState.currentVbat		= outputArray[2];
		EPSState.voltageVbat		= outputArray[3];
		EPSState.current12v			= outputArray[4];
		EPSState.voltage12v			= outputArray[5];
		EPSState.remainingCapacity	= outputArray[6];
		EPSState.battCurrent		= outputArray[7];
		EPSState.battVoltage		= outputArray[8];
		EPSState.battStatus			= outputArray[9];
		EPSState.frangCurrent		= outputArray[10];
		EPSState.frangVoltage		= outputArray[11];
		EPSState.convCurrentX		= outputArray[12];
		EPSState.convThreshX		= outputArray[13];
		EPSState.convCurrentY		= outputArray[14];
		EPSState.convThreshY		= outputArray[15];
		EPSState.convCurrentW		= outputArray[16];
		EPSState.convThreshW		= outputArray[17];

		logger->Log(LOGGER_LEVEL_DEBUG, "EPS H&S: 3v3 Current:     %u", EPSState.current3v3);
		logger->Log(LOGGER_LEVEL_DEBUG, "EPS H&S: 3v3 Voltage:     %u", EPSState.voltage3v3);
		logger->Log(LOGGER_LEVEL_DEBUG, "EPS H&S: Vbat Current:    %u", EPSState.currentVbat);
		logger->Log(LOGGER_LEVEL_DEBUG, "EPS H&S: Vbat Voltage:    %u", EPSState.voltageVbat);
		logger->Log(LOGGER_LEVEL_DEBUG, "EPS H&S: 12v Current:     %u", EPSState.current12v);
		logger->Log(LOGGER_LEVEL_DEBUG, "EPS H&S: 12v Voltage:     %u", EPSState.voltage12v);
		logger->Log(LOGGER_LEVEL_INFO,  "EPS H&S: Rem cap:         %u", EPSState.remainingCapacity);
		logger->Log(LOGGER_LEVEL_DEBUG, "EPS H&S: Batt curr:       %u", EPSState.battCurrent);
		logger->Log(LOGGER_LEVEL_DEBUG, "EPS H&S: Batt volt:       %u", EPSState.battVoltage);
		logger->Log(LOGGER_LEVEL_DEBUG, "EPS H&S: Batt status:     %u", EPSState.battStatus);
		logger->Log(LOGGER_LEVEL_DEBUG, "EPS H&S: Frang curr:      %u", EPSState.frangCurrent);
		logger->Log(LOGGER_LEVEL_DEBUG, "EPS H&S: Frang volt:      %u", EPSState.frangVoltage);
		logger->Log(LOGGER_LEVEL_DEBUG, "EPS H&S: Conv curr x:     %u", EPSState.convCurrentX);
		logger->Log(LOGGER_LEVEL_DEBUG, "EPS H&S: Conv thresh x:   %u", EPSState.convThreshX);
		logger->Log(LOGGER_LEVEL_DEBUG, "EPS H&S: Conv curr y:     %u", EPSState.convCurrentY);
		logger->Log(LOGGER_LEVEL_DEBUG, "EPS H&S: Conv thresh y:   %u", EPSState.convThreshY);
		logger->Log(LOGGER_LEVEL_DEBUG, "EPS H&S: Conv curr w:     %u", EPSState.convCurrentW);
		logger->Log(LOGGER_LEVEL_DEBUG, "EPS H&S: Conv thresh w:   %u", EPSState.convThreshW);

		return true;
	}
}

} // End Namespace servers
} // End Namespace AllStar
