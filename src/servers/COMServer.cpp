/*
 * COMServer.cpp
 *
 *  Created on: Feb 21, 2011
 *      Author: Michael
 */

#include "servers/COMServer.h"
#include "servers/COMHandlers.h"
#include "servers/COMStdTasks.h"
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

/*extern "C"
{
	#include "util/eKermit.h"
	#include "util/CircularBuffer.h"
	#include "util/cdefs.h"
}*/

using namespace std;
using namespace Phoenix::Core;

namespace Phoenix
{
	namespace Servers
	{	
		/*! \brief COM message handlers. */
		static COMHSHandler * comHSHandler;
		static COMBeaconHandler * comBeacHandler;
		static COMDataSendHandler * comDataSendHandler;
		static COMLoginHandler * comLoginHandler;
		static COMLogoutHandler * comLogoutHandler;
		static COMDataReceiveHandler * comDataRecHandler;
		static COMResetHandler * comRstHandler;
		static COMErrorHandler * comErrorHandler;
		
		COMServer::COMServer(string nameIn, LocationIDType idIn)
				: SubsystemServer(nameIn, idIn), Singleton(), arby(idIn), COMCurrentState(BEACON_STATE), parseFlag(false), kermitRunFlag(false), beaconData()
		{			
			/*fileInputBuffer = new char[IBUFLEN + 8];
			fileOutputBuffer = new char[OBUFLEN + 8];
			inputBuffer = CreateCircularBuffer(COM_INPUT_BUFFER_LENGTH);*/
			//SetState(BEACON_STATE);
		}

		COMServer::~COMServer()
		{
			delete fileInputBuffer;
			delete fileOutputBuffer;
			//DestroyCircularBuffer(inputBuffer);
		}

		COMServer & COMServer::operator=(const COMServer & source)
		{
			if (this == &source)
			{
				return *this;
			}

			SubsystemServer::operator =(source);

			return *this;
		}

		void COMServer::Initialize(void)
		{
			//Initialize handlers
#ifdef WIN32
			if(comHSHandler == NULL)
				comHSHandler = new COMHSHandler();
			if(comBeacHandler == NULL)
				comBeacHandler = new COMBeaconHandler();
			if(comDataSendHandler == NULL)
				comDataSendHandler = new COMDataSendHandler();
			if(comLoginHandler == NULL)
				comLoginHandler = new COMLoginHandler();
			if(comLogoutHandler == NULL)
				comLogoutHandler = new COMLogoutHandler();
			if(comDataRecHandler == NULL)
				comDataRecHandler = new COMDataReceiveHandler();
			if(comRstHandler == NULL)
				comRstHandler = new COMResetHandler();
			if(comErrorHandler == NULL)
				comErrorHandler = new COMErrorHandler();
#else
			comHSHandler = new COMHSHandler();
			comBeacHandler = new COMBeaconHandler();
			comDataSendHandler = new COMDataSendHandler();
			comLoginHandler = new COMLoginHandler();
			comLogoutHandler = new COMLogoutHandler();
			comDataRecHandler = new COMDataReceiveHandler();
			comRstHandler = new COMResetHandler();
			comErrorHandler = new COMErrorHandler();
#endif
		}
		
#ifdef TEST
		void COMServer::Destroy(void)
		{
			//delete handlers
			delete comHSHandler;
			delete comBeacHandler;
			delete comDataSendHandler;
			delete comLoginHandler;
			delete comLogoutHandler;
			delete comDataRecHandler;
			delete comRstHandler;
			delete comErrorHandler;
		}
#endif
		
		bool COMServer::IsFullyInitialized(void)
		{
			return(Singleton::IsFullyInitialized());
		}

