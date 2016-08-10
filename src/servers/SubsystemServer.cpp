/*
 * SubsystemServer.cpp
 *
 *  Created on: Feb 16, 2011
 *      Author: Riley
 */

#include "servers/SubsystemServer.h"
#include "servers/DispatchStdTasks.h"

#include "core/ModeManager.h"
#include "core/Dispatcher.h"
#include "core/Singleton.h"
#include "core/Factory.h"
#include "POSIX.h"

using namespace std;
using namespace AllStar::Core;

namespace AllStar{
namespace Servers{

SubsystemServer::SubsystemServer(string nameIn, LocationIDType idIn)
				: Server(nameIn, idIn), currentState(0)
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

void SubsystemServer::SubsystemLoop(void)
{
	//Dispatcher * dispatcher = dynamic_cast<Dispatcher *> (Factory::GetInstance(DISPATCHER_SINGLETON));

	while(1)
	{
		while(Listen(id));
		usleep(50000);
		
		StateFunc function = GetStateMap()[currentState].function;

		(this->*function)();
	}
}

void SubsystemServer::Update(const SystemModeEnum mode)
{

}

} // End of namespace Servers
} // End of namespace AllStar
