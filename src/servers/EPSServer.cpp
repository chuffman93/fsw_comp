/*
* EPSServer.cpp
*
*  Created on: Feb 21, 2011
*      Author: Michael
*  Modified by: Conrad Hougen
*/

//#include "boards/board.h"
#include "HAL/RTC.h"
#include "HAL/GPIO.h"

#include "core/CommandMessage.h"
#include "core/ReturnMessage.h"
#include "core/Singleton.h"
#include "core/Factory.h"
#include "core/WatchdogManager.h"
#include "core/AccessMode.h"
#include "core/StartupMode.h"
#include "core/BusPriorityMode.h"
#include "core/PayloadPriorityMode.h"
#include "core/ErrorMode.h"
#include "core/ComMode.h"
#include "core/SystemMode.h"
#include "core/ErrorMessage.h"
#include "core/StdTypes.h"

#include "servers/EPSHandlers.h"
#include "servers/EPSServer.h"
#include "servers/EPSStdTasks.h"
#include "servers/DispatchStdTasks.h"
#include "util/Logger.h"

//#include "boards/backplane/dbg_led.h"

using namespace std;
using namespace Phoenix::Core;
using namespace Phoenix::HAL;

#define EPS_DEBUG			1

#if EPS_DEBUG && defined(WIN32)
#include <iostream>
#define DEBUG_COUT(m) cout << m << endl;
#else
#define DEBUG_COUT(m)
#endif // DEBUG

namespace Phoenix
{
	namespace Servers
	{
		// Message handler for a measurements request.
		static EPSHSHandler * epsHSHandler;
		// Message handler for a power cycle command.
		static EPSPowerCycleHandler * epsPowerCycleHandler;
		// Message handler for EPS generated errors.
		static EPSErrorHandler * epsErrorHandler;
		
		// -------------------------------------- Necessary Methods --------------------------------------
		EPSServer::EPSServer(string nameIn, LocationIDType idIn)
				: SubsystemServer(nameIn, idIn), Singleton(), arby(idIn), sensorThreshhold(0)
		{
			powerStateACS = false;
			powerStateCOM = false;
			powerStateGPS = false;
			powerStatePLD = false;
			powerStatePROP = false;
		}

		EPSServer::~EPSServer()
		{
			
		}

		EPSServer & EPSServer::operator=(const EPSServer & source)
		{
			if (this == &source)
			{
				return *this;
			}

			SubsystemServer::operator =(source);

			return *this;
		}
		
		void EPSServer::Initialize(void)
		{
			//Initialize handlers
			epsHSHandler = new EPSHSHandler();
			epsPowerCycleHandler = new EPSPowerCycleHandler();

			/* OUTDATED HANDLERS
			epsSOCHandler = new EPSStateofChargeHandler();
			epsDisHandler = new EPSDisableOCHandler();
			epsEnHandler = new EPSEnableOCHandler();epsPowerHandler = new EPSPowerHandler();
			*/

			epsErrorHandler = new EPSErrorHandler();

		}
#ifdef TEST
		void EPSServer::Destroy(void)
		{
			//delete handlers
			delete epsHSHandler;
			delete epsPowerCycleHandler;

			/* OUTDATED HANDLERS
			delete epsSOCHandler;
			delete epsDisHandler;
			delete epsEnHandler;
			delete epsPowerHandler;
			*/

			delete epsErrorHandler;
		}
#endif
		bool EPSServer::IsFullyInitialized(void)
		{
			return(Singleton::IsFullyInitialized());
		}

		void EPSServer::Update(const SystemMode * mode)
		{
			/* Called by mode manager each time mode changes
			* Ex: Needs to do things to close mode 1, enter mode 2
			* Setup and tear down between modes
			*/
		}

