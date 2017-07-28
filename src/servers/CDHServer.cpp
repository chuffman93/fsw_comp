/*
 * CDHServer.cpp
 *
 *  Created on: Mar 4, 2016
 *      Author: Alex St. Clair
 */

#include "servers/CDHServer.h"
#include "servers/CDHStdTasks.h"
#include "servers/DispatchStdTasks.h"
#include "servers/FileSystem.h"
#include "servers/FMGServer.h"
#include "HAL/I2C/HotSwaps.h"
#include "HAL/I2C/PowerMonitor.h"
#include "core/Singleton.h"
#include "core/Factory.h"
#include "core/StdTypes.h"
#include "core/WatchdogManager.h"
#include "core/ModeManager.h"
#include "util/Logger.h"
#include "util/TLM.h"
#include <iostream>
#include <sys/sysinfo.h>

using namespace AllStar::Core;
using namespace AllStar::HAL;
using namespace std;

#define SYS_EN 	1
#define TEMP_EN 1
#define HS_EN	1
#define PM_EN	0

namespace AllStar {
namespace Servers {

CDHConfig CDHServer::CDHConfiguration(0);

// -------------------------------------- Necessary Methods --------------------------------------
CDHServer::CDHServer(std::string nameIn, LocationIDType idIn) :
		SubsystemServer(nameIn, idIn), Singleton(), healthCount(0), lastHSTLog(0) {
	devMngr = new I2CDeviceManager();

	// all subsystems are off on startup except for EPS
	for (int i = HARDWARE_LOCATION_MIN; i < HARDWARE_LOCATION_MAX; i++) {
		subsystemPowerStates[i] = false;
	}
	subsystemPowerStates[HARDWARE_LOCATION_EPS] = true;
}

CDHServer::~CDHServer() {
	delete devMngr;
}

CDHServer & CDHServer::operator=(const CDHServer & source) {
	if (this == &source) {
		return *this;
	}

	SubsystemServer::operator =(source);

	return *this;
}

bool CDHServer::IsFullyInitialized(void) {
	return(Singleton::IsFullyInitialized());
}

// -------------------------------------------- Loops ---------------------------------------------
void CDHServer::loopInit() {
	Logger * logger = static_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));

	logger->Info("CDHServer: Initializing");
	TLM_CDH_SERVER_STARTED();

#if HS_EN
	bool initHS = devMngr->initializeHS();
	if (!initHS) {
		logger->Error("CDHServer: Error initializing hot swaps!");
	}
#endif

#if PM_EN
	devMngr->initializePM();
#endif //PM_EN

	bootConfig();
	prepPowerGPIOs(); // allows CDH to control subsystem power
	currentState = ST_MONITOR;
}

void CDHServer::loopMonitor() {
	ModeManager * modeManager = static_cast<ModeManager *> (Factory::GetInstance(MODE_MANAGER_SINGLETON));

	readHealth();

	// Check for state transitions based on mode switches
	SystemModeEnum currentMode = modeManager->GetMode();
	switch(currentMode){
	case MODE_RESET:
		currentState = ST_RESET;
		break;
	default:
		break;
	}
}

void CDHServer::loopReset() {
	// nothing to do
	for (uint8 i = 0; i < 60; i++) {
		usleep(1000000);
	}

	currentState = ST_MONITOR;
}

// ----------------------------------------- CDH Methods -----------------------------------------
void CDHServer::readHealth() {
	Logger * logger = static_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));

	// Start sensors for reading next round
#if PM_EN
	if(healthCount == 0){
		devMngr->startPMMeas(); // start power monitor measurements
	}
#endif //PM_EN

#if TEMP_EN
	if (healthCount == 8) {
		CDHTempStart();
	}
