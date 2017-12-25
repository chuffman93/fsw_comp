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

/*!
 * Sets the global logging level to the passed value
 * \param level the new logging level
 */
void Logger::setLevel(log_level_t level){
	LockGuard l(lock);
	cout << "Setting global log level to " << levelToString(level) << endl;
	globalLevel = level;
}

/*!
 * Adds an entry into the registry between the current thread id and the passed name
 * \param threadname the name to add into the registry
 */
void Logger::registerThread(std::string threadname){
	LockGuard l(lock);
	threadNames.insert ( std::pair<pthread_t,std::string>(pthread_self(), threadname) );
}

/*!
 * Helper function to convert the passed level into a color coded string
 * \param level the level to be converted
 * \return the corrisponding string
 */
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

/*!
 * Helper function to convert the current thread id into a tag.
 * Will return the name of the thread if it is in the registry or the thread id if not
 * \return the name of the thread or the thread id
 */
std::string Logger::threadIdTag(){
	std::stringstream ss;
	std::map<pthread_t, std::string>::iterator it = threadNames.find(pthread_self());
	if (it == threadNames.end()){
		ss << "[TID: " << pthread_self() << "]";
	}else{
		ss << "[" << (*it).second << "]";
	}
	return ss.str();
}

/*!
 * Logs a passed message, if the level is above the global logging level
 * \param level the level of the message
 * \param message the message to be logged
 */
void Logger::log(log_level_t level, std::string message){
	LockGuard l(lock);
	if(level >= globalLevel){
		cout << threadIdTag() << "[" << levelToString(level) << "]" << message << endl;
	}
}



