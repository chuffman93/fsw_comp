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

#include "core/ModeManager.h"
#include "core/Dispatcher.h"
#include "core/Singleton.h"
#include "core/Factory.h"
#include "POSIX.h"

using namespace std;
using namespace AllStar::Core;

namespace AllStar {
namespace Servers {

SubsystemServer::SubsystemServer(string nameIn, LocationIDType idIn)
				: Server(nameIn, idIn), currentState(0), sleepTime(1000), hsDelays(10)
{ }

SubsystemServer::SubsystemServer(string nameIn, LocationIDType idIn, int sleep, int delays)
				: Server(nameIn, idIn), currentState(0), sleepTime(sleep), hsDelays(delays)
{ }

SubsystemServer::~SubsystemServer() { }

SubsystemServer & SubsystemServer::operator=(const SubsystemServer & source) {
	if (this == &source) {
		return *this;
	}

	Server::operator =(source);

	return *this;
}

bool SubsystemServer::operator ==(const Server & check) const {
	return (name == check.GetName() && id == check.GetID());
}

bool SubsystemServer::CheckHealthStatus() {
	return false;
}

void SubsystemServer::SubsystemLoop(void) {
	while (1) {
		for (int i = 0; i < hsDelays; i++) {
			wdmAlive();
			int64 LastWakeTime = getTimeInMillis();

			StateFunc function = GetStateMap()[currentState].function;
			(this->*function)();

			waitUntil(LastWakeTime, sleepTime);
		}
		
		this->CheckHealthStatus();
	}
}

} // End of namespace Servers
} // End of namespace AllStar
