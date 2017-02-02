/*
 * SubsystemServer.cpp
 *
 *  Created on: Feb 16, 2011
 *      Author: Riley
 *
 *  Last Modified: Jack Dinkel 12/16/16
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
				: Server(nameIn, idIn), currentState(0), sleepTime(1000), hsDelays(10)
{
	// Left Intentionally Blank
}

SubsystemServer::SubsystemServer(string nameIn, LocationIDType idIn, int sleep, int delays)
				: Server(nameIn, idIn), currentState(0), sleepTime(sleep), hsDelays(delays)
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

bool SubsystemServer::CheckHealthStatus(){
	return false;
}

void SubsystemServer::SubsystemLoop(void)
{
	WatchdogManager * wdm = dynamic_cast<WatchdogManager *> (Factory::GetInstance(WATCHDOG_MANAGER_SINGLETON));

	while(1)
	{
		for(int i = 0; i < hsDelays; i++){
			int64 LastWakeTime = getTimeInMillis();
			//while(Listen(id));
			wdm->Kick();
			waitUntil(LastWakeTime, sleepTime);

			StateFunc function = GetStateMap()[currentState].function;

			(this->*function)();
		}
		
		this->CheckHealthStatus();
	}
}

} // End of namespace Servers
} // End of namespace AllStar
