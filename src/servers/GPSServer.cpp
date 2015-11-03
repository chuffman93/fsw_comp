/*
* GPSServer.cpp
*
* Created: 7/6/12
* Written by: Conrad Hougen
*/

//GPS frame rotation files
/*
extern "C"
{
#include "util/gpsFrameRotation.h"
#include "util/IAU2006_XYS.h"
#include "util/leapSecond.h"
#include "util/vector3D.h"
}
*/

#include "servers/GPSServer.h"
#include "servers/GPSHandlers.h"
#include "servers/GPSStdTasks.h"
#include "servers/DispatchStdTasks.h"

#include "core/CommandMessage.h"
#include "core/ReturnMessage.h"
#include "core/Singleton.h"
#include "core/Factory.h"
#include "core/ModeManager.h"
#include "core/WatchdogManager.h"
#include "core/ErrorMessage.h"
#include "core/StdTypes.h"

#include "core/ErrorMessage.h"
#include "core/AccessMode.h"
#include "core/StartupMode.h"
#include "core/BusPriorityMode.h"
#include "core/PayloadPriorityMode.h"
#include "core/ErrorMode.h"
#include "core/ComMode.h"
#include "core/SystemMode.h"

#include "util/FileHandler.h"

#include "HAL/GPS.h"
#include "HAL/RTC.h"

//#include "boards/backplane/dbg_led.h"

using namespace std;
using namespace Phoenix::Core;
using namespace Phoenix::HAL;

namespace Phoenix
{
	namespace Servers
	{
		// handler for Health and status measurements
		static GPSHealthAndStatusHandler * gpsHSHandler;
		// GPS date/time handler
		static GPSTimeHandler * gpsTimeHandler;
		// handler for position requests
		static GPSPositionHandler * gpsPosHandler;
		// handler for position requests
		static GPSResetHandler * gpsRstHandler;
		// handler for errors
		static GPSErrorHandler * gpsErrorHandler;
		
		GPSServer::GPSServer(string nameIn, LocationIDType idIn)
				: SubsystemServer(nameIn, idIn), Singleton(), arby(idIn)
		{
			GPSDataHolder = new GPSData();
		}

		GPSServer::~GPSServer()
		{
			// Left Intentionally Blank
		}

		GPSServer & GPSServer::operator=(const GPSServer & source)
		{
			if (this == &source)
			{
				return *this;
			}

			SubsystemServer::operator =(source);

			return *this;
		}
		
		void GPSServer::Initialize(void)
		{
			gpsHSHandler = new GPSHealthAndStatusHandler();
			gpsTimeHandler = new GPSTimeHandler;
			gpsPosHandler = new GPSPositionHandler();
			gpsRstHandler = new GPSResetHandler;
			gpsErrorHandler = new GPSErrorHandler;
		}
		
#ifdef TEST
		void GPSServer::Destroy(void)
		{
			delete gpsHSHandler;
			delete gpsTimeHandler;
			delete gpsPosHandler;
			delete gpsRstHandler;
			delete gpsErrorHandler;
		}
#endif
		
		bool GPSServer::IsFullyInitialized(void)
		{
			return(Singleton::IsFullyInitialized());
		}
		
		void GPSServer::Update(const SystemMode * mode)
		{

		}

		bool GPSServer::RegisterHandlers()
		{
			bool success = true;

			Dispatcher * dispatcher = dynamic_cast<Dispatcher *> (Factory::GetInstance(DISPATCHER_SINGLETON));

			success &= reg.RegisterHandler(MessageIdentifierType(MESSAGE_TYPE_COMMAND, GPS_HS_CMD), gpsHSHandler);
			success &= reg.RegisterHandler(MessageIdentifierType(MESSAGE_TYPE_COMMAND, GPS_TIME_CMD), gpsTimeHandler);
			success &= reg.RegisterHandler(MessageIdentifierType(MESSAGE_TYPE_COMMAND, GPS_POSITION_CMD), gpsPosHandler);
			success &= reg.RegisterHandler(MessageIdentifierType(MESSAGE_TYPE_COMMAND, GPS_RESET_CMD), gpsRstHandler);
			
			for(int opcode = GPS_CMD_MIN; opcode < GPS_CMD_MAX; opcode++)
			{
				success &= arby.ModifyPermission(MessageIdentifierType(MESSAGE_TYPE_COMMAND, opcode), true);
			}

			for(int opcode = GPS_SUB_ERROR_MIN; opcode < GPS_SUB_ERROR_MAX; opcode++)
			{
				success &= reg.RegisterHandler(MessageIdentifierType(MESSAGE_TYPE_ERROR, opcode), gpsErrorHandler);
				success &= arby.ModifyPermission(MessageIdentifierType(MESSAGE_TYPE_COMMAND, opcode), true);
			}

			success &= dispatcher->AddRegistry(id, &reg, &arby);

			return success;
		}

		void GPSServer::SubsystemLoop(void)
		{
			Dispatcher * dispatcher = dynamic_cast<Dispatcher *> (Factory::GetInstance(DISPATCHER_SINGLETON));
			ModeManager * modeManager = dynamic_cast<ModeManager *> (Factory::GetInstance(MODE_MANAGER_SINGLETON));
			//WatchdogManager * wdm = dynamic_cast<WatchdogManager *> (Factory::GetInstance(WATCHDOG_MANAGER_SINGLETON));
			//char GPSret[300] = "#BESTXYZA,COM1,0,0,FINESTEERING,1770,172800.000000,00000040,d821,2724;SOL_COMPUTED,NARROW_INT,135.177600,5664.273000,-3567.261000,0.000000,0.000000,0.000000,SOL_COMPUTED,NARROW_INT,-5.159300,2.912776,4.448210,0.000000,0.000000,0.000000,AAAA,0.000000,0.000000,0.000000,12,11,11,11,0,1,0,33*e9eafeca";
			char GPSBuffer[350] = {0};
			
			const SystemMode * mode;
			
			//Grab H&S, Time, and Position.
			uint32 input_size;

			while(1)
			{
				uint64_t LastWakeTime = getTimeInMilis();
				//wdm->Kick();
				while(dispatcher->Listen(id));
				
				//debug_led_set_led(0, LED_TOGGLE);
				
// 				mode = modeManager->GetMode();
// 				if(mode == AccessMode::GetInstance())
// 				{
// 					debug_led_set_led(4, LED_TOGGLE);
// 				}
// 				else if(mode == BusPriorityMode::GetInstance())
// 				{
// 					debug_led_set_led(5, LED_TOGGLE);
// 				}
// 				else if(mode == ComMode::GetInstance())
// 				{
// 					debug_led_set_led(7, LED_TOGGLE);
// 				}
				//TODO::GETGPSDATA
/*				if(!GetGPSData(LOG, POSITION, (uint8 *) GPSBuffer, &input_size))
				{
					//Some Error Stuff
					input_size = 0;
				}*/
				if(input_size > 200)
				{
					
					GPSDataProcess((char *) GPSBuffer, input_size);
				}
				input_size = 0;

				
				waitUntil(LastWakeTime, 1000);
			}

		}
		
		bool GPSServer::SetGPSData(GPSData * gpsData)
		{
			if(true == this->TakeLock(MAX_BLOCK_TIME))
			{
				GPSDataHolder = gpsData;
				this->GiveLock();
				return true;
			}
			return false;
		}
		
		GPSData * GPSServer::GetGPSDataPtr(void)
		{
			if(true == this->TakeLock(MAX_BLOCK_TIME))
			{
				this->GiveLock();
				return GPSDataHolder;
			}
			return NULL;
		}
	}
}
