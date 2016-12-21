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
	EPSState = {0,0,0,0,0,0,0,0};
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
	if(modeManager->GetMode() == MODE_DIAGNOSTIC){
		currentState = ST_DIAGNOSTIC;
	}
}

void EPSServer::loopDiagnostic(){
	ModeManager * modeManager = dynamic_cast<ModeManager *> (Factory::GetInstance(MODE_MANAGER_SINGLETON));
	if(modeManager->GetMode() != MODE_DIAGNOSTIC){
		currentState = ST_MONITOR;
	}
}

// -------------------------------------------- EPS Methods ---------------------------------------------
void EPSServer::CheckHealthStatus(){
	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));

	ACPPacket * HSQuery = new ACPPacket(SERVER_LOCATION_EPS, HARDWARE_LOCATION_EPS, HEALTH_STATUS_CMD);
	ACPPacket * HSRet = DispatchPacket(HSQuery);

	if(HSRet == NULL){
		logger->Log(LOGGER_LEVEL_ERROR, "EPSStdTasks: NULL HSRet");
		return;
	}

	if(HSRet->getLength() != 8*sizeof(uint16)){
		logger->Log(LOGGER_LEVEL_WARN, "EPSStdTasks: EPSHealthStat(): incorrect message length!");

		//TODO: return error?
		return;
	}else{
		logger->Log(LOGGER_LEVEL_INFO, "EPSStdTasks: EPSHealthStat(): packet dispatched, HSRet acquired");
		// Parse buffer
		uint8 * msgPtr = HSRet->getMessageBuff();
		if(msgPtr==NULL){
			//Error
			return;
		}

		uint16 outputArray[8];
		for(uint8 i = 0; i < 8; i++){
			outputArray[i] = GetUInt16(msgPtr);
			msgPtr += 2;
		}

		EPSState.current3v3		= outputArray[0];
		EPSState.voltage3v3		= outputArray[1];
		EPSState.currentBatt	= outputArray[2];
		EPSState.voltageBatt	= outputArray[3];
		EPSState.current12v		= outputArray[4];
		EPSState.voltage12v		= outputArray[5];
		EPSState.battStatus		= outputArray[6];
		EPSState.stateOfCharge	= outputArray[7];

		logger->Log(LOGGER_LEVEL_DEBUG, "EPS H&S: 3v3 Current:     %u", EPSState.current3v3);
		logger->Log(LOGGER_LEVEL_DEBUG, "EPS H&S: 3v3 Voltage:     %u", EPSState.voltage3v3);
		logger->Log(LOGGER_LEVEL_DEBUG, "EPS H&S: Batt Current:    %u", EPSState.currentBatt);
		logger->Log(LOGGER_LEVEL_DEBUG, "EPS H&S: Batt Voltage:    %u", EPSState.voltageBatt);
		logger->Log(LOGGER_LEVEL_DEBUG, "EPS H&S: 12v Current:     %u", EPSState.current12v);
		logger->Log(LOGGER_LEVEL_DEBUG, "EPS H&S: 12v Voltage:     %u", EPSState.voltage12v);
		logger->Log(LOGGER_LEVEL_DEBUG, "EPS H&S: Batt status:     %u", EPSState.battStatus);
		logger->Log(LOGGER_LEVEL_DEBUG, "EPS H&S: State of charge: %u", EPSState.stateOfCharge);

		//PacketProcess(SERVER_LOCATION_EPS, HSRet);
	}
}

} // End Namespace servers
} // End Namespace AllStar
