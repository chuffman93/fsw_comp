/*
* ACSServer.cpp
*
*  Created on: Feb 21, 2011
*      Author: Michael
*  Modified by: Conrad Hougen
*  Updated: Alex St. Clair
*/

#include "servers/ACSStdTasks.h"
#include "servers/ACSServer.h"
#include "servers/ACSHandlers.h"
#include "servers/DispatchStdTasks.h"
#include "servers/CDHServer.h"

#include "core/Singleton.h"
#include "core/Factory.h"
#include "core/WatchdogManager.h"
#include "core/StdTypes.h"
#include "core/Dispatcher.h"

#include "util/FileHandler.h"
#include "util/Logger.h"

using namespace std;
using namespace AllStar::Core;

namespace AllStar{
namespace Servers{

// -------------------------------------- Necessary Methods --------------------------------------
ACSServer::ACSServer(string nameIn, LocationIDType idIn) :
		SubsystemServer(nameIn, idIn, ACS_SLEEP_TIME, ACS_HS_DELAYS), Singleton(), arby(idIn), ACSOrientation(ACS_UNORIENTED), testsRun(false){
	ACSState = {0,0,0,0,0,0,0,7};
}

ACSServer::~ACSServer() {
}

ACSServer & ACSServer::operator=(const ACSServer & source){
	if (this == &source)
	{
		return *this;
	}

	SubsystemServer::operator =(source);
	return *this;
}

void ACSServer::Initialize(void){

}

#ifdef TEST
void ACSServer::Destroy(void){

}
#endif

bool ACSServer::IsFullyInitialized(void){
	return Singleton::IsFullyInitialized();
}

bool ACSServer::RegisterHandlers(){
	bool success = true;
	return success;
}

// -------------------------------------------- State Machine --------------------------------------------
void ACSServer::loopInit(){
	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
	CDHServer * cdhServer = dynamic_cast<CDHServer *> (Factory::GetInstance(CDH_SERVER_SINGLETON));
	cdhServer->subPowerOn(HARDWARE_LOCATION_ACS);

	// delay while ACS boots up
	usleep(1000000);

	if(ACSTestAlive()){
		if(!ACSSelfCheck()){
			logger->Log(LOGGER_LEVEL_FATAL, "ACS failed self check!");
		}

		logger->Log(LOGGER_LEVEL_INFO, "ACS passed self check");

		CheckHealthStatus();

		currentState = ST_SUN_SOAK;
	}else{
		logger->Log(LOGGER_LEVEL_FATAL, "ACS non-responsive in init");
	}
}

void ACSServer::loopSunSoak(){
	ModeManager * modeManager = dynamic_cast<ModeManager*>(Factory::GetInstance(MODE_MANAGER_SINGLETON));
	CDHServer * cdhServer = dynamic_cast<CDHServer *> (Factory::GetInstance(CDH_SERVER_SINGLETON));

	if(ACSOrientation != ACS_SUN_ORIENTED){
		if(ACSPointSun()){
			ACSOrientation = ACS_SUN_ORIENTED;
		}
	}

	// check for state transitions from mode switches
	SystemModeEnum currentMode = modeManager->GetMode();
	switch(currentMode){
	case MODE_COM:
		currentState = ST_COM_START;
		break;
	case MODE_PLD_PRIORITY:
		currentState = ST_PLD_START;
		break;
	case MODE_DIAGNOSTIC:
		currentState = ST_DIAGNOSTIC;
		break;
	case MODE_RESET:
		currentState = ST_RESET;
		break;
	default:
		break;
	}

	// if ACS is powered off due to a fault, switch to the init state
	if(!cdhServer->subsystemPowerStates[HARDWARE_LOCATION_ACS])
		currentState = ST_INIT;
}

void ACSServer::loopPLDStart(){
	if(ACSPointNadir()){
		currentState = ST_PLD_POINTING;
		ACSOrientation = ACS_NADIR_ORIENTED;
		sleepTime = 1000;
		hsDelays = 15;
	}else{
		currentState = ST_SUN_SOAK;
	}
}

void ACSServer::loopPLDPointing(){
	ModeManager * modeManager = dynamic_cast<ModeManager*>(Factory::GetInstance(MODE_MANAGER_SINGLETON));
	CDHServer * cdhServer = dynamic_cast<CDHServer *> (Factory::GetInstance(CDH_SERVER_SINGLETON));

	// if ACS is powered off due to a fault, switch to the init state
	if(!cdhServer->subsystemPowerStates[HARDWARE_LOCATION_ACS]){
		currentState = ST_INIT;
	}

	ACSSendGPS();

	if(modeManager->GetMode() != MODE_PLD_PRIORITY)
		currentState = ST_PLD_STOP;
}

void ACSServer::loopPLDStop(){
	sleepTime = 5000;
	hsDelays = 3;
	currentState = ST_SUN_SOAK;
}

void ACSServer::loopCOMStart(){
	if(ACSPointGND()){
		currentState = ST_COM_POINTING;
		ACSOrientation = ACS_GND_ORIENTED;
	}else{
		currentState = ST_SUN_SOAK;
	}
}

void ACSServer::loopCOMPointing(){
	ModeManager * modeManager = dynamic_cast<ModeManager*>(Factory::GetInstance(MODE_MANAGER_SINGLETON));
	CDHServer * cdhServer = dynamic_cast<CDHServer *> (Factory::GetInstance(CDH_SERVER_SINGLETON));

	// if ACS is powered off due to a fault, switch to the init state
	if(!cdhServer->subsystemPowerStates[HARDWARE_LOCATION_ACS]){
		currentState = ST_INIT;
	}

	ACSSendGPS();

	if(modeManager->GetMode() != MODE_COM)
		currentState = ST_COM_STOP;
}

void ACSServer::loopCOMStop(){
	currentState = ST_SUN_SOAK;
}

void ACSServer::loopDiagnostic(){
	ModeManager * modeManager = dynamic_cast<ModeManager *> (Factory::GetInstance(MODE_MANAGER_SINGLETON));

	if(!testsRun){
		ACSTestDriver(1,0.04,0.5);
		usleep(8000000);
		ACSTestDriver(1,0.0,0.0);
		testsRun = true;
	}

	if(modeManager->GetMode() != MODE_DIAGNOSTIC){
		currentState = ST_SUN_SOAK;
	}
}

void ACSServer::loopReset(){
	ACSPrepReset();

	for(uint8 i = 0; i < 60; i++){
		usleep(1000000);
	}

	currentState = ST_SUN_SOAK;
}

// -------------------------------------------- ACS Methods --------------------------------------------
void ACSServer::CheckHealthStatus(){
	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));

