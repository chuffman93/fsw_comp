/*
 * SubsystemServer.cpp
 *
 *  Created on: Feb 16, 2011
 *      Author: Riley
 */

#include "servers/SubsystemServer.h"

#include "core/Dispatcher.h"
#include "core/Singleton.h"
#include "core/Factory.h"
#include "POSIX.h"

using namespace std;
using namespace Phoenix::Core;

namespace Phoenix
{
	namespace Servers 
	{
		SubsystemServer::SubsystemServer(string nameIn, LocationIDType idIn)
						: Server(nameIn, idIn)
		{
			// Left Intentionally Blank
		}

		SubsystemServer::~SubsystemServer()
		{
			// Left Intentionally Blank
		}

		SubsystemServer & SubsystemServer::operator=(const SubsystemServer & source)
		{
		    if (this == &source)
		    {
		        return *this;
		    }

		    Server::operator =(source);

		    return *this;
		}
		
		bool SubsystemServer::operator ==(const Server & check) const
		{
			return (name == check.GetName() && id == check.GetID());
		}

		void SubsystemServer::MainLoop(void )
		{
			//Check subsystem for a message, get the message, and then handle it

			SubsystemLoop();
		}

		void SubsystemServer::SubsystemLoop(void)
		{
			Dispatcher * dispatcher = dynamic_cast<Dispatcher *> (Factory::GetInstance(DISPATCHER_SINGLETON));
			
			while(1)
			{
				dispatcher->Listen(id);
				
				usleep(1000000);
			}
		}

		void SubsystemServer::Update(const SystemMode * mode)
		{

		}
	}
}
