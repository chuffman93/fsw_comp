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
#include "servers/SubsystemServer.h"
#include <sys/sysinfo.h>
#include <sys/statvfs.h>
#include "HAL/I2C/HotSwaps.h"
#include "HAL/I2C/PowerMonitor.h"

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
			HotSwap * hotSwaps[16];
			PowerMonitor * powerMonitors[4];

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

			HotSwap_Address adresses[16] = {COM_3V3_ADDR,COM_VBAT_ADDR,COM_12V0_ADDR,ACS_3V3_ADDR,ACS_VBAT_ADDR,ACS_12V0_ADDR,PROP_3V3_ADDR,PROP_VBAT_ADDR,PROP_12V0_ADDR,PLD_3V3_ADDR,PLD_VBAT_ADDR,PLD_12V0_ADDR,GPS_3V3_ADDR,GPS_VBAT_ADDR,AUXCOM_3V3_ADDR,AUXCOM_VBAT_ADDR};
			HotSwap_Fault faults[16] = {COM_3V3_FAULT,COM_VBAT_FAULT,COM_12V0_FAULT,ACS_3V3_FAULT,ACS_VBAT_FAULT,ACS_12V0_FAULT,PROP_3V3_FAULT,PROP_VBAT_FAULT,PROP_12V0_FAULT,PLD_3V3_FAULT,PLD_VBAT_FAULT,PLD_12V0_FAULT,GPS_3V3_FAULT,GPS_VBAT_FAULT,AUXCOM_3V3_FAULT,AUXCOM_VBAT_FAULT};
			float resistors[16] = {0.01,0.01,0.015,0.01,0.01,0.015,0.01,0.01,0.015,0.01,0.01,0.015,0.01,0.01,0.01,0.01};

			PowerMonitor_Address PM_adresses[4] = {A5_1V8_ADDR,A5_1V2_ADDR,ETH_2V5_ADDR,ETH_1V2_ADDR};
		};
	}
}

#endif //CDHSERVER_H_
