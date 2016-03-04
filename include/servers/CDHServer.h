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

namespace Phoenix
{
	namespace Servers
	{
	class SCHServer : public SubsystemServer, public Phoenix::Core::Singleton
		{
			/*! \brief Declare Factory a friend class
			*
			*	This allows factory to call CDHServer's private constructor
			*/
			friend class Phoenix::Core::Factory;

		public:

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
		};
	}
}
