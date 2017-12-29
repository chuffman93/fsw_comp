/*
 * Logger.h
 *
 *  Created on: Dec 10, 2017
 *      Author: cyborg9
 */

#ifndef LOGGER_H_
#define LOGGER_H_

#include "core/Lock.h"
#include <pthread.h>
#include <sstream>
#include <string>
#include <map>
#include <vector>

enum LogLevel{
	LEVEL_DEBUG,
	LEVEL_INFO,
	LEVEL_WARN,
	LEVEL_ERROR,
	LEVEL_FATAL,
	LEVEL_NONE,
};

enum LogMode{
	MODE_NOTHING,
	MODE_PRINT,
};


typedef std::pair<std::string, std::string> LogTag;

class LogTags: public std::map<std::string, std::string>{
public:
	LogTags(){}
	LogTags(LogTag tag){
		this->insert(tag);
	}
	LogTags operator+(const LogTag& tag){
		LogTags newtags;
		newtags.insert(this->begin(), this->end());
		newtags.insert(tag);
		return newtags;
	}
	void operator+=(const LogTag& tag){
		this->insert(tag);
	}
	LogTags operator+(const LogTags& tags2){
		LogTags newtags;
		newtags.insert(this->begin(), this->end());
		newtags.insert(tags2.begin(), tags2.end());
		return newtags;
	}
};

/*
 * Class to handle all required logging for FSW.
 * Intended for only one instance to be made, performs mutual exclusion so that no output gets spliced together.
 */
class Logger{
public:
	class Stream{
	public:
		Stream(LogLevel level, LogTags tags):level(level), tags(tags){}
		Stream(LogLevel level):level(level), tags(){}
		~Stream(){
			Logger::log(level, tags, ss.str());
		}
	    template <typename T> inline Stream& operator<<(const T& t)
	    {
	        ss << t;
	        return * this;
	    }
	private:
		LogLevel level;
		LogTags tags;
		std::stringstream ss;
	};
	static void setLevel(LogLevel level);
	static void setMode(LogMode mode);
	static void registerThread(std::string threadName);
	static void registerFilter(LogTags tags, LogLevel level);
	static void clearFilters();
	static void log(LogLevel level, LogTags tags, std::string);
	static void log(LogLevel level, std::string);

private:
	static LogTag levelTag(LogLevel level);
	static LogTag threadTag();
	static LogTag timeTag();
	static std::string tagsToString(LogTags tags);

	//! Filters to set different LogLevels
	static std::vector<std::pair<LogTags, LogLevel> > filters;
	//! Default logging level for all log messages
	static LogLevel globalLevel;
	//! Mode for the logger
	static LogMode mode;
	//! Registry of thread names
	static std::map<pthread_t, std::string> threadNames;
	//! Lock for the logger
	static Lock lock;
};




#endif /* LOGGER_H_ */
