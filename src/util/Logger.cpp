/*
 * 	Logger.cpp
 *
 * 	Created on: May 26, 2016
 * 	    Author: Alex St. Clair
 */

#include "core/StdTypes.h"
#include "util/Logger.h"
#include <stdio.h>
#include <string.h>
#include <sys/types.h>

// Set desired threshold for logging here
const LoggerLevelType Logger::threshold = LOGGER_LEVEL_INFO;
using namespace Phoenix::Core;
using namespace std;


void Logger::Initialize(void){
	//Left intentionally blank
}

bool Logger::IsFullyInitialized(void)
{
        return (Singleton::IsFullyInitialized());
}

Logger::Logger(void){
	//Left intentionally blank
}

Logger::~Logger(void){
	//Left intentionally blank
}

// TODO:
// 1.  When writing out a logging line, prepend a timestamp to the line
// 2.  When writing out a logging line, derive a thread ID and write it out, probably between the timestamp and the actual logging text
// 3.  The logging method should have two parameters:  a string and the logging level, like "DEBUG" or "INFO" or "ERROR".  The logging level should be an enumerated type and part of the logging class.
// 4.  The logging class can output to the screen, a file, or both
// 5.  If logging to a file, the PID and process name should be part of the filename
void Logger::Log(char const * message, LoggerLevelType level_in){

	if(level_in >= threshold){
		// Log all of the thread and time info
		// Print thread and time info
		printf("[PID: %d]", getpid());
		printf("[TID: %u]", (uint16) pthread_self());
		printf("(%u) ", getTimeInMilis());

		// Display the log level
		switch(level_in){
		case LOGGER_LEVEL_DEBUG:
			printf("DEBUG:\t");
			break;
		case LOGGER_LEVEL_INFO:
			printf("INFO:\t");
			break;
		case LOGGER_LEVEL_WARN:
			printf("WARN:\t");
			break;
		case LOGGER_LEVEL_ERROR:
			printf("ERROR:\t");
			break;
		case LOGGER_LEVEL_FATAL:
			printf("FATAL:\t");
			break;
		default:
			// maybe add assert on bounds
			break;
		}

		// Display the message
		printf("%s\n", message);
	}
}

void Logger::Log(string message, LoggerLevelType level_in){
	Log(message.c_str(), level_in);
}
