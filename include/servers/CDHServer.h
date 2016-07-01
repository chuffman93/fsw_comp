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
			void Update(const Core::SystemMode * mode);

			bool RegisterHandlers();

			void SubsystemLoop();

			/*! \brief Sets up and initializes hot swaps
			 *
			 * 	Creates arrays for hot swaps and their information
			 * 	Initializes each hot swap object
			 */
			void PrepHSPM(void);

			// Allows for easy look into memory usage
			struct sysinfo si;
			struct statvfs svfs;
//			HotSwap * hotSwaps[16];
//			PowerMonitor * powerMonitors[4];
			I2CDeviceManager * devMan;
		private:
			/*! \brief Initialize the CDHServer Class
			 *
			 *  Initializes the operating system constructs needed for
			 *  PLDServer to work properly.
			 */
			static void Initialize(void);

			/*! \brief Static Destructor for CDHServer
			 *
			 *  Frees all internal memory use, frees all operating system
			 *  objects used, and deletes the singleton instance pointer.
			 */
#ifdef TEST
			static void Destroy(void);
#endif

			/*! \brief Checks if CDHServer Class is initialized
			 *
			 *  \return true if the initialization was successful and
			 *  false otherwise.
			 */
			bool IsFullyInitialized(void);

			CDHServer(std::string nameIn, LocationIDType idIn);
			~CDHServer();
			CDHServer & operator=(const CDHServer & source);

			// Member variables needed to register message handlers.
			Phoenix::Core::MessageHandlerRegistry reg;
			Phoenix::Core::Arbitrator arby;

			void CDHAccessMode(Phoenix::Core::ModeManager * modeManager);
			void CDHBusMode(Phoenix::Core::ModeManager * modeManager);
			void(CDHServer::*modeArray[6])(Phoenix::Core::ModeManager * modeManager);

			void readHealth(uint8 frequency, uint8 timeUnit);

		};
	}
}

#endif //CDHSERVER_H_
