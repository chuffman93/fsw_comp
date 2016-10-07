/*
 * CDHServer.cpp
 *
 *  Created on: Mar 4, 2016
 *      Author: Alex St. Clair
 */

#include "servers/CDHServer.h"
#include "servers/CDHHandlers.h"
#include "servers/CDHStdTasks.h"
#include "servers/DispatchStdTasks.h"
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

#define CPU_EN 	0
#define MEM_EN 	0
#define TEMP_EN 0
#define HS_EN	0
#define PM_EN	0
#define STOR_EN 0

namespace AllStar{
namespace Servers{

static CDHCPUUsageHandler * cdhCPUUsageHandler;
static CDHMemUsageHandler * cdhMemUsageHandler;
static CDHTempStartHandler * cdhTempStartHandler;
static CDHTempReadHandler * cdhTempReadHandler;
static CDHHotSwapsHandler * cdhHotSwapsHandler;
static CDHPowerMonitorsHandler * cdhPowerMonitorsHandler;
static CDHStartPMHandler * cdhStartPMHandler;
static CDHCleanFSHandler * cdhCleanFSHandler;

// -------------------------------------- Necessary Methods --------------------------------------
CDHServer::CDHServer(std::string nameIn, LocationIDType idIn) :
		SubsystemServer(nameIn, idIn), Singleton(), arby(idIn) {
	devMngr = new I2CDeviceManager();
	storMngr = new StorageManager(0.9);

	// all subsystems are off on startup except for EPS
	for (int i = HARDWARE_LOCATION_MIN; i < HARDWARE_LOCATION_MAX; i++) {
		subsystemPowerStates[i] = false;
	}
	subsystemPowerStates[HARDWARE_LOCATION_EPS] = true;
}

CDHServer::~CDHServer(){
	delete devMngr;
	delete storMngr;
}

CDHServer & CDHServer::operator=(const CDHServer & source){
	if (this == &source){
		return *this;
	}

	SubsystemServer::operator =(source);

	return *this;
}

void CDHServer::Initialize(void){
	cdhCPUUsageHandler = new CDHCPUUsageHandler();
	cdhMemUsageHandler = new CDHMemUsageHandler();
	cdhTempStartHandler = new CDHTempStartHandler();
	cdhTempReadHandler = new CDHTempReadHandler();
	cdhHotSwapsHandler = new CDHHotSwapsHandler();
	cdhPowerMonitorsHandler = new CDHPowerMonitorsHandler();
	cdhStartPMHandler = new CDHStartPMHandler();
	cdhCleanFSHandler = new CDHCleanFSHandler();
}

#ifdef TEST
void CDHServer::Destroy(void){
	delete cdhCPUUsageHandler;
	delete cdhMemUsageHandler;
	delete cdhTempStartHandler;
	delete cdhTempReadHandler;
	delete cdhHotSwapsHandler;
	delete cdhPowerMonitorsHandler;
	delete cdhStartPMHandler;
	delete cdhCleanFSHandler;
}
#endif

bool CDHServer::IsFullyInitialized(void){
	return(Singleton::IsFullyInitialized());
}

bool CDHServer::RegisterHandlers(){
	bool success = true;
	Dispatcher * dispatcher = dynamic_cast<Dispatcher *> (Factory::GetInstance(DISPATCHER_SINGLETON));

	// ACS Command OpCodes
	success &= reg.RegisterHandler(MessageIdentifierType(SERVER_LOCATION_CDH, CDH_CPU_USAGE_CMD), cdhCPUUsageHandler);
	success &= reg.RegisterHandler(MessageIdentifierType(SERVER_LOCATION_CDH, CDH_MEM_USAGE_CMD), cdhMemUsageHandler);
	success &= reg.RegisterHandler(MessageIdentifierType(SERVER_LOCATION_CDH, CDH_TEMP_START_CMD), cdhTempStartHandler);
	success &= reg.RegisterHandler(MessageIdentifierType(SERVER_LOCATION_CDH, CDH_TEMP_READ_CMD), cdhTempReadHandler);
	success &= reg.RegisterHandler(MessageIdentifierType(SERVER_LOCATION_CDH, CDH_HOT_SWAPS_CMD), cdhHotSwapsHandler);
	success &= reg.RegisterHandler(MessageIdentifierType(SERVER_LOCATION_CDH, CDH_POWER_MONITORS_CMD), cdhPowerMonitorsHandler);
	success &= reg.RegisterHandler(MessageIdentifierType(SERVER_LOCATION_CDH, CDH_START_PM_CMD), cdhStartPMHandler);
	success &= reg.RegisterHandler(MessageIdentifierType(SERVER_LOCATION_CDH, CDH_CLEAN_FS_CMD), cdhCleanFSHandler);

	for(int opcode = CDH_CMD_MIN; opcode < CDH_CMD_MAX; opcode++)
	{
		success &= arby.ModifyPermission(MessageIdentifierType(SERVER_LOCATION_CDH, opcode), true);
	}

	success &= dispatcher->AddRegistry(id, &reg, &arby);

	return success;
}

// -------------------------------------------- Loops ---------------------------------------------
void CDHServer::loopInit(){
	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));

