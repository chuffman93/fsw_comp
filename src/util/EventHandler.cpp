/*
 * EventHandler.cpp
 *
 *  Created on: Dec 23, 2017
 *      Author: fsw
 */
#include "util/EventHandler.h"

Logger* EventHandler::logger = NULL;

void EventHandler::event(log_level_t level, std::string message){
	if(logger != NULL){
		logger->log(level, message);
	}
}

void EventHandler::attachLogger(Logger* newlogger){
	if(logger != NULL){
		delete logger;
	}
	logger = newlogger;
}