	ACPPacket * HSQuery = new ACPPacket(SERVER_LOCATION_ACS, HARDWARE_LOCATION_ACS, HEALTH_STATUS_CMD);
	ACPPacket * HSRet = DispatchPacket(HSQuery);

	if(HSRet == NULL){
		logger->Log(LOGGER_LEVEL_ERROR, "ACSServer: NULL HSRet");
		return;
	}

	if(HSRet->getLength() != ACSState.numItems*sizeof(uint32)){
		logger->Log(LOGGER_LEVEL_WARN, "ACSServer: CheckHealthStatus(): incorrect message length! %u", HSRet->getLength());

		//TODO: return error?
		return;
	}else{
		logger->Log(LOGGER_LEVEL_INFO, "ACSServer: CheckHealthStatus(): packet dispatched, HSRet acquired");
		// Parse buffer
		uint8 * msgPtr = HSRet->getMessageBuff();
		if(msgPtr==NULL){
			//Error
			return;
		}

		uint32 outputArray[ACSState.numItems];
		for(uint8 i = 0; i < ACSState.numItems; i++){
			outputArray[i] = GetUInt32(msgPtr);
			msgPtr += 2;
		}

		ACSState.MRP_X			= outputArray[0];
		ACSState.MRP_Y			= outputArray[1];
		ACSState.MRP_Z			= outputArray[2];
		ACSState.ST_Status		= outputArray[3];
		ACSState.RW_Speed_X		= outputArray[4];
		ACSState.RW_Speed_Y		= outputArray[5];
		ACSState.RW_Speed_Z		= outputArray[6];

		logger->Log(LOGGER_LEVEL_DEBUG, "ACS H&S: MRP X:       %lx", ACSState.MRP_X);
		logger->Log(LOGGER_LEVEL_DEBUG, "ACS H&S: MRP Y:       %lx", ACSState.MRP_Y);
		logger->Log(LOGGER_LEVEL_DEBUG, "ACS H&S: MRP Z:       %lx", ACSState.MRP_Z);
		logger->Log(LOGGER_LEVEL_DEBUG, "ACS H&S: ST Status:   %lx", ACSState.ST_Status);
		logger->Log(LOGGER_LEVEL_DEBUG, "ACS H&S: RW Speed X:  %u", ACSState.RW_Speed_X);
		logger->Log(LOGGER_LEVEL_DEBUG, "ACS H&S: RW Speed Y:  %u", ACSState.RW_Speed_Y);
		logger->Log(LOGGER_LEVEL_DEBUG, "ACS H&S: RW Speed Z:  %u", ACSState.RW_Speed_Z);

	}
}

} // End Namespace servers
} // End Namespace AllStar
