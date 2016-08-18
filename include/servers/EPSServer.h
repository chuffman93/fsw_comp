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

namespace AllStar
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
		
		class EPSServer : public SubsystemServer, public AllStar::Core::Singleton
		{
			/*! \brief Declare Factory a friend class
			*
			*	This allows factory to call EPSServer's private constructor
			*/
			friend class AllStar::Core::Factory;
			
		public:
			void Update(SystemModeEnum mode);

			bool RegisterHandlers();
			
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
#ifdef TEST
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
			AllStar::Core::MessageHandlerRegistry reg;
			AllStar::Core::Arbitrator arby;
			
			//Modes
			void loopInit();
			void loopMonitor();
			
			BEGIN_STATE_MAP
				STATE_MAP_ENTRY(&EPSServer::loopInit)
				STATE_MAP_ENTRY(&EPSServer::loopMonitor)
			END_STATE_MAP

			enum EPS_States{
				ST_INIT = 0,
				ST_MONITOR
			};

		};
	}
}

#endif /* EPSSERVER_H_ */
