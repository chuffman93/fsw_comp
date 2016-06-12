/*
* PayloadServer.cpp
*
*  Created on: May 27, 2011
*      Author: Michael
*      Last Updated: 11/23/12
*      Last Updated by: Glenda
*/

#include "servers/PLDServer.h"
#include "servers/PLDStdTasks.h"
#include "servers/DispatchStdTasks.h"

#include "core/CommandMessage.h"
#include "core/ReturnMessage.h"
#include "core/Singleton.h"
#include "core/Factory.h"
#include "core/WatchdogManager.h"
#include "core/ErrorMessage.h"
#include "core/StdTypes.h"
#include "core/ModeManager.h"
#include "core/AccessMode.h"
#include "core/StartupMode.h"
#include "core/BusPriorityMode.h"
#include "core/PayloadPriorityMode.h"
#include "core/ErrorMode.h"
#include "core/ComMode.h"
#include "core/SystemMode.h"

#include "util/FileHandler.h"

//#include "boards/backplane/dbg_led.h"

using namespace std;
using namespace Phoenix::Core;

namespace Phoenix
{
	namespace Servers
	{
		// Message handler for a health and status command.
		static PLDHSHandler * pldHSHandler;
		// Message handler for a picture command.
		static PLDPictureTakeHandler * pldPicHandler;
		// Message handler for a get picture command.
		static PLDGetPictureHandler * pldGetPicHandler;
		// Message handler for a data command.
		static PLDDataHandler * pldGetDatHandler;
		// Message handler for a reset command.
		static PLDResetHandler * pldRstHandler;
		static PLDResSetHandler * pldResSetHandler;
		static PLDSetChunkSizeHandler * pldChuSetHandler;
		static PLDSetGainHandler * pldGainSetHandler;
		static PLDSetExpTimeHandler * pldExpSetHandler;
		// Message handler for an error.
		static PLDErrorHandler * pldErrorHandler;
		
		PLDServer::PLDServer(string nameIn, LocationIDType idIn) :
				SubsystemServer(nameIn, idIn), Singleton(), arby(idIn), resolution(DEFAULT_RES_X*DEFAULT_RES_Y), chuckSize(DEFAULT_CHUNK_SIZE)
		{
			//TODO: default gain and exposure
		}

		PLDServer::~PLDServer()
		{
			// Left Intentionally Blank
		}

		PLDServer & PLDServer::operator=(const PLDServer & source)
		{
			if (this == &source)
			{
				return *this;
			}

			SubsystemServer::operator =(source);

			return *this;
		}
		
		void PLDServer::Initialize(void)
		{
			pldHSHandler = new PLDHSHandler();
			pldPicHandler = new PLDPictureTakeHandler();
			pldGetPicHandler = new PLDGetPictureHandler();
			pldGetDatHandler = new PLDDataHandler();
			pldRstHandler = new PLDResetHandler();
			pldResSetHandler = new PLDResSetHandler();
			pldChuSetHandler = new PLDSetChunkSizeHandler();
			pldGainSetHandler = new PLDSetGainHandler();
			pldExpSetHandler = new PLDSetExpTimeHandler();
			pldErrorHandler = new PLDErrorHandler();
		}
		
#ifdef TEST
		void PLDServer::Destroy(void)
		{
			delete pldHSHandler;
			delete pldPicHandler;
			delete pldGetPicHandler;
			delete pldGetDatHandler;
			delete pldRstHandler;
			delete pldResSetHandler;
			delete pldChuSetHandler;
			delete pldGainSetHandler;
			delete pldExpSetHandler;
			delete pldErrorHandler;
		}
#endif
		
		bool PLDServer::IsFullyInitialized(void)
		{
			return(Singleton::IsFullyInitialized());
		}

		void PLDServer::Update(const SystemMode * mode)
		{
			/*mode manager
			*when mode changes this func is called
			*need to exit current mode and enter next mode
			*zip all open files
			*/

		}

