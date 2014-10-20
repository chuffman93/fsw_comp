/*
* EPSServer.h
*
*  Created on: Feb 21, 2011
*      Author: Michael
*  Modified: November 11, 2012
*/

#ifndef EPSSERVER_H_
#define EPSSERVER_H_

#include "servers/SubsystemServer.h"
#include "servers/EPSHandlers.h"

#include "core/StdTypes.h"

#include "core/MessageHandlerRegistry.h"
#include "core/Arbitrator.h"
#include "core/ModeManager.h"
#include "core/Dispatcher.h"

#include "util/FileHandler.h"

//#include "boards/board.h"

namespace Phoenix
{
	namespace Servers
	{
		/*! \brief Wait time in seconds before deployment */
		//#define INHIBIT_WAIT_TIME					2700 // 45 (minutes) * 60 (Seconds)
		#define INHIBIT_WAIT_TIME					2700
		
		/*! \brief Wait time in seconds while waiting for star camera lock */
		#define STAR_CAMERA_LOCK_WAIT_TIME			300 // 5 (minutes) * 60 (Seconds)
		
		/*! \brief Burn time in seconds for each burn of the Frangi Bolt */
		#define DEPLOYMENT_BURN_TIME				90
		
		/*! \brief Delay between burns of the Frangi Bolt */
		#define DEPLOYMENT_WAIT_TIME				90
		
		#define STAR_CAMERA_STARS_THRESHOLD			10
		
		#define STAR_CAMERA_LIGHT_THRESHOLD			2.0
		
		#define ACS_MAX_OFF_TIME					60000
		#define COM_MAX_OFF_TIME					60000
		#define GPS_MAX_OFF_TIME					60000
		#define PLD_MAX_OFF_TIME					60000
		#define PROP_MAX_OFF_TIME					60000      
		
		class EPSServer : public SubsystemServer, public Phoenix::Core::Singleton
		{
			/*! \brief Declare Factory a friend class
			*
			*	This allows factory to call EPSServer's private constructor
			*/
			friend class Phoenix::Core::Factory;
			
		public:
			
			void SubsystemLoop(void);
			void Update(const Core::SystemMode * mode);

			/*! \brief Registers this server's message handlers.
			*
			*  \note Dispatcher must be initialized before this
			*      function can be called.
			*
			*  Registers any message handlers with the member
			*  MessageHandlerRegistry and modifies permissions
			*  with the member Arbitrator. Then those two members
			*  are added to the Dispatcher.
			*/

			bool RegisterHandlers();

			bool getPowerSate(PowerSubsystemEnum subsystem);
			uint32 getPowerOffTimer(PowerSubsystemEnum subsystem);
			void setPowerOffTimer(PowerSubsystemEnum subsystem, uint32 time);
			void setPowerState(PowerSubsystemEnum subsystem, bool state);
			
			uint32 ACSOffTimer;
			uint32 PLDOffTimer;
			uint32 GPSOffTimer;
			uint32 COMOffTimer;
			uint32 PROPOffTimer;
		private:
			/*! \brief Initialize the EPSServer Class
			*
			*  Initializes the operating system constructs needed for
			*  EPSServer to work properly.
			*/
			static void Initialize(void);
			
			/*! \brief Static Destructor for EPSServer
			*
			*  Frees all internal memory use, frees all operating system
			*  objects used, and deletes the singleton instance pointer.
			*/
#ifdef HOST
			static void Destroy(void);
#endif
			
			/*! \brief Checks if EPSServer Class is initialized
			*
			*  \return true if the initialization was successful and
			*  false otherwise.
			*/
			bool IsFullyInitialized(void);
			
			EPSServer(std::string nameIn, LocationIDType idIn); 
			~EPSServer();
			EPSServer & operator=(const EPSServer & source);
			
			// Member variables needed to register message handlers.
			Phoenix::Core::MessageHandlerRegistry reg;
			Phoenix::Core::Arbitrator arby;
			
			float sensorThreshhold;

			void EPSAccessMode(Phoenix::Core::ModeManager * modeManager);		// 0
			void EPSStartupMode(Phoenix::Core::ModeManager * modeManager);		// 1
			void EPSBusMode(Phoenix::Core::ModeManager * modeManager);			// 2
			void EPSPayloadMode(Phoenix::Core::ModeManager * modeManager);		// 3
			void EPSErrorMode(Phoenix::Core::ModeManager * modeManager);		// 4
			void EPSComMode(Phoenix::Core::ModeManager * modeManager);			// 5
			
			void(EPSServer::*modeArray[6])(Phoenix::Core::ModeManager * modeManager);
			
			// Data regarding on/off state of each subsystem and max shutoff time
            bool powerStateACS;
			bool powerStatePLD;
			bool powerStateGPS;
			bool powerStateCOM;
			bool powerStatePROP;
			
		};
	}
}

#endif /* EPSSERVER_H_ */
