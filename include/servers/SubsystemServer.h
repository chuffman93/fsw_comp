/*
 * SubsystemServer.h
 *
 *  Created on: Feb 16, 2011
 *      Author: Riley
 */

#ifndef SUBSYSTEMSERVER_H_
#define SUBSYSTEMSERVER_H_

#include "servers/Server.h"

namespace Phoenix
{
    namespace Servers
    {
        class SubsystemServer : public Server
        {
        public:
            SubsystemServer(std::string nameIn, LocationIDType idIn);
            virtual ~SubsystemServer();
            SubsystemServer & operator=(const SubsystemServer & source);
			virtual bool operator ==(const Server & check) const;
            virtual void MainLoop(void);
            virtual void SubsystemLoop(void);
            virtual void Update(const Core::SystemMode * mode);
        };
    }
}

#endif /* SUBSYSTEMSERVER_H_ */
