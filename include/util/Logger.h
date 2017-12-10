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

/*
 * Class to handle all required logging for FSW.
 * Intended for only one instance to be made, performs mutual exclusion so that no output gets spliced together.
 */
class Logger: public Monitor{
public:
	//! By default set the global logging level to Debug
	Logger():globalLevel(LEVEL_DEBUG) {}
	~Logger() {}

	void setLevel(log_level_t level);
	void log(log_level_t level, std::string message);
	void registerThread(std::string threadName);

private:
	std::string levelToString(log_level_t);
	std::string threadIdTag();

	//! Default logging level for all log messages
	log_level_t globalLevel;
	//! Registry of thread names
	std::map<pthread_t, std::string> threadNames;
};




#endif /* LOGGER_H_ */
