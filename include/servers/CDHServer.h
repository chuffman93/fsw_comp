/*
 * CDHServer.h
 *
 *  Created on: Mar 4, 2016
 *      Author: Alex St. Clair
 */

#ifndef CDHSERVER_H_
#define CDHSERVER_H_

#include "core/StdTypes.h"
#include "core/Dispatcher.h"
#include "core/MessageHandlerRegistry.h"
#include "core/Arbitrator.h"
#include "core/MessageHandler.h"
#include "core/Singleton.h"
#include "core/VariableTypeEnum.h"
#include "core/ModeManager.h"

#include "servers/SubsystemServer.h"

#include <sys/sysinfo.h>
#include <sys/statvfs.h>
#include "HAL/I2C/HotSwaps.h"
#include "HAL/I2C/PowerMonitor.h"
#include "HAL/I2C/I2CDeviceManager.h"

using namespace Phoenix::HAL;

namespace Phoenix
{
	namespace Servers
	{
	class CDHServer : public SubsystemServer, public Phoenix::Core::Singleton
		{
			/*! \brief Declare Factory a friend class
			*
			*	This allows factory to call CDHServer's private constructor
			*/
			friend class Phoenix::Core::Factory;

		public:
			void Update(SystemModeEnum mode);

			bool RegisterHandlers();

			void SubsystemLoop();

			// Turn a subsystem on  (only ACS, COM, PLD, GPS)
			void subPowerOn(HardwareLocationIDType subsystem);

			// Turn a subsystem off (only ACS, COM, PLD, GPS)
			void subPowerOff(HardwareLocationIDType subsystem);

			//control the reset lines (ACS, COM, PLD, EPS only)
			void resetAssert(HardwareLocationIDType subsystem);
			void resetDeassert(HardwareLocationIDType subsystem);

			// Allows for easy look into memory usage
			struct sysinfo si;
			struct statvfs svfs;
			I2CDeviceManager * devMan;

		private:
			static void Initialize(void);
#ifdef TEST
			static void Destroy(void);
#endif
			bool IsFullyInitialized(void);

			CDHServer(std::string nameIn, LocationIDType idIn);
			~CDHServer();
			CDHServer & operator=(const CDHServer & source);

			// Member variables needed to register message handlers.
			Phoenix::Core::MessageHandlerRegistry reg;
			Phoenix::Core::Arbitrator arby;

			// Modes
			void loopInit();
			void loopMonitor();

			// Gather CDH data at a given frequency
			void readHealth(uint8 frequency, uint8 timeUnit);

			BEGIN_STATE_MAP
				STATE_MAP_ENTRY(&CDHServer::loopInit)
				STATE_MAP_ENTRY(&CDHServer::loopMonitor)
			END_STATE_MAP

			enum CDH_States {
				ST_INIT = 0,
				ST_MONITOR
			};
		};
	}
}

#endif //CDHSERVER_H_
