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

			void ACSAccessMode(Phoenix::Core::ModeManager * modeManager);		// 0
			void ACSStartupMode(Phoenix::Core::ModeManager * modeManager);		// 1
			void ACSBusMode(Phoenix::Core::ModeManager * modeManager);			// 2
			void ACSPayloadMode(Phoenix::Core::ModeManager * modeManager);		// 3
			void ACSErrorMode(Phoenix::Core::ModeManager * modeManager);			// 4
			void ACSComMode(Phoenix::Core::ModeManager * modeManager);			// 5
			
			void(ACSServer::*modeArray[6])(Phoenix::Core::ModeManager * modeManager);

		};
	}
}

#endif /* ACSSERVER_H_ */
