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
const LoggerLevelType Logger::threshold = LOGGER_LEVEL_DEBUG;

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

void Logger::Log(char * message, LoggerLevelType level_in){

	if(level_in >= threshold){
		//Display the PID
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
		printf(message);
		printf("\n");
	}
}

// --------------------- Below is working class -------------------------------

//#include "core/StdTypes.h"
//#include "util/Logger.h"
//#include <stdio.h>
//#include <string.h>
//
//// Set desired threshold for logging here
//const LoggerLevelType Logger::threshold = LOGGER_LEVEL_DEBUG;
//
//Logger::Logger(void){
//	//Left intentionally blank
//}
//
//void Logger::Log(char * message, LoggerLevelType level_in){
//	if(level_in >= threshold){
//		switch(level_in){
//		case LOGGER_LEVEL_DEBUG:
//			printf("DEBUG:\t");
//			break;
//		case LOGGER_LEVEL_INFO:
//			printf("INFO:\t");
//			break;
//		case LOGGER_LEVEL_WARN:
//			printf("WARN:\t");
//			break;
//		case LOGGER_LEVEL_ERROR:
//			printf("ERROR:\t");
//			break;
//		case LOGGER_LEVEL_FATAL:
//			printf("FATAL:\t");
//			break;
//		default:
//			// maybe add assert on bounds
//			break;
//		}
//		printf(message);
//		printf("\n");
//	}
//}
