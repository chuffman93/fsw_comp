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
#include "servers/EPSServer.h"
#include "servers/EPSStdTasks.h"
#include "servers/ERRServer.h"
#include "servers/FMGServer.h"
#include "servers/DispatchStdTasks.h"
#include "servers/CDHServer.h"
#include "servers/Structs.h"
#include "servers/FileSystem.h"
#include "util/Logger.h"
#include "util/TLM.h"

using namespace std;
using namespace AllStar::Core;

namespace AllStar{
namespace Servers{

EPSStatus EPSServer::EPSState(0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0);
EPSConfig EPSServer::EPSConfiguration(0,0,0);

// -------------------------------------- Necessary Methods --------------------------------------
EPSServer::EPSServer(string nameIn, LocationIDType idIn) :
		SubsystemServer(nameIn, idIn), Singleton() {
}

EPSServer & EPSServer::operator=(const EPSServer & source){
	if(this == &source){
		return *this;
	}

	SubsystemServer::operator =(source);

	return *this;
}

bool EPSServer::IsFullyInitialized(void){
	return(Singleton::IsFullyInitialized());
}

// -------------------------------------------- State Machine ---------------------------------------------
void EPSServer::loopInit() {
	ERRServer * errServer = static_cast<ERRServer *> (Factory::GetInstance(ERR_SERVER_SINGLETON));
	Logger * logger = static_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));

	TLM_EPS_SERVER_STARTED();

	if(EPSTestAlive()){
		if(!EPSSelfCheck()){
			logger->Fatal("EPS failed self check!");
			errServer->SendError(ERR_EPS_SELFCHECK);
		}
		logger->Info("EPS passed self check");

		bootConfig();

		currentState = ST_MONITOR;
	}else{
		logger->Fatal("EPS non-responsive in init loop");
		errServer->SendError(ERR_EPS_NOTALIVE);
		wdmAsleep();
		usleep(3000000);
		wdmAlive();
	}
}

void EPSServer::loopMonitor(){
	ModeManager * modeManager = static_cast<ModeManager *> (Factory::GetInstance(MODE_MANAGER_SINGLETON));
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
	FMGServer * fmgServer = static_cast<FMGServer *> (Factory::GetInstance(FMG_SERVER_SINGLETON));
	Logger * logger = static_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));

	logger->Info("EPS ready for reset");

	sleep(30);

	for(uint8 i = 0; i < 30; i++){
		if(fmgServer->isResetReady()){
			EPSPowerCycle();
		}
		usleep(1000000);
	}

	currentState = ST_MONITOR;
}