	logger->Log("CDHServer: Initializing", LOGGER_LEVEL_INFO);

	bool initHS = devMngr->initializeHS();
	if(!initHS){
		logger->Log("CDHServer: Error initializing hot swaps!", LOGGER_LEVEL_ERROR);
	}

#if PM_EN
	devMngr->initializePM();
#endif //PM_EN

	prepPowerGPIOs();
	currentState = ST_MONITOR;
}

void CDHServer::loopMonitor(){
	uint64 lastWake = getTimeInMillis();
	ModeManager * modeManager = dynamic_cast<ModeManager *> (Factory::GetInstance(MODE_MANAGER_SINGLETON));
	uint8 readFrequency = 5;

	readHealth(readFrequency, (uint32) lastWake/1000);

	if(modeManager->GetMode() == MODE_DIAGNOSTIC){
		currentState = ST_DIAGNOSTIC;
	}

	waitUntil(lastWake, 1000);
}

void CDHServer::loopDiagnostic(){
	uint64 lastWake = getTimeInMillis();

	ModeManager * modeManager = dynamic_cast<ModeManager *> (Factory::GetInstance(MODE_MANAGER_SINGLETON));
	if(modeManager->GetMode() != MODE_DIAGNOSTIC){
		currentState = ST_MONITOR;
	}

	waitUntil(lastWake, 1000);
}


// ----------------------------------------- CDH Methods -----------------------------------------
void CDHServer::readHealth(uint8 frequency, uint32 timeUnit){
	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));

	ACPPacket * TSRet;
	ACPPacket * TRRet;
	ACPPacket * CPURet;
	ACPPacket * MemRet;
	ACPPacket * HtswRet;
	ACPPacket * PMRet;
	ACPPacket * SPMRet;
	int ccRet;

	// Check storage, and delete files if necessary
#if STOR_EN
	if(((timeUnit - 60 - 1) % 60) == 0){
		logger->Log("------------------ Checking storage --------------------", LOGGER_LEVEL_DEBUG);
		ccRet = storMngr->CheckAndClean();
		if(ccRet == 1){
			ACPPacket * packet = new ACPPacket(CDH_CLEAN_FS_SUCCESS);
			PacketProcess(SERVER_LOCATION_CDH, packet);
		}else if(ccRet == -1){
			ACPPacket * packet = new ACPPacket(CDH_CLEAN_FS_FAILURE);
			PacketProcess(SERVER_LOCATION_CDH, packet);
		}
	}
#endif //STOR_EN

	// Start sensors for reading next round
#if PM_EN
	if((timeUnit % 10) == 0){
		SPMRet = CDHStartPM();
		PacketProcess(SERVER_LOCATION_CDH, SPMRet);
	}
#endif //PM_EN

#if TEMP_EN
	if(((timeUnit - frequency - 2) % frequency) == 0){
		TSRet = CDHTempStart();
		PacketProcess(SERVER_LOCATION_CDH, TSRet);
	}
#endif //TEMP_EN

	// Get all CDH information
	if(((timeUnit - frequency - 1) % frequency) == 0){
		logger->Log("CDHServer: Gathering information", LOGGER_LEVEL_DEBUG);

		// ensure that no power faults have been detected, if they have, power off the subsystem
		//devMngr->checkHS();

#if CPU_EN
		// CPU usage
		CPURet = CDHCPUUsage();
		PacketProcess(SERVER_LOCATION_CDH, CPURet);
#endif //CPU_EN

#if MEM_EN
		// Memory usage
		MemRet = CDHMemUsage();
		PacketProcess(SERVER_LOCATION_CDH, MemRet);
#endif //MEM_EN

#if TEMP_EN
		// Read Temp sensors
		TRRet = CDHTempRead();
		PacketProcess(SERVER_LOCATION_CDH, TRRet);
#endif //TEMP_EN

#if HS_EN
		// Read Hot swaps
		HtswRet = CDHHotSwaps();
		PacketProcess(SERVER_LOCATION_CDH, HtswRet);
#endif //HS_EN

#if PM_EN
		// Read Power Monitors
		PMRet = CDHPowerMonitors();
		PacketProcess(SERVER_LOCATION_CDH, PMRet);
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

}
}
