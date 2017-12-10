/*
 * Logger.cpp
 *
 *  Created on: Dec 10, 2017
 *      Author: cyborg9
 */

#include "util/Logger.h"
#include <iostream>
#include <pthread.h>
#include <sstream>

using namespace std;


void Logger::setLevel(log_level_t level){
	LockGuard lock(this);
	cout << "Setting log level to " << levelToString(level) << endl;
	globalLevel = level;
}

void Logger::registerThread(std::string threadname){
	LockGuard lock(this);
	threadNames.insert ( std::pair<pthread_t,std::string>(pthread_self(), threadname) );
}


std::string Logger::levelToString(log_level_t level){
	switch(level){
	case LEVEL_DEBUG:
		return "DEBUG";
	case LEVEL_INFO:
		return "\x1b[34m""INFO""\x1b[0m";
	case LEVEL_WARN:
		return "\x1b[33m""WARN""\x1b[0m";
	case LEVEL_ERROR:
		return "\x1b[31m""ERROR""\x1b[0m";
	case LEVEL_FATAL:
		return "\x1b[35m""FATAL""\x1b[0m";
	default:
		return "\x1b[35m""INVALID""\x1b[0m";
	}
}

std::string Logger::threadIdTag(){
	std::stringstream ss;
	std::map<pthread_t, std::string>::iterator it = threadNames.find(pthread_self());
	if (it == threadNames.end()){
		ss << "[TID: " << pthread_self() << "]";
	}else{
		ss << "[TID: " << (*it).second << "]";
	}
	return ss.str();
}

void Logger::log(log_level_t level, std::string message){
	LockGuard lock(this);
	if(level >= globalLevel){
		cout << threadIdTag() << "[" << levelToString(level) << "] " << message << endl;
	}
}



