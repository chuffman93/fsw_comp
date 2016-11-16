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

namespace AllStar{
namespace Core{

enum TaskRunStateEnum{
	TASK_RUNSTATE_RUNNING = 0,
	TASK_RUNSTATE_NOT_RUNNING,
	TASK_RUNSTATE_MAX
};

// ---------------------------------------- Task State Class ----------------------------------------
class TaskState{
	public:
		TaskState(TaskRunStateEnum taskStateIn, bool kickStateIn = false)
		: taskState(taskStateIn), kickState(kickStateIn){
		}

		bool GetKickState() const{
			return kickState;
		}

		TaskRunStateEnum GetTaskState() const{
			return taskState;
		}

		void SetKickState(bool kickState){
			this->kickState = kickState;
		}

		void SetTaskState(TaskRunStateEnum taskState){
			this->taskState = taskState;
		}

	private:
		TaskRunStateEnum taskState;
		bool kickState;
};

// ------------------------------------ pthread_t Wrapper Class -------------------------------------
class PThread{
public:
	SubsystemServer * server;
	pthread_t tid;
	bool isRunning;
	string serverName;

    PThread(SubsystemServer * serverIn){
    	tid = 1;
    	isRunning = false;
    	serverName = serverIn->name;
    	server = serverIn;
    	start();
    }

    void stop(){
    	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
    	logger->Log(LOGGER_LEVEL_WARN, "Stopping thread for %s Server!", serverName.c_str());
    	pthread_cancel(tid);
    	isRunning = false;
    }

    void start(){
    	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));

    	logger->Log(LOGGER_LEVEL_INFO, "Starting thread for %s Server!", serverName.c_str());
    	if(pthread_create(&tid, NULL, &runServer, (void *) server) == 0){
			logger->Log(LOGGER_LEVEL_INFO, "Thread created with tid: %u", tid);
    		isRunning = true;
		}else{
			isRunning = false;
			logger->Log(LOGGER_LEVEL_FATAL, "Error creating thread for %s server!", serverName.c_str());
		}
    }
};

}
}

#endif
