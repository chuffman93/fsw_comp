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

#include "core/CommandMessage.h"
#include "core/ReturnMessage.h"
#include "core/Singleton.h"
#include "core/Factory.h"
#include "core/ErrorMessage.h"
#include "core/StdTypes.h"
#include "core/WatchdogManager.h"

#include "core/ModeManager.h"
#include "core/AccessMode.h"
#include "core/BusPriorityMode.h"

#include "util/Logger.h"
#include "util/FileHandler.h"

#include <iostream>
#include <sys/sysinfo.h>
#include <sys/statvfs.h>

using namespace Phoenix::Core;
using namespace Phoenix::HAL;
using namespace std;

#define CPU_EN 	0
#define MEM_EN 	0
#define STOR_EN 0
#define TEMP_EN 0
#define HS_EN	0
#define PM_EN	0

namespace Phoenix
{
	namespace Servers
	{
		static CDHCPUUsageHandler * cdhCPUUsageHandler;
		static CDHMemUsageHandler * cdhMemUsageHandler;
		static CDHStorageHandler * cdhStorageHandler;
		static CDHTempStartHandler * cdhTempStartHandler;
		static CDHTempReadHandler * cdhTempReadHandler;
		static CDHHotSwapsHandler * cdhHotSwapsHandler;
		static CDHPowerMonitorsHandler * cdhPowerMonitorsHandler;
		static CDHStartPMHandler * cdhStartPMHandler;

		// -------------------------------------- Necessary Methods --------------------------------------
		CDHServer::CDHServer(std::string nameIn, LocationIDType idIn)
				: SubsystemServer(nameIn, idIn), Singleton(), arby(idIn)
		{
			devMan = new I2CDeviceManager();
		}

		CDHServer::~CDHServer()
		{
			delete devMan;
		}

		CDHServer & CDHServer::operator=(const CDHServer & source)
		{
			if (this == &source)
			{
				return *this;
			}

			SubsystemServer::operator =(source);

			return *this;
		}

		void CDHServer::Initialize(void)
		{
			cdhCPUUsageHandler = new CDHCPUUsageHandler();
			cdhMemUsageHandler = new CDHMemUsageHandler();
			cdhStorageHandler = new CDHStorageHandler();
			cdhTempStartHandler = new CDHTempStartHandler();
			cdhTempReadHandler = new CDHTempReadHandler();
			cdhHotSwapsHandler = new CDHHotSwapsHandler();
			cdhPowerMonitorsHandler = new CDHPowerMonitorsHandler();
			cdhStartPMHandler = new CDHStartPMHandler;
		}
#ifdef TEST
		void CDHServer::Destroy(void)
		{
			delete cdhCPUUsageHandler;
			delete cdhMemUsageHandler;
			delete cdhStorageHandler;
			delete cdhTempStartHandler;
			delete cdhTempReadHandler;
			delete cdhHotSwapsHandler;
			delete cdhPowerMonitorsHandler;
			delete cdhStartPMHandler;
		}
#endif
		bool CDHServer::IsFullyInitialized(void)
		{
			return(Singleton::IsFullyInitialized());
		}

		void CDHServer::Update(const SystemMode * mode)
		{
			/* Called by mode manager each time mode changes
			 * Ex: Needs to do things to close mode 1, enter mode 2
			 * Setup and tear down between modes
			 */
		}

		bool CDHServer::RegisterHandlers()
		{
			bool success = true;
			Dispatcher * dispatcher = dynamic_cast<Dispatcher *> (Factory::GetInstance(DISPATCHER_SINGLETON));

			// ACS Command OpCodes
			success &= reg.RegisterHandler(MessageIdentifierType(MESSAGE_TYPE_COMMAND, CDH_CPU_USAGE_CMD), cdhCPUUsageHandler);
			success &= reg.RegisterHandler(MessageIdentifierType(MESSAGE_TYPE_COMMAND, CDH_MEM_USAGE_CMD), cdhMemUsageHandler);
			success &= reg.RegisterHandler(MessageIdentifierType(MESSAGE_TYPE_COMMAND, CDH_STORAGE_CMD), cdhStorageHandler);
			success &= reg.RegisterHandler(MessageIdentifierType(MESSAGE_TYPE_COMMAND, CDH_TEMP_START_CMD), cdhTempStartHandler);
			success &= reg.RegisterHandler(MessageIdentifierType(MESSAGE_TYPE_COMMAND, CDH_TEMP_READ_CMD), cdhTempReadHandler);
			success &= reg.RegisterHandler(MessageIdentifierType(MESSAGE_TYPE_COMMAND, CDH_HOT_SWAPS_CMD), cdhHotSwapsHandler);
			success &= reg.RegisterHandler(MessageIdentifierType(MESSAGE_TYPE_COMMAND, CDH_POWER_MONITORS_CMD), cdhPowerMonitorsHandler);
			success &= reg.RegisterHandler(MessageIdentifierType(MESSAGE_TYPE_COMMAND, CDH_START_PM_CMD), cdhStartPMHandler);

			for(int opcode = CDH_CMD_MIN; opcode < CDH_CMD_MAX; opcode++)
			{
				success &= arby.ModifyPermission(MessageIdentifierType(MESSAGE_TYPE_COMMAND, opcode), true);
			}

			success &= dispatcher->AddRegistry(id, &reg, &arby);

			return success;
		}

		// -------------------------------------------- Loops ---------------------------------------------
		void CDHServer::loopInit(){
			ModeManager * modeManager = dynamic_cast<ModeManager *> (Factory::GetInstance(MODE_MANAGER_SINGLETON));
#if HS_EN
			bool initHS = devMan->initializeHS();
			if(!initHS){
				logger->Log("CDHServer: Error initializing hot swaps!", LOGGER_LEVEL_ERROR);
			}
#endif

#if PM_EN
			devMan->initializePM();
#endif //PM_EN

			prepPowerGPIOs();
			currentState = ST_MONITOR;

		}

		void CDHServer::loopMonitor(){
			uint8 readFrequency = 10;

			readHealth(readFrequency, getTimeInMillis()/1000);
		}


		// ----------------------------------------- CDH Methods -----------------------------------------
		void CDHServer::readHealth(uint8 frequency, uint8 timeUnit)
		{
			Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));

			FSWPacket * TSRet;
			FSWPacket * TRRet;
			FSWPacket * CPURet;
			FSWPacket * MemRet;
			FSWPacket * StrRet;
			FSWPacket * HtswRet;
			FSWPacket * PMRet;
			FSWPacket * SPMRet;

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

#if STOR_EN
				// Storage in use
				StrRet = CDHStorage();
				PacketProcess(SERVER_LOCATION_CDH, StrRet);
#endif //STOR_EN

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
		}

		void CDHServer::subPowerOff(HardwareLocationIDType subsystem){
			toggleSubPower(subsystem, false);
		}

		void CDHServer::resetAssert(HardwareLocationIDType subsystem){
			toggleResetLine(subsystem, false);
		}
		void CDHServer::resetDeassert(HardwareLocationIDType subsystem){
			toggleResetLine(subsystem, true);
		}
	}
}

