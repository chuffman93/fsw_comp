/*
 * Logger.h
 *
 *  Created on: Dec 10, 2017
 *      Author: cyborg9
 */

#ifndef LOGGER_H_
#define LOGGER_H_

#include "core/Monitor.h"
#include <pthread.h>
#include <string>
#include <map>

enum log_level_t{
	LEVEL_DEBUG,
	LEVEL_INFO,
	LEVEL_WARN,
	LEVEL_ERROR,
	LEVEL_FATAL,
};

class Logger: public Monitor{
public:
	Logger():globalLevel(LEVEL_DEBUG) {}
	~Logger() {}

	void setLevel(log_level_t level);
	void log(log_level_t level, std::string message);
	void registerThread(std::string threadName);

private:
	std::string levelToString(log_level_t);
	std::string threadIdTag();
	log_level_t globalLevel;
	std::map<pthread_t, std::string> threadNames;
};




#endif /* LOGGER_H_ */
