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
// Timestamp
// Thread ID
void Logger::Log(char const * message, LoggerLevelType level_in){

	if(level_in >= threshold){
		// Log all of the thread and time info
		// Print thread and time info
		printf("[PID: %d]", getpid());
		printf("[TID: %u]", (uint16) pthread_self());
		printf("(%d) ", getTimeInMilis());

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
	if(level_in >= threshold){
		// Log all of the thread and time info
		// Print thread and time info
		printf("[PID: %d] ", getpid());

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
		printf("%s", message.c_str());
	}
}
