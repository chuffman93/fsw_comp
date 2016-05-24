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
#include "core/ModeManager.h"
#include "core/WatchdogManager.h"
#include <iostream>
#include <sys/sysinfo.h>
#include <sys/statvfs.h>

using namespace Phoenix::Core;
using namespace Phoenix::HAL;

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

		CDHServer::CDHServer(std::string nameIn, LocationIDType idIn)
				: SubsystemServer(nameIn, idIn), Singleton(), arby(idIn)
		{
			// Left Intentionally Blank
		}

		CDHServer::~CDHServer()
		{
			// Left Intentionally Blank
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

			for(int opcode = CDH_CMD_MIN; opcode < CDH_CMD_MAX; opcode++)
			{
				success &= arby.ModifyPermission(MessageIdentifierType(MESSAGE_TYPE_COMMAND, opcode), true);
			}

			success &= dispatcher->AddRegistry(id, &reg, &arby);

			return success;
		}

		void CDHServer::PrepHSPM(){
			for(uint8 i = 0; i < 16; i++){
				hotSwaps[i] = new HotSwap(adresses[i],faults[i],resistors[i]);
				//hotSwaps[i]->Init();
			}
			for(uint8 j = 0; j < 4; j++){
				powerMonitors[j] = new PowerMonitor(PM_adresses[j]);
				//powerMonitors[j]->Init(); //TODO: init with config struct for each
			}
		}

		void CDHServer::SubsystemLoop(void)
		{
			Dispatcher * dispatcher = dynamic_cast<Dispatcher *> (Factory::GetInstance(DISPATCHER_SINGLETON));
			//WatchdogManager * wdm = dynamic_cast<WatchdogManager *> (Factory::GetInstance(WATCHDOG_MANAGER_SINGLETON));

			std::cout<<"CDHServer: Subsystem Loop Entered"<<std::endl;

			//PrepHotSwaps();

			ReturnMessage * TSRet;
			ReturnMessage * TRRet;
			ReturnMessage * CPURet;
			ReturnMessage * MemRet;
			ReturnMessage * StrRet;
			ReturnMessage * HtswRet;

			uint64_t LastWakeTime = 0;
			uint8 seconds = 0;
			//bool shouldReadTemp[4][16];
			//float temperatures[4][16];
			while(1)
			{
				while(dispatcher->Listen(id));
				LastWakeTime = getTimeInMilis();
				//wdm->Kick();

				// Start sensors for reading next round
				if((seconds % 10) == 0){
					TSRet = CDHTempStart();
					MessageProcess(SERVER_LOCATION_CDH, TSRet);
				}

				// Get all CDH information
				if(((seconds - 1) % 10) == 0){

					// CPU usage
					CPURet = CDHCPUUsage();
					MessageProcess(SERVER_LOCATION_CDH, CPURet);

					// Memory usage
					MemRet = CDHMemUsage();
					MessageProcess(SERVER_LOCATION_CDH, MemRet);

					// Storage in use
					StrRet = CDHStorage();
					MessageProcess(SERVER_LOCATION_CDH, StrRet);

					// Read Temp sensors
					TRRet = CDHTempRead();
					MessageProcess(SERVER_LOCATION_CDH, TRRet);

					// Read Hot swaps
//					HtswRet = CDHHotSwaps();
//					MessageProcess(SERVER_LOCATION_CDH, HtswRet);
				}

				// Delay
				waitUntil(LastWakeTime, 1000);
				seconds++;
			}
		}
	}
}

