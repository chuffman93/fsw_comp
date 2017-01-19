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
	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));

	SubsystemServer * server = (SubsystemServer *) args;

	// wait before starting the server
	for(int i = 0; i < server->startupWait; i++){
		usleep(100000);
	}

	logger->Log(LOGGER_LEVEL_INFO, "Kicking off the %s Server", server->name.c_str());
	server->SubsystemLoop();

	pthread_exit(NULL);
}

}
}
