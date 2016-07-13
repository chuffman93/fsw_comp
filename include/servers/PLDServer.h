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
		class PLDServer : public SubsystemServer, public Phoenix::Core::Singleton
		{
			friend class Phoenix::Core::Factory;
		public:
			
			void SubsystemLoop(void);
			void Update(const SystemModeEnum mode);

			bool RegisterHandlers();

		private:
			static void Initialize(void);
			
#ifdef TEST
			static void Destroy(void);
#endif
			
			bool IsFullyInitialized(void);
			
			PLDServer(std::string nameIn, LocationIDType idIn);
			~PLDServer();
			PLDServer & operator=(const PLDServer & source); //possibly for firmware update
			
			Phoenix::Core::MessageHandlerRegistry reg;
			Phoenix::Core::Arbitrator arby;
			
			void loopInit();
			void loopIdle();
			void loopStartup();
			void loopShutdown();
			void loopScience();

			BEGIN_STATE_MAP
				STATE_MAP_ENTRY(loopInit)
				STATE_MAP_ENTRY(loopIdle)
				STATE_MAP_ENTRY(loopStartup)
				STATE_MAP_ENTRY(loopScience)
				STATE_MAP_ENTRY(loopShutdown)
			END_STATE_MAP

			enum PLD_States {
				ST_INIT = 0,
				ST_IDLE,
				ST_STARTUP,
				ST_SCIENCE,
				ST_SHUTDOWN
			};

		};
	}
}

#endif /* PLDSERVER_H_ */
