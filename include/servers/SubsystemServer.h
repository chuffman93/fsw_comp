/*
 * SubsystemServer.h
 *
 *  Created on: Feb 16, 2011
 *      Author: Riley
 *
 *   State Machine template from : http://www.drdobbs.com/cpp/state-machine-design-in-c/184401236?pgno=2
 */

#ifndef SUBSYSTEMSERVER_H_
#define SUBSYSTEMSERVER_H_

#include "servers/Server.h"
#include "core/StdTypes.h"
#include <stdint.h>

namespace Phoenix
{
    namespace Servers
    {
    	struct StateStruct;
        class SubsystemServer : public Server
        {
        public:
            SubsystemServer(std::string nameIn, LocationIDType idIn);
            virtual ~SubsystemServer();
            SubsystemServer & operator=(const SubsystemServer & source);
			virtual bool operator ==(const Server & check) const;
            void SubsystemLoop(void);
            void MainLoop(void){}
            virtual void Update(SystemModeEnum mode);
        protected:
            uint16_t currentState;
            virtual const StateStruct * GetStateMap() = 0;
        };

        typedef void (SubsystemServer::*StateFunc)(void);
        struct StateStruct{
        	StateFunc function;
        };

		#define BEGIN_STATE_MAP \
			public:\
			const StateStruct* GetStateMap() {\
				static const StateStruct StateMap[] = {

		#define STATE_MAP_ENTRY(entry)\
				{reinterpret_cast<StateFunc>(entry)},

		#define END_STATE_MAP \
			};\
			return &StateMap[0];}
    }
}

#endif /* SUBSYSTEMSERVER_H_ */