		bool EPSServer::RegisterHandlers()
		{
			bool success = true;

			Dispatcher * dispatcher = dynamic_cast<Dispatcher *> (Factory::GetInstance(DISPATCHER_SINGLETON));

			success &= reg.RegisterHandler(MessageIdentifierType(MESSAGE_TYPE_COMMAND, EPS_HS_CMD), epsHSHandler);
			success &= reg.RegisterHandler(MessageIdentifierType(MESSAGE_TYPE_COMMAND, EPS_POWER_CYCLE_CMD), epsPowerCycleHandler);

			/*
			success &= reg.RegisterHandler(MessageIdentifierType(MESSAGE_TYPE_COMMAND, EPS_SOC_CMD), epsSOCHandler);
			success &= reg.RegisterHandler(MessageIdentifierType(MESSAGE_TYPE_COMMAND, EPS_DISABLE_OC_PROT_CMD), epsDisHandler);
			success &= reg.RegisterHandler(MessageIdentifierType(MESSAGE_TYPE_COMMAND, EPS_ENABLE_OC_PROT_CMD), epsEnHandler);
			success &= reg.RegisterHandler(MessageIdentifierType(MESSAGE_TYPE_COMMAND, EPS_POWER_SUB_CMD), epsPowerHandler);
			*/

			for(int opcode = EPS_CMD_MIN; opcode < EPS_CMD_MAX; opcode++)
			{
				success &= arby.ModifyPermission(MessageIdentifierType(MESSAGE_TYPE_COMMAND, opcode), true);
			}

			for(int opcode = EPS_SUB_ERROR_MIN; opcode < EPS_SUB_ERROR_MAX; opcode++)
			{
				success &= reg.RegisterHandler(MessageIdentifierType(MESSAGE_TYPE_ERROR, opcode), epsErrorHandler);
				success &= arby.ModifyPermission(MessageIdentifierType(MESSAGE_TYPE_COMMAND, opcode), true);
			}

			success &= dispatcher->AddRegistry(id, &reg, &arby);

			return success;
		}

		// -------------------------------------------- Loop ---------------------------------------------
		void EPSServer::SubsystemLoop(void)
		{
			Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
			logger->Log("	EPSServer: Entering subsystem loop...", LOGGER_LEVEL_INFO);

			ModeManager * modeManager = dynamic_cast<ModeManager *> (Factory::GetInstance(MODE_MANAGER_SINGLETON));

			const SystemMode * mode;
			SystemModeEnum modeIndex;

			modeArray[0] = &EPSServer::EPSAccessMode;
			modeArray[1] = &EPSServer::EPSStartupMode;
			modeArray[2] = &EPSServer::EPSBusMode;
			modeArray[3] = &EPSServer::EPSPayloadMode;
			modeArray[4] = &EPSServer::EPSErrorMode;
			modeArray[5] = &EPSServer::EPSComMode;

			while(1)
			{
				mode = modeManager->GetMode();
				if (mode == NULL)
				{
					// FIXME: handle this!
					logger->Log("EPSServer: Mode Manager thinks mode is NULL!", LOGGER_LEVEL_FATAL);
					return;
				}
				modeIndex = mode->GetID();
				(this->*modeArray[modeIndex]) (modeManager);
			}

		}

