/*
 * CMDServer.h
 *
 *  Created on: Feb 12, 2013
 *      Author: Caitlyn
 *  Modified:
 */

#ifndef CMDSERVER_H_
#define CMDSERVER_H_

#include "servers/SubsystemServer.h"

#include "core/Dispatcher.h"
#include "core/StdTypes.h"
#include "core/ModeManager.h"
#include "core/MessageHandlerRegistry.h"
#include "core/Arbitrator.h"

#include "util/FileHandler.h"



namespace Phoenix
{
	namespace Servers
	{

		class CMDServer : public SubsystemServer, public Phoenix::Core::Singleton
		{
			/*! \brief Declare Factory a friend class
			*
			*	This allows factory to call CMDServer's private constructor
			*/
			friend class Phoenix::Core::Factory;
			
		public:
			void SubsystemLoop(void);
			void Update(const Core::SystemMode * mode);
			bool RegisterHandlers();
			static int subsystem_acp_protocol[HARDWARE_LOCATION_MAX];

		private:

			/*! \brief Initialize the CMDServer Class
			*
			*  Initializes the operating system constructs needed for
			*  CMDServer to work properly.
			*/
			static void Initialize(void);
			
			/*! \brief Static Destructor for CMDServer
			*
			*  Frees all internal memory use, frees all operating system
			*  objects used, and deletes the singleton instance pointer.
			*/
#ifdef TEST
			static void Destroy(void);
#endif
			
			/*! \brief Checks if CMDServer Class is initialized
			*
			*  \return true if the initialization was successful and
			*  false otherwise.
			*/
			bool IsFullyInitialized(void);
				
			CMDServer(std::string nameIn, LocationIDType idIn);
			~CMDServer();
			CMDServer & operator=(const CMDServer & source);

			// Member variables needed to register message handlers.
			Phoenix::Core::MessageHandlerRegistry reg;
			Phoenix::Core::Arbitrator arby;

			char * CMDFiles [5];
		};
	}
}

#endif /* CMDSERVER_H_ */
