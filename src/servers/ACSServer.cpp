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
#include "servers/DispatchStdTasks.h"
#include "servers/CDHServer.h"
#include "servers/ERRServer.h"
#include "servers/FileSystem.h"
#include "core/Singleton.h"
#include "core/Factory.h"
#include "core/WatchdogManager.h"
#include "core/StdTypes.h"
#include "core/Dispatcher.h"
#include "util/Logger.h"

using namespace std;
using namespace AllStar::Core;

namespace AllStar{
namespace Servers{

ACSConfig ACSServer::ACSConfiguration(0);

// -------------------------------------- Necessary Methods --------------------------------------
ACSServer::ACSServer(string nameIn, LocationIDType idIn) :
		SubsystemServer(nameIn, idIn, ACS_SLEEP_TIME, ACS_HS_DELAYS), Singleton(), ACSOrientation(ACS_UNORIENTED), testsRun(false),
		ACSState(0,0,0,0,0,0,0) {
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

bool ACSServer::IsFullyInitialized(void){
	return Singleton::IsFullyInitialized();
}

// -------------------------------------------- State Machine --------------------------------------------
void ACSServer::loopInit() {
	ERRServer * errServer = dynamic_cast<ERRServer *> (Factory::GetInstance(ERR_SERVER_SINGLETON));
	CDHServer * cdhServer = dynamic_cast<CDHServer *> (Factory::GetInstance(CDH_SERVER_SINGLETON));
	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));

	if (!cdhServer->subsystemPowerStates[HARDWARE_LOCATION_ACS]) {
		cdhServer->resetAssert(HARDWARE_LOCATION_ACS);
		cdhServer->subPowerOn(HARDWARE_LOCATION_ACS);
		usleep(100000);
		cdhServer->resetDeassert(HARDWARE_LOCATION_ACS);

		// delay while ACS boots up
		wdmAsleep();
		usleep(4000000);
		wdmAlive();
	}

	if (ACSTestAlive()) {
		if (!ACSSelfCheck()) {
			errServer->SendError(ERR_ACS_SELFCHECK);
			wdmAsleep();
			usleep(3000000);
			wdmAlive();
			return;
		}

		logger->Log(LOGGER_LEVEL_INFO, "ACS passed self check");

		bootConfig();

		currentState = ST_SUN_SOAK;
	} else {
		errServer->SendError(ERR_ACS_NOTALIVE);
		wdmAsleep();
		usleep(3000000);
		wdmAlive();
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

void ACSServer::loopReset(){
	ACSPrepReset();

	for(uint8 i = 0; i < 60; i++){
		usleep(1000000);
	}

	currentState = ST_SUN_SOAK;
}

// -------------------------------------------- ACS Methods --------------------------------------------
bool ACSServer::CheckHealthStatus(){
	if (currentState == ST_INIT) {
		return false;
	}

	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));

	ACPPacket * HSQuery = new ACPPacket(SERVER_LOCATION_ACS, HARDWARE_LOCATION_ACS, HEALTH_STATUS_CMD);
	ACPPacket * HSRet = DispatchPacket(HSQuery);

	if(HSRet == NULL){
		logger->Log(LOGGER_LEVEL_ERROR, "ACSServer: NULL HSRet");
		return false;
	}

	if(HSRet->getLength() != ACSState.size){
		logger->Log(LOGGER_LEVEL_WARN, "ACSServer: CheckHealthStatus(): incorrect message length! %u", HSRet->getLength());

		//TODO: return error?
		return false;
	}else{
		logger->Log(LOGGER_LEVEL_INFO, "ACSServer: CheckHealthStatus(): packet dispatched, HSRet acquired");

		// Parse buffer
		uint8 * msgPtr = HSRet->getMessageBuff();
		if(msgPtr==NULL){
			logger->Log(LOGGER_LEVEL_ERROR, "ACSServer: CheckHealthStatus(): NULL msgPtr");
			return false;
		}

		// deserialize the buffer
		ACSState.update(msgPtr,ACSState.size,0,0);
		ACSState.deserialize();

		logger->Log(LOGGER_LEVEL_DEBUG, "ACS H&S: MRP X:       %lx", ACSState.MRP_X);
		logger->Log(LOGGER_LEVEL_DEBUG, "ACS H&S: MRP Y:       %lx", ACSState.MRP_Y);
		logger->Log(LOGGER_LEVEL_DEBUG, "ACS H&S: MRP Z:       %lx", ACSState.MRP_Z);
		logger->Log(LOGGER_LEVEL_DEBUG, "ACS H&S: ST Status:   %lx", ACSState.ST_Status);
		logger->Log(LOGGER_LEVEL_DEBUG, "ACS H&S: RW Speed X:  %u", ACSState.RW_Speed_X);
		logger->Log(LOGGER_LEVEL_DEBUG, "ACS H&S: RW Speed Y:  %u", ACSState.RW_Speed_Y);
		logger->Log(LOGGER_LEVEL_DEBUG, "ACS H&S: RW Speed Z:  %u", ACSState.RW_Speed_Z);

		return true;
	}
}

void ACSServer::bootConfig() {
	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));

	FILE * fp = fopen(ACS_CONFIG, "r");
	uint8 buffer[ACSConfiguration.size];

	// make sure we get a valid file pointer
	if (fp == NULL) {
		logger->Log(LOGGER_LEVEL_ERROR, "ACSServer: NULL ACS config file pointer, cannot boot");
		return;
	}

	// read and update the configs
	if (fread(buffer, sizeof(uint8), ACSConfiguration.size, fp) == ACSConfiguration.size) {
		ACSConfiguration.update(buffer, ACSConfiguration.size, 0, 0);
		ACSConfiguration.deserialize();
		logger->Log(LOGGER_LEVEL_INFO, "ACSServer: successfully booted ACS configs");
		fclose(fp);
		return;
	} else {
		logger->Log(LOGGER_LEVEL_ERROR, "ACSServer: error reading ACS config file, cannot boot");
		fclose(fp);
		return;
	}
}

bool ACSServer::updateConfig() {
	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));

	FILE * fp = fopen(ACS_CFG_UP, "r");
	uint8 buffer[ACSConfiguration.size];

	// make sure we get a valid file pointer
	if (fp == NULL) {
		logger->Log(LOGGER_LEVEL_ERROR, "ACSServer: NULL ACS config file pointer, cannot update");
		return false;
	}

	// read and update the configs
	if (fread(buffer, sizeof(uint8), ACSConfiguration.size, fp) == ACSConfiguration.size) {
		ACSConfiguration.update(buffer, ACSConfiguration.size, 0, 0);
		ACSConfiguration.deserialize();
		logger->Log(LOGGER_LEVEL_INFO, "ACSServer: successfully updated ACS configs");
		fclose(fp);
		return true;
	} else {
		logger->Log(LOGGER_LEVEL_ERROR, "ACSServer: error reading ACS config file, cannot update");
		fclose(fp);
		return false;
	}
}

} // End Namespace servers
} // End Namespace AllStar