// -------------------------------------------- EPS Methods ---------------------------------------------
bool EPSServer::CheckHealthStatus(){
	Logger * logger = static_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));

	ACPPacket * HSQuery = new ACPPacket(SERVER_LOCATION_EPS, HARDWARE_LOCATION_EPS, HEALTH_STATUS_CMD);
	ACPPacket * HSRet = DispatchPacket(HSQuery);

	if(HSRet == NULL){
		logger->Error("EPSServer: NULL HSRet");
		return false;
	}

	if(HSRet->getLength() != EPSStatus::size) {
		logger->Warning("EPSServer: CheckHealthStatus(): incorrect message length!");

		//TODO: return error?
		return false;
	}else{
		logger->Info("EPSServer: CheckHealthStatus(): packet dispatched, HSRet acquired");

		// Parse buffer
		uint8 * msgPtr = HSRet->getMessageBuff();
		if(msgPtr==NULL){
			logger->Error("EPSServer: CheckHealthStatus(): NULL msgPtr");
			return false;
		}

		// deserialize the message
		EPSState.update(msgPtr, EPSStatus::size);
		EPSState.deserialize();

		logger->Debug("EPS H&S: 3v3 Current:     %u", EPSState.current3v3);
		logger->Debug("EPS H&S: 3v3 Voltage:     %u", EPSState.voltage3v3);
		logger->Debug("EPS H&S: Vbat Current:    %u", EPSState.currentVbat);
		logger->Debug("EPS H&S: Vbat Voltage:    %u", EPSState.voltageVbat);
		logger->Debug("EPS H&S: 12v Current:     %u", EPSState.current12v);
		logger->Debug("EPS H&S: 12v Voltage:     %u", EPSState.voltage12v);
		logger->Info( "EPS H&S: Rem cap:         %u", EPSState.remainingCapacity);
		logger->Debug("EPS H&S: Batt curr:       %u", EPSState.battCurrent);
		logger->Debug("EPS H&S: Batt volt:       %u", EPSState.battVoltage);
		logger->Debug("EPS H&S: Batt status:     %u", EPSState.battStatus);
		logger->Debug("EPS H&S: Frang curr:      %u", EPSState.frangCurrent);
		logger->Debug("EPS H&S: Frang volt:      %u", EPSState.frangVoltage);
		logger->Debug("EPS H&S: Conv curr x:     %u", EPSState.convCurrentX);
		logger->Debug("EPS H&S: Conv thresh x:   %u", EPSState.convThreshX);
		logger->Debug("EPS H&S: Conv curr y:     %u", EPSState.convCurrentY);
		logger->Debug("EPS H&S: Conv thresh y:   %u", EPSState.convThreshY);
		logger->Debug("EPS H&S: Conv curr w:     %u", EPSState.convCurrentW);
		logger->Debug("EPS H&S: Conv thresh w:   %u", EPSState.convThreshW);

		int32 currTime = getTimeInSec();
		if (currTime >= (lastHSTLog + 60)) {
			lastHSTLog = currTime;

			// add the current spacecraft time to the log buffer
			uint8 * buffer = new uint8[EPSStatus::size + sizeof(int32)];
			AddUInt32(buffer, currTime);

			// add the EPS H&S to the buffer
			EPSState.update(buffer, EPSStatus::size, 4, 0);
			EPSState.serialize();

			FMGServer * fmgServer = static_cast<FMGServer *> (Factory::GetInstance(FMG_SERVER_SINGLETON));
			fmgServer->Log(DESTINATION_EPS_HST, buffer, EPSStatus::size + sizeof(int32));
		}

		return true;
	}
}

void EPSServer::bootConfig() {
	Logger * logger = static_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));

	FILE * fp = fopen(EPS_CONFIG, "r");
	uint8 buffer[EPSConfiguration.size];

	// make sure we get a valid file pointer
	if (fp == NULL) {
		logger->Error("EPSServer: NULL EPS config file pointer, cannot boot");
		return;
	}

	// read and update the configs
	if (fread(buffer, sizeof(uint8), EPSConfiguration.size, fp) == EPSConfiguration.size) {
		EPSConfiguration.update(buffer, EPSConfiguration.size, 0, 0);
		EPSConfiguration.deserialize();
		logger->Info("EPSServer: successfully booted EPS configs");
		fclose(fp);
		return;
	} else {
		logger->Error("EPSServer: error reading EPS config file, cannot boot");
		fclose(fp);
		return;
	}
}

bool EPSServer::updateConfig() {
	Logger * logger = static_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));

	FILE * fp = fopen(EPS_CFG_UP, "r");
	uint8 buffer[EPSConfiguration.size];

	// make sure we get a valid file pointer
	if (fp == NULL) {
		logger->Error("EPSServer: NULL EPS config file pointer, cannot update");
		return false;
	}

	// read and update the configs
	if (fread(buffer, sizeof(uint8), EPSConfiguration.size, fp) == EPSConfiguration.size) {
		EPSConfiguration.update(buffer, EPSConfiguration.size, 0, 0);
		EPSConfiguration.deserialize();
		logger->Info("EPSServer: successfully updated EPS configs");
		fclose(fp);
		return true;
	} else {
		logger->Error("EPSServer: error reading EPS config file, cannot update");
		fclose(fp);
		return false;
	}
}

} // End Namespace servers
} // End Namespace AllStar