		bool PLDServer::RegisterHandlers()
		{
			bool success = true;
			Dispatcher * dispatcher = dynamic_cast<Dispatcher *> (Factory::GetInstance(DISPATCHER_SINGLETON));

			// Register PLD handlers

			success &= reg.RegisterHandler(MessageIdentifierType(MESSAGE_TYPE_COMMAND, PLD_HS_CMD), pldHSHandler);
			success &= reg.RegisterHandler(MessageIdentifierType(MESSAGE_TYPE_COMMAND, PLD_PIC_CMD), pldPicHandler);
			success &= reg.RegisterHandler(MessageIdentifierType(MESSAGE_TYPE_COMMAND, PLD_PIC_GET_CMD), pldGetPicHandler);
			success &= reg.RegisterHandler(MessageIdentifierType(MESSAGE_TYPE_COMMAND, PLD_DATA_CMD), pldGetDatHandler);
			success &= reg.RegisterHandler(MessageIdentifierType(MESSAGE_TYPE_COMMAND, PLD_RESET_CMD), pldRstHandler);

			success &= reg.RegisterHandler(MessageIdentifierType(MESSAGE_TYPE_CONFIG, PLD_RES_CONFIG), pldResSetHandler);
			success &= reg.RegisterHandler(MessageIdentifierType(MESSAGE_TYPE_CONFIG, PLD_CHUNK_CONFIG), pldChuSetHandler);
			success &= reg.RegisterHandler(MessageIdentifierType(MESSAGE_TYPE_CONFIG, PLD_GAIN_CONFIG), pldGainSetHandler);
			success &= reg.RegisterHandler(MessageIdentifierType(MESSAGE_TYPE_CONFIG, PLD_EXPTIME_CONFIG), pldExpSetHandler);


			for(int opcode = PLD_CMD_MIN; opcode < PLD_CMD_MAX; opcode++)
			{
				success &= arby.ModifyPermission(MessageIdentifierType(MESSAGE_TYPE_COMMAND, opcode), true);
			}
			
			for(int opcode = PLD_CONFIG_MIN; opcode < PLD_CONFIG_MAX; opcode++)
			{
				success &= arby.ModifyPermission(MessageIdentifierType(MESSAGE_TYPE_COMMAND, opcode), true);
			}

			for(int opcode = PLD_SUB_ERROR_MIN; opcode < PLD_SUB_ERROR_MAX; opcode++)
			{
				success &= reg.RegisterHandler(MessageIdentifierType(MESSAGE_TYPE_ERROR, opcode), pldErrorHandler);
				success &= arby.ModifyPermission(MessageIdentifierType(MESSAGE_TYPE_COMMAND, opcode), true);
			}

			success &= dispatcher->AddRegistry(id, &reg, &arby);

			return success;
		}


		void PLDServer::SubsystemLoop(void)
		{
			ModeManager * modeManager = dynamic_cast<ModeManager *> (Factory::GetInstance(MODE_MANAGER_SINGLETON));
			
			const SystemMode * mode = modeManager->GetMode();
			SystemModeEnum modeIndex = mode->GetID();

			// Index assignments for functional array
			modeArray[0] = &PLDServer::PLDAccessMode;
			modeArray[1] = &PLDServer::PLDStartupMode;
			modeArray[2] = &PLDServer::PLDBusMode;
			modeArray[3] = &PLDServer::PLDPayloadMode;
			modeArray[4] = &PLDServer::PLDErrorMode;
			modeArray[5] = &PLDServer::PLDComMode;

			while(1)
			{
				mode = modeManager->GetMode();
				if (mode == NULL)
				{
					// modeManagerError
					// Store Error to Data File
					// Recommend Reboot to Ground
					return;
				}
				modeIndex = mode->GetID();
				(this->*modeArray[modeIndex]) (modeManager);
			}

		}

