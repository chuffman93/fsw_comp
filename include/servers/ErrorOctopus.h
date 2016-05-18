/*
* ErrorOctopus.h
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

#include "core/ReturnMessage.h"

namespace Phoenix
{
	namespace Servers
	{
		class ErrorOctopus : public SubsystemServer, public Phoenix::Core::Singleton
		{
			/*! \brief Declare Factory a friend class
			*
			*	This allows factory to call ErrorOctopus's private constructor
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

		private:
			/*! \brief Initialize the ErrorOctopus Class
			*
			*  Initializes the operating system constructs needed for
			*  ErrorOctopus to work properly.
			*/
			static void Initialize(void);
			
			/*! \brief Static Destructor for ErrorOctopus
			*
			*  Frees all internal memory use, frees all operating system
			*  objects used, and deletes the singleton instance pointer.
			*/
#ifdef HOST
			static void Destroy(void);
#endif
			
			/*! \brief Checks if ErrorOctopus Class is initialized
			*
			*  \return true if the initialization was successful and
			*  false otherwise.
			*/
			bool IsFullyInitialized(void);
			
			ErrorOctopus(std::string nameIn, LocationIDType idIn);
			~ErrorOctopus();
			ErrorOctopus & operator=(const ErrorOctopus & source);
			
			// Member variables needed to register message handlers.
			Phoenix::Core::MessageHandlerRegistry reg;
			Phoenix::Core::Arbitrator arby;

		};
	}
}

#endif /* ERROROCTOPUS_H_ */
