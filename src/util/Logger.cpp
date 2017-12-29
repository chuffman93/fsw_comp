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

LogLevel Logger::globalLevel = LEVEL_DEBUG;
Lock Logger::lock;
std::map<pthread_t, std::string> Logger::threadNames;
LogMode Logger::mode = MODE_NOTHING;
std::vector<std::pair<LogTags, LogLevel> > Logger::filters;

/*!
 * Sets the global logging level to the passed value
 * \param level the new logging level
 */
void Logger::setLevel(LogLevel level){
	LogTags tags;
	tags += LogTag("Name", "Logger");
	Stream(LEVEL_FATAL, tags) << "Setting global log level to " << levelTag(level).second;
	lock.lock();
	globalLevel = level;
	lock.unlock();
}

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
 * \return the corrisponding string
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
 * Helper function to convert the current thread id into a tag.
 * Will return the name of the thread if it is in the registry or the thread id if not
 * \return the name of the thread or the thread id
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


void Logger::registerFilter(LogTags tags, LogLevel level){
	Logger::Stream(LEVEL_FATAL, tags) << "Will now filter to level " << levelTag(level).second;
	lock.lock();
	filters.push_back(std::pair<LogTags, LogLevel>(tags, level));
	lock.unlock();
}

void Logger::clearFilters(){
	lock.lock();
	filters.clear();
	lock.unlock();
}

/*!
 * Logs a passed message, if the level is above the global logging level
 * \param level the level of the message
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
		}
	}
}

void Logger::log(LogLevel level, std::string message){
	Logger::log(level, LogTags(), message);
}

std::string Logger::tagsToString(LogTags tags){
	stringstream ss;
	const char* names[] = {"Level", "Time", "Thread", "Name", "Instance"};
	int widths[] = {5, 8, 4, 0, 0};
	for(size_t i = 0; i < sizeof(names)/sizeof(names[0]); i++){
		if(tags.find(names[i]) != tags.end()){
			ss << std::setw(0) << "[";
			ss << std::left << std::setw(widths[i]) << tags[names[i]];
			ss << std::setw(0) << "]";
		}
	}

	return ss.str();
}



