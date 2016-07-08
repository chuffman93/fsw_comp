/*
* PayloadServer.h
*
*  Created on: May 27, 2011
*      Author: Michael
* Last Updated by: Glenda Alvarenga
*
*/

#ifndef PLDSERVER_H_
#define PLDSERVER_H_

#include "core/StdTypes.h"
#include "servers/SubsystemServer.h"
#include "core/Dispatcher.h"
#include "core/ModeManager.h"
#include "core/MessageHandlerRegistry.h"
#include "core/Arbitrator.h"
#include "servers/PLDHandlers.h"

namespace Phoenix
{
	namespace Servers
	{
		#define DEFAULT_RES_X		800

		#define DEFAULT_RES_Y		800

		#define DEFAULT_CHUNK_SIZE	100
		
		#define DEFAULT_GAIN		6

		#define DEFAULT_EXP_TIME	32

		#define MAX_RES_X			2592

		#define MAX_RES_Y			1944

		#define MAX_CHUNK_SIZE		65536

		#define MAX_GAIN			64

		#define MAX_EXP_TIME		65536

		class PLDServer : public SubsystemServer, public Phoenix::Core::Singleton
		{
			/*! \brief Declare Factory a friend class
			*
			*	This allows factory to call PLDServer's private constructor
			*/
			friend class Phoenix::Core::Factory;
			
		public:
			
			void SubsystemLoop(void);
			//void PLDProcess(SystemModeEnum modeIndex, int opcode, ReturnMessage * retMsg);
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

			bool PLDSetResolution(uint32 res);
			bool PLDSetChunkSize(uint32 chunk);
			bool PLDSetGain(uint32 newGain);
			bool PLDSetExpTime(uint32 newExpTime);
			
			bool PLDGetResolution(uint32 * res);
			bool PLDGetChunkSize(uint32 * chunk);
			bool PLDGetGain(uint32 * curGain);
			bool PLDGetExpTime(uint32 * curExp);

		private:
			
			/*! \brief Initialize the PLDServer Class
			*
			*  Initializes the operating system constructs needed for
			*  PLDServer to work properly.
			*/
			static void Initialize(void);
			
			/*! \brief Static Destructor for PLDServer
			*
			*  Frees all internal memory use, frees all operating system
			*  objects used, and deletes the singleton instance pointer.
			*/
#ifdef TEST
			static void Destroy(void);
#endif
			
			/*! \brief Checks if PLDServer Class is initialized
			*
			*  \return true if the initialization was successful and
			*  false otherwise.
			*/
			bool IsFullyInitialized(void);
			
			PLDServer(std::string nameIn, LocationIDType idIn);
			~PLDServer();
			PLDServer & operator=(const PLDServer & source); //possibly for firmware update
			
			Phoenix::Core::MessageHandlerRegistry reg;
			Phoenix::Core::Arbitrator arby;

			//picture variables
			uint32 resolution;
			uint32 chuckSize;
			uint32 gain;
			uint32 exposureTime;

			void PLDAccessMode(Phoenix::Core::ModeManager * modeManager);		// 0
			void PLDStartupMode(Phoenix::Core::ModeManager * modeManager);		// 1
			void PLDBusMode(Phoenix::Core::ModeManager * modeManager);			// 2
			void PLDPayloadMode(Phoenix::Core::ModeManager * modeManager);		// 3
			void PLDErrorMode(Phoenix::Core::ModeManager * modeManager);			// 4
			void PLDComMode(Phoenix::Core::ModeManager * modeManager);			// 5
			
			void(PLDServer::*modeArray[6])(Phoenix::Core::ModeManager * modeManager);

		};
	}
}

#endif /* PLDSERVER_H_ */
