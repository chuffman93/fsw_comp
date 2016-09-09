/*
* ERRServer.h
*
*  Created on: Feb 27, 2013
*      Author: Brian Campuzano
*/


#ifndef ERROROCTOPUS_H_
#define ERROROCTOPUS_H_

#include "core/StdTypes.h"
#include "core/Dispatcher.h"
#include "util/FileHandler.h"
#include "servers/SubsystemServer.h"
#include "core/MessageHandlerRegistry.h"
#include "core/Arbitrator.h"
#include "servers/ErrorHandlers.h"

namespace AllStar
{
	namespace Servers
	{
		class ERRServer : public SubsystemServer, public AllStar::Core::Singleton
		{
			/*! \brief Declare Factory a friend class
			*
			*	This allows factory to call ERRServer's private constructor
			*/
			friend class AllStar::Core::Factory;
			
		public:
			
			void SubsystemLoop(void);

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

		private:
			/*! \brief Initialize the ERRServer Class
			*
			*  Initializes the operating system constructs needed for
			*  ERRServer to work properly.
			*/
			static void Initialize(void);
			
			/*! \brief Static Destructor for ERRServer
			*
			*  Frees all internal memory use, frees all operating system
			*  objects used, and deletes the singleton instance pointer.
			*/
#ifdef HOST
			static void Destroy(void);
#endif
			
			/*! \brief Checks if ERRServer Class is initialized
			*
			*  \return true if the initialization was successful and
			*  false otherwise.
			*/
			bool IsFullyInitialized(void);
			
			ERRServer(std::string nameIn, LocationIDType idIn);
			~ERRServer();
			ERRServer & operator=(const ERRServer & source);
			
			// Member variables needed to register message handlers.
			AllStar::Core::MessageHandlerRegistry reg;
			AllStar::Core::Arbitrator arby;

			BEGIN_STATE_MAP
			END_STATE_MAP
		};
	}
}

#endif /* ERROROCTOPUS_H_ */
