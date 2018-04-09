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
#include <iomanip>
#include <time.h>

using namespace std;

LogLevel Logger::globalLevel = LEVEL_INFO;
Lock Logger::lock;
std::map<pthread_t, std::string> Logger::threadNames;
LogMode Logger::mode = MODE_NOTHING;
std::vector<std::pair<LogTags, LogLevel> > Logger::filters;

LogTags::LogTags(){}

LogTags::LogTags(LogTag tag){
	this->insert(tag);
}

LogTags LogTags::operator+(const LogTag& tag){
	LogTags newtags;
	newtags.insert(this->begin(), this->end());
	newtags.insert(tag);
	return newtags;
}

void LogTags::operator+=(const LogTag& tag){
	this->insert(tag);
}

LogTags LogTags::operator+(const LogTags& tags2){
	LogTags newtags;
	newtags.insert(this->begin(), this->end());
	LogTags temp = tags2; //Yes I know it's stupid, but it's what eclipse wants...
	newtags.insert(temp.begin(), temp.end());
	return newtags;
}

/*!
 * Sets the global logging level to the passed value
 * \param level the new logging level
 */
void Logger::setLevel(LogLevel level){
	LogTags tags;
	tags += LogTag("Name", "Logger");
	Stream(LEVEL_INFO, tags) << "Setting global log level to " << levelTag(level).second;
	lock.lock();
	globalLevel = level;
	lock.unlock();
}

/*!
 * Sets the mode for the logger
 * \param mode the new logging mode
 */
void Logger::setMode(LogMode mode){
	lock.lock();
	Logger::mode = mode;
	lock.unlock();
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
 * \return A tag for the level
 */
LogTag Logger::levelTag(LogLevel level){
	switch(level){
	case LEVEL_DEBUG:
		return LogTag("Level", "DEBUG");
	case LEVEL_INFO:
		return LogTag("Level", "\x1b[34m""INFO ""\x1b[0m");
	case LEVEL_WARN:
		return LogTag("Level", "\x1b[33m""WARN ""\x1b[0m");
	case LEVEL_ERROR:
		return LogTag("Level", "\x1b[31m""ERROR""\x1b[0m");
	case LEVEL_FATAL:
		return LogTag("Level", "\x1b[35m""FATAL""\x1b[0m");
	default:
		return LogTag("Level", "\x1b[35m""INVALID""\x1b[0m");
	}
}

/*!
 * Helper function to get the string name of a thread. Used in watchdog to indicate that
 * which threads had issue.
 * \param thread id
 * \return string name associated with ID.
 */
std::string Logger::getThreadName(pthread_t thread){
	std::map<pthread_t, std::string>::iterator it = threadNames.find(thread);
	std::stringstream ss;
	ss << (*it).second;
	return ss.str();
}

/*!
 * Helper function to convert the current thread id into a tag.
 * Will return the name of the thread if it is in the registry or the thread id if not
 * \return tag containing the name of the thread or the thread id
 */
LogTag Logger::threadTag(){
	std::stringstream ss;
	std::map<pthread_t, std::string>::iterator it = threadNames.find(pthread_self());
	if (it == threadNames.end()){
		ss << (pthread_self() % 10000);
	}else{
		ss << (*it).second;
	}
	return LogTag("Thread", ss.str());
}

/*!
 * Helper function to create a timestamp tag
 * Will return the time formated in hh:mm:ss.sss
 * \return Timestamp tag
 */
LogTag Logger::timeTag(){
	timespec t;
	clock_gettime(CLOCK_REALTIME, &t);
	struct tm tm = *localtime(&t.tv_sec);
	stringstream ss;
	ss << std::setw(2) << std::setfill('0') << std::right << tm.tm_hour << ":";
	ss << std::setw(2) << std::setfill('0') << std::right << tm.tm_min << ":";
	ss << std::setw(2) << std::setfill('0') << std::right << tm.tm_sec;
	ss << "." << std::setw(3) << (t.tv_nsec/1000000	);
	return LogTag("Time", ss.str());
}

/*!
 * Register a new filter. When a message is logged with tags that match those supplied,
 * the provided level will be used as the threshold instead of the global level
 * \param tags The tags to match on for the filter
 * \param level The new threshold for comparison
 */
void Logger::registerFilter(LogTags tags, LogLevel level){
	Logger::Stream(LEVEL_INFO, tags) << "Will now filter to level " << levelTag(level).second;
	lock.lock();
	filters.push_back(std::pair<LogTags, LogLevel>(tags, level));
	lock.unlock();
}

//! Clears all current filters in the logger
void Logger::clearFilters(){
	lock.lock();
	filters.clear();
	lock.unlock();
}

/*!
 * Logs a passed message, if the level is above the global logging level
 * or if there is a filter for the type of message
 * \param level the level of the message
 * \param tags the tags to be logged along with the message
 * \param message the message to be logged
 */
void Logger::log(LogLevel level, LogTags tags, std::string message){
	LockGuard l(lock);

	LogLevel threshold = globalLevel;
	//Go through all of the filters
	for(std::vector<std::pair<LogTags, LogLevel> >::iterator i = filters.begin(); i != filters.end(); i++){
		bool match = true;
		//Go through all of the tags in the filter
		for(LogTags::iterator j = i->first.begin(); j != i->first.end(); j++){
			LogTags::iterator k = tags.find(j->first);
			//If the tag doesn't match the passed tag it isn't a match
			if(k == tags.end() || k->second != j->second){
				match = false;
			}
		}
		//If the passed tags match a filter, set the threshold accordingly
		if(match){
			threshold = i->second;
		}
	}
	if(level >= threshold){
		tags += levelTag(level);
		tags += threadTag();
		tags += timeTag();
		switch(mode){
		case MODE_NOTHING:
			break;
		case MODE_PRINT:
			cout << tagsToString(tags) << " " << message << endl;
			break;
		case MODE_PW:
			FileManager::writeLog(tagsToString(tags),message);
			cout << tagsToString(tags) << " " << message << endl;
			break;

		}
	}
}

/*!
 * Logs a passed message, if the level is above the global logging level
 * or if there is a filter for the type of message
 * \param level the level of the message
 * \param message the message to be logged
 */
void Logger::log(LogLevel level, std::string message){
	Logger::log(level, LogTags(), message);
}

/*!
 * Helper function to convert LogTags into a string.
 * Will arrange tags in a fixed order (Level, Time, Thread, Name, Instance).
 * Will not include a tag in the string if not present
 * \param tags The tags to be converted
 * \return string containing the tags
 */
std::string Logger::tagsToString(LogTags tags){
	stringstream ss;
	const char* names[] = {"Time", "Level", "Thread", "Name", "Instance"};
	int widths[] = {8, 5, 4, 0, 0};
	for(size_t i = 0; i < sizeof(names)/sizeof(names[0]); i++){
		if(tags.find(names[i]) != tags.end()){
			ss << std::setw(0) << "[";
			ss << std::left << std::setw(widths[i]) << tags[names[i]];
			ss << std::setw(0) << "]";
		}
	}

	return ss.str();
}




