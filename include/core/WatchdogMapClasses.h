/*
 * WatchdogMapClasses.h
 *
 *  Created on: Aug 17, 2016
 *      Author: Alex St. Clair
 */

#ifndef WATCHDOGMAP_H_
#define WATCHDOGMAP_H_

#include "core/WatchdogStdTasks.h"
#include "util/Logger.h"
#include <pthread.h>

namespace AllStar {
namespace Core {

// ------------------------------------ pthread_t Wrapper Class -------------------------------------
class PThread {
public:
	SubsystemServer * server;
	pthread_t tid;
	bool isRunning;
	string serverName;

    PThread(SubsystemServer * serverIn) {
    	tid = 1;
    	isRunning = false;
    	serverName = serverIn->name;
    	server = serverIn;
    	start();
    }

    void stop() {
    	Logger * logger = static_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
    	logger->Warning("Stopping thread for %s Server!", serverName.c_str());
    	pthread_cancel(tid);
    	isRunning = false;
    }

    void start() {
    	Logger * logger = static_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));

    	logger->Info("Starting thread for %s Server!", serverName.c_str());
    	if (pthread_create(&tid, NULL, &runServer, (void *) server) == 0) {
			logger->Info("Thread created with tid: %u", tid);
    		isRunning = true;
		} else {
			isRunning = false;
			logger->Fatal("Error creating thread for %s server!", serverName.c_str());
		}
    }
};

}
}

#endif