		// -------------------------------------------- Modes --------------------------------------------
		void EPSServer::EPSAccessMode(ModeManager * modeManager)
		{

			EPSServer * epsServer = dynamic_cast<EPSServer *> (Factory::GetInstance(EPS_SERVER_SINGLETON));
			Dispatcher * dispatcher = dynamic_cast<Dispatcher *> (Factory::GetInstance(DISPATCHER_SINGLETON));
			Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
			//WatchdogManager * wdm = dynamic_cast<WatchdogManager *> (Factory::GetInstance(WATCHDOG_MANAGER_SINGLETON));

			const SystemMode * mode = AccessMode::GetInstance();
			const SystemMode * currentMode = mode;
			uint8 seconds = 0;
			uint8 input = 1;
			int i = 0;
			uint64_t LastWakeTime;
			uint32 resetTime = 0;

			FSWPacket * HSRet;
			FSWPacket * PCRet;
			FSWPacket * SOCRet;
			FSWPacket * rstRet;

			logger->Log("EPS successfully entered Access Mode\n", LOGGER_LEVEL_INFO);

			for(i = 0; i < 5; i++)
			{
				//wdm->Kick();
				usleep(1000000);
			}


#ifdef HARDWARE
			EPSPowerSubsystems(POWER_SUBSYSTEM_ACS, true, 0);
			EPSPowerSubsystems(POWER_SUBSYSTEM_GPS, true, 0);
			EPSPowerSubsystems(POWER_SUBSYSTEM_COM, true, 0);
			EPSPowerSubsystems(POWER_SUBSYSTEM_PLD, true, 0);
#endif //HARDWARE

			
			while(mode == currentMode)
			{
				logger->Log("EPS Access Mode loop start", LOGGER_LEVEL_DEBUG);

				LastWakeTime = getTimeInMilis();
				//wdm->Kick();
				
				while(dispatcher->Listen(id));

				seconds++;
				

				// Check current mode
				currentMode = modeManager->GetMode();


				if ((seconds % 10) == 0 )
				{
					
					// Functions
					logger->Log("Calling EPS Health and Status Function", LOGGER_LEVEL_DEBUG);
					HSRet = EPSHealthStat();
					logger->Log("Calling Message process on the ret packet!", LOGGER_LEVEL_DEBUG);
					PacketProcess(SERVER_LOCATION_EPS, HSRet);

//					logger->Log("Power Cycling!", LOGGER_LEVEL_FATAL);
//					PCRet = EPSPowerCycle();
//					logger->Log("Calling Message process on the ret packet!", LOGGER_LEVEL_DEBUG);
//					PacketProcess(SERVER_LOCATION_EPS, PCRet);

					//SOCRet = EPSStateOfCharge();
					//MessageProcess(SERVER_LOCATION_EPS, SOCRet);
					
				}

				if((seconds % 60) == 0)
				{
					//RstRet = EPSPowerCycle();
					//MessageProcess(SERVER_LOCATION_EPS, RstRet);
					seconds = 0;
				}

				//CheckPowerTimers();

				//SattyResetTimer();
				// Delay
				waitUntil(LastWakeTime, 1000);
			}
		}

