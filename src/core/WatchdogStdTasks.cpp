/*
 * WatchdogStdTasks.cpp
 *
 *  Created on: August 16, 2016
 *      Author: Alex St. Clair
 */

#include "core/WatchdogStdTasks.h"
#include "core/ModeManager.h"
#include "core/StdTypes.h"
#include "core/Factory.h"
#include "util/Logger.h"

namespace AllStar{
namespace Core{

void * runServer(void * args){
	Logger * logger = static_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));

	SubsystemServer * server = (SubsystemServer *) args;

	// wait before starting the server
	server->wdmAsleep();
	for(int i = 0; i < server->startupWait; i++){
		usleep(1000000);
	}
	server->wdmAlive();
	server->startupWait = 0; // TODO: find better way to ensure restarted threads start immediately?

	logger->Info("Kicking off the %s Server", server->name.c_str());
	server->SubsystemLoop();

	pthread_exit(NULL);
}

}
}