		void PLDServer::PLDAccessMode(ModeManager * modeManager)
		{
			Dispatcher * dispatcher = dynamic_cast<Dispatcher *> (Factory::GetInstance(DISPATCHER_SINGLETON));
			//WatchdogManager * wdm = dynamic_cast<WatchdogManager *> (Factory::GetInstance(WATCHDOG_MANAGER_SINGLETON));
			const SystemMode * mode = AccessMode::GetInstance();
			const SystemMode * currentMode = mode;
			uint32 seconds = 0;

			FSWPacket * PicRet;
			FSWPacket * HSRet;
			FSWPacket * DatRet;
			FSWPacket * ChunkSize;
			bool getPic = false;
			
			while(mode == currentMode)
			{
				uint64_t LastWakeTime = getTimeInMilis();
				//wdm->Kick();
				//while(dispatcher->Listen(id));
				//debug_led_set_led(3, LED_TOGGLE);

				// Functions
				

				if(seconds % 3 == 0 && getPic == false)
				{
					printf("Received CDH CMD, initiating PLD data retrieval\n");
					PLDGetData();
					getPic = true;
					pthread_exit(0);
				}

// 				if ((seconds % 600) == 0 ) //Use this to change frequency.
// 				{
//					HSRet = PLDHealthStatus();
//					PacketProcess(SERVER_LOCATION_PLD, HSRet);
//					ChunkSize = PLDConfigChunkSize(150);
//					PacketProcess(SERVER_LOCATION_PLD, ChunkSize);
//					PicRet = PLDPicture();
//					PacketProcess(SERVER_LOCATION_PLD, PicRet);
//					DatRet = PLDGetPicture();
//					PacketProcess(SERVER_LOCATION_PLD, DatRet);
//
// 					seconds = 0;
// 				}
 				seconds++;

				// Delay
				waitUntil(LastWakeTime, 1000);
				
				// Check current mode
				currentMode = modeManager->GetMode();
			}
		}


		void PLDServer::PLDStartupMode(ModeManager * modeManager)
		{
			Dispatcher * dispatcher = dynamic_cast<Dispatcher *> (Factory::GetInstance(DISPATCHER_SINGLETON));
			//WatchdogManager * wdm = dynamic_cast<WatchdogManager *> (Factory::GetInstance(WATCHDOG_MANAGER_SINGLETON));
			const SystemMode * mode = StartupMode::GetInstance();
			const SystemMode * currentMode = mode;

			while(mode == currentMode)
			{
				uint64_t LastWakeTime = getTimeInMilis();
				//wdm->Kick();
				while(dispatcher->Listen(id));

				// Delay
				waitUntil(LastWakeTime, 1000);
				
				// Check current mode
				currentMode = modeManager->GetMode();
			}
		}



		void PLDServer::PLDBusMode(ModeManager * modeManager)
		{
			Dispatcher * dispatcher = dynamic_cast<Dispatcher *> (Factory::GetInstance(DISPATCHER_SINGLETON));
			//WatchdogManager * wdm = dynamic_cast<WatchdogManager *> (Factory::GetInstance(WATCHDOG_MANAGER_SINGLETON));
			const SystemMode * mode = BusPriorityMode::GetInstance();
			const SystemMode * currentMode = mode;
			uint8 seconds = 0;

			FSWPacket * HSRet;

			while(mode == currentMode)
			{
				uint64_t LastWakeTime = getTimeInMilis();
				//wdm->Kick();
				while(dispatcher->Listen(id));

				seconds++;
				if ((seconds % 60) == 0 )
				{
					// Functions

					//HSRet = PLDHealthStatus();
					//PacketProcess(SERVER_LOCATION_PLD, HSRet);

					seconds = 0;

				}
				// Delay
				waitUntil(LastWakeTime, 1000);
				
				// Check current mode
				currentMode = modeManager->GetMode();
			}

		}



		void PLDServer::PLDPayloadMode(ModeManager * modeManager)
		{
			Dispatcher * dispatcher = dynamic_cast<Dispatcher *> (Factory::GetInstance(DISPATCHER_SINGLETON));
			//WatchdogManager * wdm = dynamic_cast<WatchdogManager *> (Factory::GetInstance(WATCHDOG_MANAGER_SINGLETON));
			const SystemMode * mode = PayloadPriorityMode::GetInstance();
			const SystemMode * currentMode = mode;
			uint8 seconds = 0;
			FSWPacket * HSRet;

			while(mode == currentMode)
			{
				uint64_t LastWakeTime = getTimeInMilis();
				//wdm->Kick();
				while(dispatcher->Listen(id));

				seconds++;
				if ((seconds % 10) == 0 )
				{
					// Functions

					HSRet = PLDHealthStatus();
					PacketProcess(SERVER_LOCATION_PLD, HSRet);
					
					seconds = 0;

				}
				// Delay
				waitUntil(LastWakeTime, 1000);
				
				// Check current mode
				currentMode = modeManager->GetMode();
			}
		}