		void EPSServer::EPSStartupMode(ModeManager * modeManager)
		{
			Dispatcher * dispatcher = dynamic_cast<Dispatcher *> (Factory::GetInstance(DISPATCHER_SINGLETON));
			//WatchdogManager * wdm = dynamic_cast<WatchdogManager *> (Factory::GetInstance(WATCHDOG_MANAGER_SINGLETON));

			uint8 seconds = 0;
			ReturnMessage * RstRet;
			int i = 0;
			
			//check deployment flag
			//if true, turn on subsystems and change to bus mode and exit
			if(CheckDeploymentFlag())
			{
				
				for(i = 0; i < 5; i++)
				{
					//wdm->Kick();
					usleep(1000000);
				}
#ifdef HARDWARE
				EPSPowerSubsystems(POWER_SUBSYSTEM_ACS, true, 0);
				EPSPowerSubsystems(POWER_SUBSYSTEM_GPS, true, 0);
				EPSPowerSubsystems(POWER_SUBSYSTEM_COM, true, 0);
				EPSPowerSubsystems(POWER_SUBSYSTEM_PLD, true, 0);
				
				//Check fuck it flag here
#endif //HARDWARE
				modeManager->SetMode(MODE_BUS_PRIORITY, id);
				usleep(500000);
				return;
			}
			
			//else wait 45 min
			uint32 delay = 0;
			int LastWakeTime;
			//TODO:RTC REPLACE
			//RTCSetTime(0);
			while(delay < INHIBIT_WAIT_TIME)
			{
				LastWakeTime = getTimeInMilis();
				//wdm->Kick();
#ifdef HARDWARE
				Phoenix::HAL::RTCGetTime(&delay , NULL);
#endif //HARDWARE
				waitUntil(LastWakeTime, 1000);
			}
#ifdef HARDWARE
			//set start camera sensor threshold
			EPSPowerSubsystems(POWER_SUBSYSTEM_ACS, true, 0);
			
			for(i = 0; i < 20; i++)
			{
				//wdm->Kick();
				usleep(1000000);
			}
			sensorThreshhold = SetStarCameraThreshold();
			for(i = 0; i < 5; i++)
			{
				//wdm->Kick();
				usleep(1000000);
			}
			EPSPowerSubsystems(POWER_SUBSYSTEM_ACS, false, 0);
			//outer for loop, 3 iterations
			for(i = 0; i < 6; i++)
			{
				//inner for loop condition on reading the button 3x
				for(int j = 0; (j < 3) ; j++)
				{
					//burn 3x
					//wdm->Kick();
					BurnFrangiBolt();
				}
				
				//turn on ACS
				EPSPowerSubsystems(POWER_SUBSYSTEM_ACS, true, 0);
				//TODO: Do we need to turn on GPS here too?
				
				uint32 offset = 0;
				//Phoenix::HAL::RTCGetTime(&offset , NULL);
				//Phoenix::HAL::RTCGetTime(&delay , NULL);
				
				//while loop based on star camera lock 5 minutes
				while((delay - offset) < STAR_CAMERA_LOCK_WAIT_TIME)
				{
					//wdm->Kick();
					//if star camera is deployed, set deployment flag power cycle this bitch
					if(CheckStarCameraDeployed(sensorThreshhold))
					{
						LastWakeTime = getTimeInMilis();
						SetDeploymentFlag();
						RstRet = EPSPowerCycle();
						MessageProcess(SERVER_LOCATION_EPS, RstRet);
						for(i = 0; i < 10; i++)
						{
							//wdm->Kick();
							usleep(1000000);
						}
						return;
					}
					//Phoenix::HAL::RTCGetTime(&delay , NULL);
					waitUntil(LastWakeTime, 1000);
				}
				EPSPowerSubsystems(POWER_SUBSYSTEM_ACS, false, 0);
			}
#endif //HARDWARE
			//if we get here we have done all 18 burns and don't have a star camera lock, set fuck it error flag and power cycle
		}

		void EPSServer::EPSBusMode(ModeManager * modeManager)
		{
			Dispatcher * dispatcher = dynamic_cast<Dispatcher *> (Factory::GetInstance(DISPATCHER_SINGLETON));
			//WatchdogManager * wdm = dynamic_cast<WatchdogManager *> (Factory::GetInstance(WATCHDOG_MANAGER_SINGLETON));
			Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));

			const SystemMode * mode = BusPriorityMode::GetInstance();
			const SystemMode * currentMode = mode;
			uint8 seconds = 0;

			ReturnMessage * HSRet;
			ReturnMessage * SOCRet;

			logger->Log("EPSServer: Entered Bus Priority Mode!", LOGGER_LEVEL_INFO);