		void COMServer::SubsystemLoop(void)
		{
			ModeManager * modeManager = dynamic_cast<ModeManager *> (Factory::GetInstance(MODE_MANAGER_SINGLETON));
			const SystemMode * mode;
			SystemModeEnum modeIndex;

			modeArray[0] = &COMServer::COMAccessMode;
			/*modeArray[1] = &COMServer::COMStartupMode;
			modeArray[2] = &COMServer::COMBusMode;
			modeArray[3] = &COMServer::COMPayloadMode;
			modeArray[4] = &COMServer::COMErrorMode;
			modeArray[5] = &COMServer::COMComMode;*/

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

		void COMServer::Update(const SystemMode * mode)
		{
			//update mode stuff!
		}

        bool COMServer::RegisterHandlers()
        {
        	volatile bool success = true;
        	Dispatcher * dispatcher = dynamic_cast<Dispatcher *> (Factory::GetInstance(DISPATCHER_SINGLETON));	

        	// COM Command OpCodes
			success &= reg.RegisterHandler(MessageIdentifierType(MESSAGE_TYPE_COMMAND, COM_HS_CMD), comHSHandler);
        	success &= reg.RegisterHandler(MessageIdentifierType(MESSAGE_TYPE_COMMAND, COM_BEACON_CMD), comBeacHandler);
        	success &= reg.RegisterHandler(MessageIdentifierType(MESSAGE_TYPE_COMMAND, COM_DATA_SEND_CMD), comDataSendHandler);
			success &= reg.RegisterHandler(MessageIdentifierType(MESSAGE_TYPE_COMMAND, COM_LOGOUT_CMD), comLogoutHandler);
        	success &= reg.RegisterHandler(MessageIdentifierType(MESSAGE_TYPE_COMMAND, COM_RESET_CMD), comRstHandler);
        	
			success &= reg.RegisterHandler(MessageIdentifierType(MESSAGE_TYPE_COMMAND, COM_LOGIN_CMD), comLoginHandler);
			success &= reg.RegisterHandler(MessageIdentifierType(MESSAGE_TYPE_COMMAND, COM_GND_LOGGED_OUT_CMD), comLogoutHandler);
			success &= reg.RegisterHandler(MessageIdentifierType(MESSAGE_TYPE_ERROR, COM_CONNECTION_LOST_CMD), comLogoutHandler);
			success &= reg.RegisterHandler(MessageIdentifierType(MESSAGE_TYPE_DATA, COM_DATA_RECEIVE_CMD), comDataRecHandler);

        	success &= reg.RegisterHandler(MessageIdentifierType(MESSAGE_TYPE_ERROR, COM_PASSWORD_REFUSED_CMD), comErrorHandler);

        	// COM Command OpCodes permissions
			for(int opcode = COM_CMD_MIN; opcode < COM_CMD_MAX; opcode++)
			{
				success &= arby.ModifyPermission(MessageIdentifierType(MESSAGE_TYPE_COMMAND, opcode), true);
				success &= arby.ModifyPermission(MessageIdentifierType(MESSAGE_TYPE_DATA, opcode), true);
				success &= arby.ModifyPermission(MessageIdentifierType(MESSAGE_TYPE_ERROR, opcode), true);
			}
			
        	success &= arby.ModifyPermission(MessageIdentifierType(MESSAGE_TYPE_ERROR, COM_PASSWORD_REFUSED_CMD), true);

        	success &= dispatcher->AddRegistry(id, &reg, &arby);

        	return success;
        }

		void COMServer::COMAccessMode(ModeManager * modeManager)
		{
			Dispatcher * dispatcher = dynamic_cast<Dispatcher *> (Factory::GetInstance(DISPATCHER_SINGLETON));
			//WatchdogManager * wdm = dynamic_cast<WatchdogManager *> (Factory::GetInstance(WATCHDOG_MANAGER_SINGLETON));
			
			const SystemMode * mode = AccessMode::GetInstance();
			const SystemMode * currentMode = mode;
			
			uint32 timeUnit = 0;

			ReturnMessage * HSRet;
			ReturnMessage * BCNRet;
			ReturnMessage * DATRet;

			while(mode == currentMode)
			{
				uint64_t LastTimeTick = getTimeInMilis();
				//wdm->Kick();	
				
				//DEBUG_COUT("		COMServer::Calling listen with id: " << id)
				while(dispatcher->Listen(id));
				
				timeUnit++;
				
				switch(COMCurrentState)
				{
					case BEACON_STATE:
						//debug_led_set_led(0, LED_TOGGLE);
						if ((timeUnit % 30) == 0 )
						{
							//BCNRet = COMBeacon();
							//delete BCNRet;
							MessageProcess(SERVER_LOCATION_COM, BCNRet);
						}
						if ((timeUnit % 200) == 0 )
						{
							HSRet = COMHealthStatus();
							//delete HSRet;
							MessageProcess(SERVER_LOCATION_COM, HSRet);
							timeUnit = 0;
						}
						break;
						
					case COMMAND_STATE:

						//debug_led_set_led(1, LED_TOGGLE);

						if(parseFlag)
						{
							/*if(!ParseGndCommand(id))
							{
								//flush buffer
								DEBUG_COUT("COMServer: flushing buffer...")
								//FlushBuffer(inputBuffer);
							}*/
						}
						if ((timeUnit % 300) == 0 )
						{
							//HSRet = COMHealthStatus();
							//MessageProcess(SERVER_LOCATION_COM, HSRet);
							timeUnit = 0;
						}
						break;
						
					case SEND_STATE:
						//TODO:FIXME:REWRITE COM t.t
						//debug_led_set_led(2, LED_TOGGLE);

						// Check if the send task is done
						/*if (true == xSemaphoreTake(&(sendParams->doneSem), 0))
						{
							DEBUG_COUT("Kermit exited send mode...")
							DELETE_TASK(*sendTaskHandle);
							vQueueDelete(sendParams->syncSem);
							vQueueDelete(sendParams->doneSem);
							for(int i = 0; (i < MAX_DOWNLINK_FILES) && (sendParams->fileList[i] != NULL); i++) {
								delete sendParams->fileList[i];
							}
							delete sendParams->fileList;
							delete sendTaskHandle;
							delete sendParams;
							
							//flush buffer
							FlushBuffer(inputBuffer);
							SetKermitRunFlag(false);
							SetState(COMMAND_STATE);
						}*/
						break;
						
					case RECEIVE_STATE:
			/*			debug_led_set_led(3, LED_TOGGLE);
						// Check if the receive task is done
						if (pdTRUE == xSemaphoreTake(recParams->doneSem, 0))
						{
							DEBUG_COUT("Kermit exited receive mode...")
							DELETE_TASK(*recTaskHandle);
							vQueueDelete(recParams->syncSem);
							vQueueDelete(recParams->doneSem);
							delete recTaskHandle;
							delete recParams;
							
							//flush buffer
							FlushBuffer(inputBuffer);
							SetKermitRunFlag(false);
							SetState(COMMAND_STATE);
						}*/
						break;
				}
				
				// Delay
				//TODO: why is this 100ms and not 1000?
				waitUntil(LastTimeTick, 1000);

				// Check current mode
				currentMode = modeManager->GetMode();
			}
		}

		/*void COMServer::COMStartupMode(ModeManager * modeManager)
		{
			Dispatcher * dispatcher = dynamic_cast<Dispatcher *> (Factory::GetInstance(DISPATCHER_SINGLETON));
			//WatchdogManager * wdm = dynamic_cast<WatchdogManager *> (Factory::GetInstance(WATCHDOG_MANAGER_SINGLETON));
			const SystemMode * mode = StartupMode::GetInstance();
			const SystemMode * currentMode = mode;

			//Not beaconing in startup mode.

			while(mode == currentMode)
			{
				uint64_t LastTimeTick = getTimeInMilis();
				//wdm->Kick();
				while(dispatcher->Listen(id));

				// Delay
				waitUntil(LastTimeTick, 1000);

				// Check current mode
				currentMode = modeManager->GetMode();
			}
		}

		void COMServer::COMBusMode(ModeManager * modeManager)
		{
			Dispatcher * dispatcher = dynamic_cast<Dispatcher *> (Factory::GetInstance(DISPATCHER_SINGLETON));
			//WatchdogManager * wdm = dynamic_cast<WatchdogManager *> (Factory::GetInstance(WATCHDOG_MANAGER_SINGLETON));
			const SystemMode * mode = BusPriorityMode::GetInstance();
			const SystemMode * currentMode = mode;
			uint8 seconds = 0;

			ReturnMessage * HSRet;
			ReturnMessage * BCNRet;

			while(mode == currentMode)
			{
				uint64_t LastTimeTick = getTimeInMilis();
				//wdm->Kick();
				while(dispatcher->Listen(id));

				if ((seconds % 10) == 0 )
				{
					// Functions

					//HSRet = COMHealthStatus();
					//MessageProcess(SERVER_LOCATION_COM, HSRet);
				}
				if((seconds % 30) == 0)
				{
					if (COMCurrentState == BEACON_STATE)
					{
						BCNRet = COMBeacon();
						MessageProcess(SERVER_LOCATION_COM, BCNRet);
					}
					seconds = 0;
				}

				seconds++;
				// Delay
				waitUntil(LastTimeTick, 1000);

				// Check current mode
				currentMode = modeManager->GetMode();
			}
		}

		void COMServer::COMPayloadMode(ModeManager * modeManager)
		{
			Dispatcher * dispatcher = dynamic_cast<Dispatcher *> (Factory::GetInstance(DISPATCHER_SINGLETON));
			//WatchdogManager * wdm = dynamic_cast<WatchdogManager *> (Factory::GetInstance(WATCHDOG_MANAGER_SINGLETON));
			const SystemMode * mode = PayloadPriorityMode::GetInstance();
			const SystemMode * currentMode = mode;
			uint8 seconds = 0;

			ReturnMessage * HSRet;
			ReturnMessage * BCNRet;

			while(mode == currentMode)
			{
				uint64_t LastTimeTick = getTimeInMilis();
				//wdm->Kick();
				while(dispatcher->Listen(id));

				if ((seconds % 10) == 0 )
				{
					// Functions

					HSRet = COMHealthStatus();
					MessageProcess(SERVER_LOCATION_COM, HSRet);
				}
				if((seconds % 30) == 0)
				{
					if (COMCurrentState == BEACON_STATE)
					{
						//BCNRet = COMBeacon();
						//MessageProcess(SERVER_LOCATION_COM, BCNRet);
					}
					seconds = 0;
				}
				seconds++;
				// Delay
				waitUntil(LastTimeTick, 1000);

				// Check current mode
				currentMode = modeManager->GetMode();
			}
		}

		void COMServer::COMErrorMode(ModeManager * modeManager)
		{
			Dispatcher * dispatcher = dynamic_cast<Dispatcher *> (Factory::GetInstance(DISPATCHER_SINGLETON));
			//WatchdogManager * wdm = dynamic_cast<WatchdogManager *> (Factory::GetInstance(WATCHDOG_MANAGER_SINGLETON));
			const SystemMode * mode = ErrorMode::GetInstance();
			const SystemMode * currentMode = mode;
			uint8 seconds = 0;

			ReturnMessage * HSRet;
			ReturnMessage * BCNRet;

			while(mode == currentMode)
			{
				uint64_t LastTimeTick = getTimeInMilis();
				//wdm->Kick();
				while(dispatcher->Listen(id));

				if ((seconds % 10) == 0 )
				{
					// Functions

					HSRet = COMHealthStatus();
					MessageProcess(SERVER_LOCATION_COM, HSRet);
				}
				if((seconds % 30) == 0)
				{
					if (COMCurrentState == BEACON_STATE)
					{
						//BCNRet = COMBeacon();
						//MessageProcess(SERVER_LOCATION_COM, BCNRet);
					}
					seconds = 0;
				}
				seconds++;
				// Delay
				waitUntil(LastTimeTick, 1000);

				// Check current mode
				currentMode = modeManager->GetMode();
			}
		}

		void COMServer::COMComMode(ModeManager * modeManager)
		{
			Dispatcher * dispatcher = dynamic_cast<Dispatcher *> (Factory::GetInstance(DISPATCHER_SINGLETON));
			//WatchdogManager * wdm = dynamic_cast<WatchdogManager *> (Factory::GetInstance(WATCHDOG_MANAGER_SINGLETON));
			const SystemMode * mode = ComMode::GetInstance();
			const SystemMode * currentMode = mode;
			uint8 seconds = 0;

			ReturnMessage * BCNRet;
			ReturnMessage * HSRet;

			while(mode == currentMode)
			{
				uint64_t LastTimeTick = getTimeInMilis();
				//wdm->Kick();
				while(dispatcher->Listen(id));

				switch(COMCurrentState)
				{
					case BEACON_STATE:
						if ((seconds % 5) == 0 )
						{
							BCNRet = COMBeacon();
							MessageProcess(SERVER_LOCATION_COM, BCNRet);
						}
						if ((seconds % 30) == 0 )
						{
							HSRet = COMHealthStatus();
							MessageProcess(SERVER_LOCATION_COM, HSRet);
							seconds = 0;
						}
						break;
						
					case COMMAND_STATE:
						if(parseFlag)
						{
							if(!ParseGndCommand(id))
							{
								//flush buffer
								DEBUG_COUT("COMServer: flushing buffer...")
								FlushBuffer(inputBuffer);
							}
						}
						if ((seconds % 30) == 0 )
						{
							HSRet = COMHealthStatus();
							MessageProcess(SERVER_LOCATION_COM, HSRet);
							seconds = 0;
						}
						break;
						
					case SEND_STATE:
						// Check if the send task is done
						if (pdTRUE == xSemaphoreTake(sendParams->doneSem, KERMIT_MAX_DELAY))
						{
							DEBUG_COUT("Kermit exited send mode...")
							DELETE_TASK(*sendTaskHandle);
							vQueueDelete(sendParams->syncSem);
							vQueueDelete(sendParams->doneSem);
							for(int i = 0; i < MAX_DOWNLINK_FILES; i++) {
								delete sendParams->fileList[i];
							}
							delete sendParams->fileList;
							delete sendTaskHandle;
							delete sendParams;
							
							//flush buffer
							FlushBuffer(inputBuffer);
							SetKermitRunFlag(false);
							SetState(COMMAND_STATE);
						}
						break;
						
					case RECEIVE_STATE:
						// Check if the receive task is done
						if (pdTRUE == xSemaphoreTake(recParams->doneSem, KERMIT_MAX_DELAY))
						{
							DEBUG_COUT("Kermit exited receive mode...")
							DELETE_TASK(*recTaskHandle);
							vQueueDelete(recParams->syncSem);
							vQueueDelete(recParams->doneSem);
							delete recTaskHandle;
							delete recParams;
							
							//flush buffer
							FlushBuffer(inputBuffer);
							SetKermitRunFlag(false);
							SetState(COMMAND_STATE);
						}
						break;
				}
				
				seconds++;
				// Delay
				waitUntil(LastTimeTick);

				// Check current mode
				currentMode = modeManager->GetMode();
			}
		}

		bool COMServer::SetState(COMStateEnum nextState)
		{
#ifdef WIN32
			switch(id)
			{
				case SERVER_LOCATION_COM:
					DEBUG_COUT("		COMServer:: SetState called for space com server...");
					break;
				case SERVER_LOCATION_GND_COM:
					DEBUG_COUT("		COMServer:: SetState called for ground com server...");
					break;
				default:
					DEBUG_COUT("		COMServer:: SetState called for default...");
					break;
			}
#endif
			
			if(pdTRUE == this->TakeLock(portMAX_DELAY))
			{
				switch(nextState)
				{
				case BEACON_STATE:
					DEBUG_COUT("		COMServer:: Beacon State set...");
					COMCurrentState = nextState;
					this->GiveLock();
					return true;

				case COMMAND_STATE:
					DEBUG_COUT("		COMServer:: Command State set...");
					COMCurrentState = nextState;
					this->GiveLock();
					return true;

				case SEND_STATE:
					DEBUG_COUT("		COMServer:: Send State set...");
					COMCurrentState = nextState;
					this->GiveLock();
					return true;

				case RECEIVE_STATE:
					DEBUG_COUT("		COMServer:: Receive State set...");
					COMCurrentState = nextState;
					this->GiveLock();
					return true;

				default:
					DEBUG_COUT("		COMServer:: default...");
					this->GiveLock();
					return false;
				}
			}
			return false;
		}

		COMStateEnum COMServer::GetState()
		{
			if(pdTRUE == this->TakeLock(portMAX_DELAY))
			{
				this->GiveLock();
				return COMCurrentState;
			}
			//TODO: what the hell
			return BEACON_STATE;
		}

		bool COMServer::SetParseFlag()
		{
			if(pdTRUE == this->TakeLock(portMAX_DELAY))
			{
				if(!parseFlag)
				{
					parseFlag = true;
					this->GiveLock();
					return true;
				}
				this->GiveLock();
				return false;
			}
			return false;
		}
		
		bool COMServer::SetSendTaskHandle(xTaskHandle * sendHandle)
		{
			if(pdTRUE == this->TakeLock(portMAX_DELAY))
			{
				sendTaskHandle = sendHandle;
				this->GiveLock();
				return true;
			}
			return false;
		}
		
		bool COMServer::SetReceiveTaskHandle(xTaskHandle * recHandle)
		{
			if(pdTRUE == this->TakeLock(portMAX_DELAY))
			{
				recTaskHandle = recHandle;
				this->GiveLock();
				return true;
			}
			return false;
		}
		
		bool COMServer::SetSendParameters(KermitSendParameter * sendParameters)
		{
			if(pdTRUE == this->TakeLock(portMAX_DELAY))
			{
				sendParams = sendParameters;
				this->GiveLock();
				return true;
			}
			return false;
		}
		
		bool COMServer::SetReceiveParameters(KermitReceiveParameter * recParameters)
		{
			if(pdTRUE == this->TakeLock(portMAX_DELAY))
			{
				recParams = recParameters;
				this->GiveLock();
				return true;
			}
			return false;
		}
		
		CircularBuffer * COMServer::GetInputBuffer(void)
		{
			if(pdTRUE == this->TakeLock(portMAX_DELAY))
			{
				this->GiveLock();
				return inputBuffer;
			}
			return NULL;
		}
		
		char * COMServer::GetFileInputBuffer(void)
		{
			if(pdTRUE == this->TakeLock(portMAX_DELAY))
			{
				this->GiveLock();
				return fileInputBuffer;
			}
			return NULL;
		}
		
		char * COMServer::GetFileOuputBuffer(void)
		{
			if(pdTRUE == this->TakeLock(portMAX_DELAY))
			{
				this->GiveLock();
				return fileOutputBuffer;
			}
			return NULL;
		}
		
		bool COMServer::SetKermitRunFlag(bool state)
		{
			if(pdTRUE == this->TakeLock(portMAX_DELAY))
			{
				kermitRunFlag = state;
				this->GiveLock();
				return true;
			}
			return false;
		}
		
		bool COMServer::GetKermitRunFlag(void)
		{
			if(pdTRUE == this->TakeLock(portMAX_DELAY))
			{
				this->GiveLock();
				return kermitRunFlag;
			}
			return false;
		}
		*/
//		BeaconData * COMServer::GetBeaconHolder(void)
//		{
//			if(pdTRUE == this->TakeLock(portMAX_DELAY))
//			{
//				this->GiveLock();
//				return beaconData;
//			}
//		}
///*
//		void COMServer::UpdateACSHS()
//		{
//			if(pdTrue == this->TakeLock(0))
//			{
//				beaconData-> acsParam1 = acsParam; // populate w/ ACS H&S
//
//				this->GiveLock();
//				return;
//			}
//		}
//*/
//		void COMServer::UpdateEPSHS(int16 battVoltage, int16 battCurrent, uint16 stateOfCharge, int16 battRemainingCapacity,
//			int16 cycleCount, int16 batteryManagerStatus)
//		{
//			if(pdTRUE == this->TakeLock(0))
//			{
//				beaconData->epsBattVoltage				= battVoltage;
//				beaconData->epsBattCurrent				= battCurrent;
//				beaconData->epsStateOfCharge			= stateOfCharge;
//				beaconData->epsBattRemainingCapacity	= battRemainingCapacity;
//				beaconData->epsCycleCount				= cycleCount;
//				beaconData->epsBatteryManagerStatus		= batteryManagerStatus;
//				this->GiveLock();
//				return;
//			}
//		}
//
//		// parameters outdated - update types from COM H&S
//		void COMServer::UpdateCOMHS(uint16 lwipPacketsTx,uint16 lwipPacketsRx,uint16 lwipPacketsDrop,uint16 lwipPacketsErrors,
//				uint16 lwipEtharpTx,uint16 lwipEtharpRx,uint16 lwipEtharpDrop,uint16 lwipEtharpErrors,
//				uint16 lwipIPTx,uint16 lwipIPRx,uint16 lwipIPDrop,uint16 lwipIPErrors,
//				uint16 lwipUDPTx,uint16 lwipUDPRx,uint16 lwipUDPDrop,uint16 lwipUDPErrors,
//				uint16 lwipTCPTx,uint16 lwipTCPRx,uint16 lwipTCPDrop,uint16 lwipTCPErrors,
//				uint16 lwipMemAvail,uint16 lwipMemUsed,uint16 lwipMemMax,uint16 lwipMemErrors,uint16 lwipMemIllegal,
//				uint16 comState,uint16 statBits, uint16 signalStrength,
//				uint8 transmitPower, uint8 RXIFDetect, uint8 RXGain, uint8 currentSense, uint8 voltageSense)
//		{
//			if(pdTRUE == this->TakeLock(0))
//			{
//				/*
//				beaconData-> comlwipPacketsTx		= lwipPacketsTx;
//				beaconData-> comlwipPacketsRx		= lwipPacketsRx;
//				beaconData-> comlwipPacketsDrop		= lwipPacketsDrop;
//				beaconData-> comlwipPacketsErrors	= lwipPacketsErrors;
//
//				// ETHARP Status
//				beaconData-> comlwipEtharpTx		= lwipEtharpTx;
//				beaconData-> comlwipEtharpRx		= lwipEtharpRx;
//				beaconData-> comlwipEtharpDrop		= lwipEtharpDrop;
//				beaconData-> comlwipEtharpErrors	= lwipEtharpErrors;
//
//				// IP Status
//				beaconData-> comlwipIPTx		= lwipIPTx;
//				beaconData-> comlwipIPRx		= lwipIPRx;
//				beaconData-> comlwipIPDrop		= lwipIPDrop;
//				beaconData-> comlwipIPErrors	= lwipIPErrors;
//
//				// UDP Status
//				beaconData-> comlwipUDPTx		= lwipUDPTx;
//				beaconData-> comlwipUDPRx		= lwipUDPRx;
//				beaconData-> comlwipUDPDrop		= lwipUDPDrop;
//				beaconData-> comlwipUDPErrors	= lwipUDPErrors;
//
//				// TCP Status
//				beaconData-> comlwipTCPTx		= lwipTCPTx;
//				beaconData-> comlwipTCPRx		= lwipTCPRx;
//				beaconData-> comlwipTCPDrop		= lwipTCPDrop;
//				beaconData-> comlwipTCPErrors	= lwipTCPErrors;
//
//				// Memory Status
//				beaconData-> comlwipMemAvail	= lwipMemAvail;
//				beaconData-> comlwipMemUsed		= lwipMemUsed;
//				beaconData-> comlwipMemMax		= lwipMemMax;
//				beaconData-> comlwipMemErrors	= lwipMemErrors;
//				beaconData-> comlwipMemIllegal	= lwipMemIllegal;
//
//				beaconData-> comState			= comState;
//				beaconData-> comstatBits		= statBits;
//				beaconData-> comSignalStrength	= signalStrength;
//				beaconData-> comTransmitPower	= transmitPower;
//				beaconData-> comRXIFDetect		= RXIFDetect;
//				beaconData-> comRXGain			= RXGain;
//				beaconData-> com5VCurrentSense	= currentSense;
//				beaconData-> com5VVoltageSense	= voltageSense;
//				*/
//				this->GiveLock();
//				return;
//			}
//		}
//
//		// parameters outdated - update types from GPS H&S
//		void COMServer::UpdateGPSHS(uint8_t flags, float posX, float posY, float posZ, float velX, float velY,
//			float velZ, float solutionAge)
//		{
//			if(true == this->TakeLock(0))
//			{
//				beaconData->gpsFlags = flags;
//				beaconData->gpsPosX	= posX;
//				beaconData->gpsPosY = posY;
//				beaconData->gpsPosZ = posZ;
//				beaconData->gpsVelX = velX;
//				beaconData->gpsVelY = velY;
//				beaconData->gpsVelZ = velZ;
//				beaconData->gpsSolutionAge = solutionAge;
//				this->GiveLock();
//				return;
//			}
//		}
	}
}


