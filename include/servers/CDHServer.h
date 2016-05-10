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

			// Allows for easy look into memory usage
			struct sysinfo si;
			struct statvfs svfs;

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

		};
	}
}

#endif //CDHSERVER_H_
