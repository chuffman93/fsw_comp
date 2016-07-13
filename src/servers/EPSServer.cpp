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
				: SubsystemServer(nameIn, idIn), Singleton(), arby(idIn)
		{
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

		void EPSServer::Update(SystemModeEnum mode)
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

		// -------------------------------------------- State Machine ---------------------------------------------
		void EPSServer::loopInit(){
			currentState = ST_MONITOR;
		}

		void EPSServer::loopMonitor(){
			Dispatcher * dispatcher = dynamic_cast<Dispatcher *> (Factory::GetInstance(DISPATCHER_SINGLETON));
			Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));

#ifdef HARDWARE
			FSWPacket * HSRet = EPSHealthStat();
			MessageProcess(SERVER_LOCATION_EPS, HSRet);

			FSWPacket * SOCRet = EPSStateOfCharge();
			MessageProcess(SERVER_LOCATION_EPS, SOCRet);
#endif //HARDWARE
		}

	} // End Namespace servers
} // End Namespace Phoenix
