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

/*
 * Enum to contain all of the different logging levels
 */
enum LogLevel{
	LEVEL_DEBUG,
	LEVEL_INFO,
	LEVEL_WARN,
	LEVEL_ERROR,
	LEVEL_FATAL,
	LEVEL_NONE,
};

/*
 * The different modes for the logger to be in
 */
enum LogMode{
	MODE_NOTHING,
	MODE_PRINT,
};


typedef std::pair<std::string, std::string> LogTag;

/*!
 * Extention of map<string, string> to provide some syntactic sugar
 */
class LogTags: public std::map<std::string, std::string>{
public:
	LogTags();
	LogTags(LogTag tag);
	LogTags operator+(const LogTag& tag);
	void operator+=(const LogTag& tag);
	LogTags operator+(const LogTags& tags2);
};

/*
 * Class to handle all required logging for FSW.
 * Static methods, performs mutual exclusion so that no output gets spliced together.
 */
class Logger{
public:
	/*!
	 * Class to allow for a stream style interface into the logger
	 * Intended for a new instance to be created each time
	 */
	class Stream{
	public:
		/*!
		 * Create a new stream object
		 */
		Stream(LogLevel level, LogTags tags):level(level), tags(tags){}
		Stream(LogLevel level):level(level), tags(){}
		/*!
		 * Upon destruction of the stream object, the logger is called
		 */
		~Stream(){
			Logger::log(level, tags, ss.str());
		}
		/*!
		 * Provides stream style syntax
		 * \param t the object to intake
		 * \return the stream
		 */
	    template <typename T> inline Stream& operator<<(const T& t)
	    {
	        ss << t;
	        return * this;
	    }
	private:
	    //! Level to be used when logged
		LogLevel level;
		//! Tags to be used when logged
		LogTags tags;
		//! Stream to hold the message until logged
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
