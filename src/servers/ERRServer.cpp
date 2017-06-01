/*
* ERRServer.cpp
*
*  Created on: June 1, 2017
*      Author: Alex St. Clair
*/
#include "servers/ERRServer.h"
#include "core/StdTypes.h"
#include "util/Logger.h"

namespace AllStar {
namespace Servers {

// --------------------- Necessary Methods --------------------------
ERRServer::ERRServer(string nameIn, LocationIDType idIn)
		: SubsystemServer(nameIn, idIn), Singleton()
{ }

ERRServer::~ERRServer() { }

bool ERRServer::IsFullyInitialized(void) {
	return Singleton::IsFullyInitialized();
}


// ---------------------- Subsystem Loop ----------------------------
void ERRServer::SubsystemLoop() {
	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
	ErrorOpcodeType error;
	int64 lastWakeTime;

	// loop
	while (1) {
		wdmAlive();
		lastWakeTime = getTimeInSec();

		if (!ErrorQueue.empty()) {
			if (this->TakeLock(MAX_BLOCK_TIME) && !ErrorQueue.empty()) {
				error = ErrorQueue.front();
				ErrorQueue.pop();

				// handle the error
			} else {
				logger->Log(LOGGER_LEVEL_WARN, "ERRServer: SubsystemLoop() error taking lock");
			}
		}

		waitUntil(lastWakeTime, 100);
	}
}


// ----------------------- Other Methods ----------------------------
void ERRServer::SendError(ErrorOpcodeType error) {
	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));

	if (this->TakeLock(MAX_BLOCK_TIME)) {
		ErrorQueue.push(error);
	} else {
		logger->Log(LOGGER_LEVEL_WARN, "ERRServer: SendError() unable to take lock");
	}
}

}
}
