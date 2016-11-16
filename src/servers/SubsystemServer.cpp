/*
 * SubsystemServer.cpp
 *
 *  Created on: Feb 16, 2011
 *      Author: Riley
 */

#include "servers/SubsystemServer.h"
#include "servers/DispatchStdTasks.h"

#include "core/WatchdogManager.h"
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
	WatchdogManager * wdm = dynamic_cast<WatchdogManager *> (Factory::GetInstance(WATCHDOG_MANAGER_SINGLETON));

	while(1)
	{
		int64 LastWakeTime = getTimeInMillis();
		while(Listen(id));
		wdm->Kick();
		waitUntil(LastWakeTime, 1000); // TODO: rewrite with sleep based on time elapsed
		
		StateFunc function = GetStateMap()[currentState].function;

		(this->*function)();
	}
}

} // End of namespace Servers
} // End of namespace AllStar