			while(mode == currentMode)
			{
				uint64_t LastWakeTime = getTimeInMilis();
				////wdm->Kick();
				while(dispatcher->Listen(id));
				
				//debug_led_set_led(1, LED_TOGGLE);

				// Check current mode
				currentMode = modeManager->GetMode();

				seconds++;
#ifdef HARDWARE
				if ((seconds % 10) == 0 )
				{
					// Functions

					HSRet = EPSHealthStat();
					MessageProcess(SERVER_LOCATION_EPS, HSRet);

					SOCRet = EPSStateOfCharge();
					MessageProcess(SERVER_LOCATION_EPS, SOCRet);

					seconds = 0;
				}
				SattyResetTimer();
				CheckPowerTimers();
#endif //HARDWARE
				// Delay
				waitUntil(LastWakeTime, 1000);
			}

		}

		void EPSServer::EPSPayloadMode(ModeManager * modeManager)
		{
			EPSServer * epsServer = dynamic_cast<EPSServer *> (Factory::GetInstance(EPS_SERVER_SINGLETON));
			Dispatcher * dispatcher = dynamic_cast<Dispatcher *> (Factory::GetInstance(DISPATCHER_SINGLETON));
			//WatchdogManager * wdm = dynamic_cast<WatchdogManager *> (Factory::GetInstance(WATCHDOG_MANAGER_SINGLETON));
			const SystemMode * mode = PayloadPriorityMode::GetInstance();
			const SystemMode * currentMode = mode;
			uint8 seconds = 0;

			ReturnMessage * HSRet;
			ReturnMessage * SOCRet;

			while(mode == currentMode)
			{
				uint64_t LastWakeTime = getTimeInMilis();
				//wdm->Kick();
				while(dispatcher->Listen(id));

				// Check current mode
				currentMode = modeManager->GetMode();

				seconds++;
#ifdef HARDWARE
				if ((seconds % 10) == 0 )
				{
					// Functions

					HSRet = EPSHealthStat();
					MessageProcess(SERVER_LOCATION_EPS, HSRet);
					SOCRet = EPSStateOfCharge();
					MessageProcess(SERVER_LOCATION_EPS, SOCRet);

					seconds = 0;
				}
				SattyResetTimer();
				CheckPowerTimers();
#endif //HARDWARE
				// Delay
				waitUntil(LastWakeTime, 1000);
			}
		}

		void EPSServer::EPSErrorMode(ModeManager * modeManager)
		{
			Dispatcher * dispatcher = dynamic_cast<Dispatcher *> (Factory::GetInstance(DISPATCHER_SINGLETON));
			////WatchdogManager * wdm = dynamic_cast<WatchdogManager *> (Factory::GetInstance(WATCHDOG_MANAGER_SINGLETON));
			const SystemMode * mode = ErrorMode::GetInstance();
			const SystemMode * currentMode = mode;
			uint8 seconds = 0;

			ReturnMessage * HSRet;
			ReturnMessage * SOCRet;

			while(mode == currentMode)
			{
				uint64_t LastWakeTime = getTimeInMilis();
				//wdm->Kick();
				while(dispatcher->Listen(id));

				// Check current mode
				currentMode = modeManager->GetMode();

				seconds++;
#ifdef HARDWARE
				if ((seconds % 10) == 0 )
				{
					// Functions
					HSRet = EPSHealthStat();
					MessageProcess(SERVER_LOCATION_EPS, HSRet);
					SOCRet = EPSStateOfCharge();
					MessageProcess(SERVER_LOCATION_EPS, SOCRet);

					seconds = 0;
				}
				// Delay
				//SattyResetTimer(); Should this be here?
#endif HARDWARE
				waitUntil(LastWakeTime, 1000);
			}
		}

		void EPSServer::EPSComMode(ModeManager * modeManager)
		{
			EPSServer * epsServer = dynamic_cast<EPSServer *> (Factory::GetInstance(EPS_SERVER_SINGLETON));
			Dispatcher * dispatcher = dynamic_cast<Dispatcher *> (Factory::GetInstance(DISPATCHER_SINGLETON));
			//WatchdogManager * wdm = dynamic_cast<WatchdogManager *> (Factory::GetInstance(WATCHDOG_MANAGER_SINGLETON));
			const SystemMode * mode = ComMode::GetInstance();
			const SystemMode * currentMode = mode;
			uint8 seconds = 0;

			ReturnMessage * HSRet;
			ReturnMessage * SOCRet;

			while(mode == currentMode)
			{
				uint64_t LastWakeTime = getTimeInMilis();
				//wdm->Kick();
				while(dispatcher->Listen(id));

				// Check current mode
				currentMode = modeManager->GetMode();

				seconds++;
				
				//debug_led_set_led(1, LED_TOGGLE);
#ifdef HARDWARE
				if ((seconds % 10) == 0 )
				{
					// Functions

					SOCRet = EPSStateOfCharge();
					MessageProcess(SERVER_LOCATION_EPS, SOCRet);

					seconds = 0;
				}
				SattyResetTimer();
				CheckPowerTimers();
#endif //HARDWARE
				// Delay
				waitUntil(LastWakeTime, 1000);
			}
	
		}
		
		// ----------------------------------------- EPS Methods -----------------------------------------
		bool EPSServer::getPowerSate(PowerSubsystemEnum subsystem)
		{
			switch (subsystem)
			{
				case POWER_SUBSYSTEM_ACS:
					return powerStateACS;
				case POWER_SUBSYSTEM_COM:
					return powerStateCOM;
				case POWER_SUBSYSTEM_GPS:
					return powerStateGPS;
				case POWER_SUBSYSTEM_PLD:
					return powerStatePLD;
				case POWER_SUBSYSTEM_PROP:
				return powerStatePROP;
				default:
				break;
			}
		}
		
		uint32 EPSServer::getPowerOffTimer(PowerSubsystemEnum subsystem)
		{
			switch (subsystem)
			{
				case POWER_SUBSYSTEM_ACS:
				return ACSOffTimer;
				case POWER_SUBSYSTEM_COM:
				return COMOffTimer;
				case POWER_SUBSYSTEM_GPS:
				return GPSOffTimer;
				case POWER_SUBSYSTEM_PLD:
				return PLDOffTimer;
				case POWER_SUBSYSTEM_PROP:
				return PROPOffTimer;
				default:
				break;
			}
		}

		void EPSServer::setPowerState(PowerSubsystemEnum subsystem, bool state)
		{
			switch (subsystem)
			{
				case POWER_SUBSYSTEM_ACS:
				powerStateACS = state;
				break;
				case POWER_SUBSYSTEM_COM:
				powerStateCOM = state;
				break;
				case POWER_SUBSYSTEM_GPS:
				powerStateGPS = state;
				break;
				case POWER_SUBSYSTEM_PLD:
				powerStatePLD = state;
				break;
				case POWER_SUBSYSTEM_PROP:
				powerStatePROP = state;
				break;
				default:
				break;
			}
		}
		
		void EPSServer::setPowerOffTimer(PowerSubsystemEnum subsystem, uint32 time)
		{
			switch (subsystem)
			{
				case POWER_SUBSYSTEM_ACS:
					if(time >= ACS_MAX_OFF_TIME)
					{
						ACSOffTimer = ACS_MAX_OFF_TIME;
					}
					else
					{
						ACSOffTimer = time;	
					}
					break;
				case POWER_SUBSYSTEM_COM:
					if(time >= COM_MAX_OFF_TIME)
					{
						COMOffTimer = COM_MAX_OFF_TIME;
					}
					else
					{
						COMOffTimer = time;
					}
					break;
				case POWER_SUBSYSTEM_GPS:
					if(time >= GPS_MAX_OFF_TIME)
					{
						GPSOffTimer = GPS_MAX_OFF_TIME;
					}
					else
					{
						GPSOffTimer = time;
					}
					break;
				case POWER_SUBSYSTEM_PLD:
					if(time >= PLD_MAX_OFF_TIME)
					{
						PLDOffTimer = PLD_MAX_OFF_TIME;
					}
					else
					{
						PLDOffTimer = time;
					}
					break;
				case POWER_SUBSYSTEM_PROP:
					if(time >= PROP_MAX_OFF_TIME)
					{
						PROPOffTimer = PROP_MAX_OFF_TIME;
					}
					else
					{
						PROPOffTimer = time;
					}
					break;
				default:
				break;
			}
		}
	} // End Namespace servers
} // End Namespace Phoenix
