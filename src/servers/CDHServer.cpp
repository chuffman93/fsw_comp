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
#include "HAL/I2C/HotSwaps.h"
#include "HAL/I2C/PowerMonitor.h"
#include "core/Singleton.h"
#include "core/Factory.h"
#include "core/StdTypes.h"
#include "core/WatchdogManager.h"
#include "core/ModeManager.h"
#include "util/Logger.h"
#include "util/FileHandler.h"
#include "util/StorageManager.h"
#include <iostream>
#include <sys/sysinfo.h>

using namespace AllStar::Core;
using namespace AllStar::HAL;
using namespace std;

#define SYS_EN 	1
#define TEMP_EN 0
#define HS_EN	1
#define PM_EN	0

namespace AllStar{
namespace Servers{

CDHConfig CDHServer::CDHConfiguration(0);


// -------------------------------------- Necessary Methods --------------------------------------
CDHServer::CDHServer(std::string nameIn, LocationIDType idIn) :
		SubsystemServer(nameIn, idIn), Singleton(), readHealthFrequency(10) {
	devMngr = new I2CDeviceManager();

	// all subsystems are off on startup except for EPS
	for (int i = HARDWARE_LOCATION_MIN; i < HARDWARE_LOCATION_MAX; i++) {
		subsystemPowerStates[i] = false;
	}
	subsystemPowerStates[HARDWARE_LOCATION_EPS] = true;
}

CDHServer::~CDHServer(){
	delete devMngr;
}

CDHServer & CDHServer::operator=(const CDHServer & source){
	if (this == &source){
		return *this;
	}

	SubsystemServer::operator =(source);

	return *this;
}

void CDHServer::Initialize(void){

}

#ifdef TEST
void CDHServer::Destroy(void){

}
#endif

bool CDHServer::IsFullyInitialized(void){
	return(Singleton::IsFullyInitialized());
}

// -------------------------------------------- Loops ---------------------------------------------
void CDHServer::loopInit(){
	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));

	logger->Log(LOGGER_LEVEL_INFO, "CDHServer: Initializing");

#if HS_EN
	bool initHS = devMngr->initializeHS();
	if(!initHS){
		logger->Log(LOGGER_LEVEL_ERROR, "CDHServer: Error initializing hot swaps!");
	}
#endif

#if PM_EN
	devMngr->initializePM();
#endif //PM_EN

	bootConfig();
	prepPowerGPIOs(); // allows CDH to control subsystem power
	currentState = ST_MONITOR;
}

void CDHServer::loopMonitor(){
	int64 currTime = getTimeInMillis();
	ModeManager * modeManager = dynamic_cast<ModeManager *> (Factory::GetInstance(MODE_MANAGER_SINGLETON));

	readHealth(readHealthFrequency, (uint32) currTime/1000);

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

void CDHServer::loopReset(){
	// nothing to do
	for(uint8 i = 0; i < 60; i++){
		usleep(1000000);
	}

	currentState = ST_MONITOR;
}

// ----------------------------------------- CDH Methods -----------------------------------------
void CDHServer::readHealth(uint8 frequency, uint32 timeUnit){
	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));

	// Start sensors for reading next round
#if PM_EN
	if((timeUnit % 10) == 0){
		devMngr->startPMMeas(); // start power monitor measurements
	}
#endif //PM_EN

#if TEMP_EN
	if(((timeUnit - frequency - 2) % frequency) == 0){
		CDHTempStart();
	}
#endif //TEMP_EN

	// Get all CDH information
	if(((timeUnit - frequency - 1) % frequency) == 0){
		logger->Log(LOGGER_LEVEL_DEBUG, "CDHServer: Gathering information");

#if SYS_EN
		CDHSystemInfo();
#endif

#if TEMP_EN
		CDHTempRead(temperatures); // Read Temp sensors
#endif //TEMP_EN

#if HS_EN
		devMngr->getHSStatus(CDH_hotswaps.hs_buffer); // Read Hot swaps
#endif //HS_EN

#if PM_EN
		devMngr->getPMStatus(CDH_powermonitors); // Read Power Monitors
#endif //PM_EN
	}
}

void CDHServer::subPowerOn(HardwareLocationIDType subsystem){
	toggleSubPower(subsystem, true);
	subsystemPowerStates[subsystem] = true;
}

void CDHServer::subPowerOff(HardwareLocationIDType subsystem){
	toggleSubPower(subsystem, false);
	subsystemPowerStates[subsystem] = false;
}

void CDHServer::resetAssert(HardwareLocationIDType subsystem){
	toggleResetLine(subsystem, false);
}

void CDHServer::resetDeassert(HardwareLocationIDType subsystem){
	toggleResetLine(subsystem, true);
}

void CDHServer::bootConfig() {
	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));

	FILE * fp = fopen(CDH_CONFIG, "r");
	uint8 buffer[CDHConfiguration.size];

	// make sure we get a valid file pointer
	if (fp == NULL) {
		logger->Log(LOGGER_LEVEL_ERROR, "CDHServer: NULL CDH config file pointer, cannot boot");
		return;
	}

	// read and update the configs
	if (fread(buffer, sizeof(uint8), CDHConfiguration.size, fp) == CDHConfiguration.size) {
		CDHConfiguration.update(buffer, CDHConfiguration.size, 0, 0);
		CDHConfiguration.deserialize();
		logger->Log(LOGGER_LEVEL_INFO, "CDHServer: successfully booted CDH configs");
		fclose(fp);
		return;
	} else {
		logger->Log(LOGGER_LEVEL_ERROR, "CDHServer: error reading CDH config file, cannot boot");
		fclose(fp);
		return;
	}
}

bool CDHServer::updateConfig() {
	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));

	FILE * fp = fopen(CDH_CFG_UP, "r");
	uint8 buffer[CDHConfiguration.size];

	// make sure we get a valid file pointer
	if (fp == NULL) {
		logger->Log(LOGGER_LEVEL_ERROR, "CDHServer: NULL CDH config file pointer, cannot update");
		return false;
	}

	// read and update the configs
	if (fread(buffer, sizeof(uint8), CDHConfiguration.size, fp) == CDHConfiguration.size) {
		CDHConfiguration.update(buffer, CDHConfiguration.size, 0, 0);
		CDHConfiguration.deserialize();
		logger->Log(LOGGER_LEVEL_INFO, "CDHServer: successfully updated CDH configs");
		fclose(fp);
		return true;
	} else {
		logger->Log(LOGGER_LEVEL_ERROR, "CDHServer: error reading CDH config file, cannot update");
		fclose(fp);
		return false;
	}
}

}
}
