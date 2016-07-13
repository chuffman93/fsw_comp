/*
* ACSServer.h
*
*  Created on: Feb 21, 2011
*      Author: Michael
*/

#ifndef ACSSERVER_H_
#define ACSSERVER_H_

#include "core/StdTypes.h"
#include "servers/SubsystemServer.h"
#include "core/Dispatcher.h"
#include "core/MessageHandlerRegistry.h"
#include "core/Arbitrator.h"
#include "servers/ACSHandlers.h"
#include "core/ModeManager.h"
#include "core/Singleton.h"

namespace Phoenix
{
	namespace Servers
	{
		
		class ACSServer : public SubsystemServer, public Phoenix::Core::Singleton
		{
			/*! \brief Declare Factory a friend class
			*
			*	This allows factory to call ACSServer's private constructor
			*/
			friend class Phoenix::Core::Factory;
			
		public:
			void SubsystemLoop(void);
			void Update(SystemModeEnum mode);

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
			
			//Phoenix::Core::ReturnMessage * ACSTakeLock(void);
			//Phoenix::Core::ReturnMessage * ACSGiveLock(void);
			
			//void SetRunTest(bool toSet);

		private:
			bool runTest1;
		
			// Member variables needed to register message handlers.
			Phoenix::Core::MessageHandlerRegistry reg;
			Phoenix::Core::Arbitrator arby;
			
			// Targets to check range from
			//Targets except ground station
			float targetPos[MAX_SCH_TARGETS-1][3];
			
			/*! \brief Initialize the ACSServer Class
			*
			*  Initializes the operating system constructs needed for
			*  ACSServer to work properly.
			*/
			static void Initialize(void);
			
			/*! \brief Static Destructor for ACSServer
			*
			*  Frees all internal memory use, frees all operating system
			*  objects used, and deletes the singleton instance pointer.
			*/
#ifdef TEST
			static void Destroy(void);
#endif
			
			/*! \brief Checks if ACSServer Class is initialized
			*
			*  \return true if the initialization was successful and
			*  false otherwise.
			*/
			bool IsFullyInitialized(void);
			
			ACSServer(std::string nameIn, LocationIDType idIn);
			~ACSServer(void);
			ACSServer & operator=(const ACSServer & source);

			void loopInit();
			void loopDisabled();
			void loopGNDStart();
			void loopGNDPointing();
			void loopGNDStop();
			void loopPLDStart();
			void loopPLDPointing();
			void loopPLDStop();

			BEGIN_STATE_MAP
				STATE_MAP_ENTRY(&ACSServer::loopInit)
				STATE_MAP_ENTRY(&ACSServer::loopDisabled)
				STATE_MAP_ENTRY(&ACSServer::loopGNDStart)
				STATE_MAP_ENTRY(&ACSServer::loopGNDPointing)
				STATE_MAP_ENTRY(&ACSServer::loopGNDStop)
				STATE_MAP_ENTRY(&ACSServer::loopPLDStart)
				STATE_MAP_ENTRY(&ACSServer::loopPLDPointing)
				STATE_MAP_ENTRY(&ACSServer::loopPLDStop)
			END_STATE_MAP

			enum ACS_State{
				ST_INIT,
				ST_DISABLED,
				ST_GND_START,
				ST_GND_POINTING,
				ST_GND_STOP,
				ST_PLD_START,
				ST_PLD_POINTING,
				ST_PLD_STOP
			};

		};
	}
}

#endif /* ACSSERVER_H_ */