#endif //TEMP_EN

	// Get all CDH information
	if (healthCount == 9) {
		logger->Debug("CDHServer: Gathering information");
		CDHState.time  = getTimeInSec();

#if SYS_EN
		CDHSystemInfo();
#endif

#if TEMP_EN
		CDHTempRead(THMState.tempSensors); // Read Temp sensors
		THMState.time = getTimeInSec();
#endif //TEMP_EN

#if HS_EN
		devMngr->getHSStatus(SWPState.hotswaps); // Read Hot swaps
		SWPState.time = getTimeInSec();
#endif //HS_EN

#if PM_EN
		devMngr->getPMStatus(CDH_powermonitors); // Read Power Monitors
#endif //PM_EN

		int32 currTime = getTimeInSec();
		if (currTime >= (lastHSTLog + 60)) {
			logger->Info("CDHServer: logging health and status");
			lastHSTLog = currTime;

			FMGServer * fmgServer = static_cast<FMGServer *> (Factory::GetInstance(FMG_SERVER_SINGLETON));
			uint8 * buffer = new uint8[CDHStatus::size];
			CDHState.update(buffer, CDHStatus::size);
			CDHState.serialize();
			fmgServer->Log(DESTINATION_CDH_HST, buffer, CDHStatus::size);

			buffer = new uint8[THMStatus::size];
			THMState.update(buffer, THMStatus::size);
			THMState.serialize();
			fmgServer->Log(DESTINATION_THM, buffer, THMStatus::size);

			buffer = new uint8[SWPStatus::size];
			SWPState.update(buffer, SWPStatus::size);
			SWPState.serialize();
			fmgServer->Log(DESTINATION_SWP, buffer, SWPStatus::size);
		}
	}

	healthCount = (healthCount == 9) ? 0 : healthCount + 1;
}

void CDHServer::subPowerOn(HardwareLocationIDType subsystem) {
	toggleSubPower(subsystem, true);
	subsystemPowerStates[subsystem] = true;
}

void CDHServer::subPowerOff(HardwareLocationIDType subsystem) {
	toggleSubPower(subsystem, false);
	subsystemPowerStates[subsystem] = false;
}

void CDHServer::resetAssert(HardwareLocationIDType subsystem) {
	toggleResetLine(subsystem, false);
}

void CDHServer::resetDeassert(HardwareLocationIDType subsystem) {
	toggleResetLine(subsystem, true);
}

void CDHServer::bootConfig() {
	Logger * logger = static_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));

	FILE * fp = fopen(CDH_CONFIG, "r");
	uint8 buffer[CDHConfiguration.size];

	// make sure we get a valid file pointer
	if (fp == NULL) {
		logger->Error("CDHServer: NULL CDH config file pointer, cannot boot");
		return;
	}

	// read and update the configs
	if (fread(buffer, sizeof(uint8), CDHConfiguration.size, fp) == CDHConfiguration.size) {
		CDHConfiguration.update(buffer, CDHConfiguration.size, 0, 0);
		CDHConfiguration.deserialize();
		logger->Info("CDHServer: successfully booted CDH configs");
		fclose(fp);
		return;
	} else {
		logger->Error("CDHServer: error reading CDH config file, cannot boot");
		fclose(fp);
		return;
	}
}

bool CDHServer::updateConfig() {
	Logger * logger = static_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));

	FILE * fp = fopen(CDH_CFG_UP, "r");
	uint8 buffer[CDHConfiguration.size];

	// make sure we get a valid file pointer
	if (fp == NULL) {
		logger->Error("CDHServer: NULL CDH config file pointer, cannot update");
		return false;
	}

	// read and update the configs
	if (fread(buffer, sizeof(uint8), CDHConfiguration.size, fp) == CDHConfiguration.size) {
		CDHConfiguration.update(buffer, CDHConfiguration.size, 0, 0);
		CDHConfiguration.deserialize();
		logger->Info("CDHServer: successfully updated CDH configs");
		fclose(fp);
		return true;
	} else {
		logger->Error("CDHServer: error reading CDH config file, cannot update");
		fclose(fp);
		return false;
	}
}

}
}