		void PLDServer::PLDErrorMode(ModeManager * modeManager)
		{
			Dispatcher * dispatcher = dynamic_cast<Dispatcher *> (Factory::GetInstance(DISPATCHER_SINGLETON));
			//WatchdogManager * wdm = dynamic_cast<WatchdogManager *> (Factory::GetInstance(WATCHDOG_MANAGER_SINGLETON));
			const SystemMode * mode = ErrorMode::GetInstance();
			const SystemMode * currentMode = mode;
			uint8 seconds = 0;

			FSWPacket * HSRet;

			while(mode == currentMode)
			{
				uint64_t LastWakeTime = getTimeInMilis();
				//wdm->Kick();
				while(dispatcher->Listen(id));

				// Check current mode
				currentMode = modeManager->GetMode();

				seconds++;

				if ((seconds % 60) == 0 )
				{
					// Functions

					HSRet = PLDHealthStatus();
					PacketProcess(SERVER_LOCATION_PLD, HSRet);

					seconds = 0;

				}
				// Delay
				waitUntil(LastWakeTime, 1000);
				
				// Check current mode
				currentMode = modeManager->GetMode();
			}
		}

		void PLDServer::PLDComMode(ModeManager * modeManager)
		{
			Dispatcher * dispatcher = dynamic_cast<Dispatcher *> (Factory::GetInstance(DISPATCHER_SINGLETON));
			//WatchdogManager * wdm = dynamic_cast<WatchdogManager *> (Factory::GetInstance(WATCHDOG_MANAGER_SINGLETON));
			const SystemMode * mode = ComMode::GetInstance();
			const SystemMode * currentMode = mode;
			uint8 seconds = 0;

			FSWPacket * HSRet;

			while(mode == currentMode)
			{
				uint64_t LastWakeTime = getTimeInMilis();
				//wdm->Kick();
				while(dispatcher->Listen(id));

				seconds++;
				if ((seconds % 10) == 0 )
				{
					// Functions

					HSRet = PLDHealthStatus();
					PacketProcess(SERVER_LOCATION_PLD, HSRet);

					seconds = 0;

				}
				// Delay
				waitUntil(LastWakeTime, 1000);
				
				// Check current mode
				currentMode = modeManager->GetMode();
			}
		}

		bool PLDServer::PLDSetResolution(uint32 res)
		{
			if(true == this->TakeLock(MAX_BLOCK_TIME))
			{
				resolution = res;
				this->GiveLock();
				return true;
			}
			return false;
		}

		bool PLDServer::PLDSetChunkSize(uint32 chunk)
		{
			if(true == this->TakeLock(MAX_BLOCK_TIME))
			{
				chuckSize = chunk;
				this->GiveLock();
				return true;
			}
			return false;
		}
		
		bool PLDServer::PLDSetGain(uint32 newGain)
		{
			if(true == this->TakeLock(MAX_BLOCK_TIME))
			{
				gain = newGain;
				this->GiveLock();
				return true;
			}
			return false;
		}
		
		bool PLDServer::PLDSetExpTime(uint32 newExpTime)
		{
			if(true == this->TakeLock(MAX_BLOCK_TIME))
			{
				exposureTime = newExpTime;
				this->GiveLock();
				return true;
			}
			return false;
		}

		bool PLDServer::PLDGetResolution(uint32 * res)
		{
			if(true == this->TakeLock(MAX_BLOCK_TIME))
			{
				* res = resolution;
				this->GiveLock();
				return true;
			}
			return false;
		}

		bool PLDServer::PLDGetChunkSize(uint32 * chunk)
		{
			if(true == this->TakeLock(MAX_BLOCK_TIME))
			{
				* chunk = chuckSize;
				this->GiveLock();
				return true;
			}
			return false;
		}
		
		bool PLDServer::PLDGetGain(uint32 * curGain)
		{
			if(true == this->TakeLock(MAX_BLOCK_TIME))
			{
				* curGain = gain;
				this->GiveLock();
				return true;
			}
			return false;
		}

		bool PLDServer::PLDGetExpTime(uint32 * curExp)
		{
			if(true == this->TakeLock(MAX_BLOCK_TIME))
			{
				* curExp = exposureTime;
				this->GiveLock();
				return true;
			}
			return false;
		}

	} // end namespace servers
} // end namespace Phoenix
