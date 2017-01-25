/*
 * 	Logger.cpp
 *
 * 	Created on: May 26, 2016
 * 	    Author: Alex St. Clair
 */

#include "core/StdTypes.h"
#include "util/Logger.h"
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <sys/types.h>

// Set desired threshold for logging here
const LoggerLevelType Logger::threshold = LOGGER_LEVEL_INFO;
const bool Logger::printToFile = true;
const bool Logger::printToStdOut = true;
using namespace AllStar::Core;
using namespace std;


void Logger::Initialize(void){
	printf("\n-------------- Logger initialized with threshold: %d --------------\n\n", threshold);
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

// TODO: The logging class should have the ability to output to the screen, a file, or both
void Logger::Log(LoggerLevelType level_in, char const * fmt, ...){

	if(level_in >= threshold){
		PrintInfo(level_in);

		char buff[500];
		va_list args;
		va_start(args, fmt);
		vsprintf(buff, fmt, args);
		va_end(args);

		// Display the message
		if(printToStdOut){
			printf("%s\n", buff);
		}
		if(printToFile){
			FILE * fp = fopen("fswOutput.txt", "a");
			fprintf(fp, "%s\n", buff);
			fclose(fp);
		}
	}
}


void Logger::PrintInfo(LoggerLevelType level_in){
	if(printToStdOut){
		printf("(%lld)", getTimeInMillis());
		printf("[TID: %lu] ", pthread_self());

		// Display the log level
		switch(level_in){
		case LOGGER_LEVEL_SUPER_DEBUG:
			printf("\x1b[37m" "SUPER:    " "\x1b[0m");
			break;
		case LOGGER_LEVEL_DEBUG:
			printf("DEBUG:    ");
			break;
		case LOGGER_LEVEL_INFO:
			printf("\x1b[34m" "INFO:     " "\x1b[0m");
			break;
		case LOGGER_LEVEL_WARN:
			printf("\x1b[33m" "WARN:     " "\x1b[0m");
			break;
		case LOGGER_LEVEL_ERROR:
			printf("\x1b[31m" "ERROR:    " "\x1b[0m");
			break;
		case LOGGER_LEVEL_FATAL:
			printf("\x1b[35m" "FATAL:    " "\x1b[0m");
			break;
		default:
			// maybe add assert on bounds
			break;
		}
	}

	if(printToFile){
		FILE * fp = fopen("fswOutput.txt", "a");
		fprintf(fp, "(%lld)", getTimeInMillis());
		fprintf(fp, "[TID: %lu] ", pthread_self());

		// Display the log level
		switch(level_in){
		case LOGGER_LEVEL_SUPER_DEBUG:
			fprintf(fp, "\x1b[37m" "SUPER:    " "\x1b[0m");
			break;
		case LOGGER_LEVEL_DEBUG:
			fprintf(fp, "DEBUG:    ");
			break;
		case LOGGER_LEVEL_INFO:
			fprintf(fp, "\x1b[34m" "INFO:     " "\x1b[0m");
			break;
		case LOGGER_LEVEL_WARN:
			fprintf(fp, "\x1b[33m" "WARN:     " "\x1b[0m");
			break;
		case LOGGER_LEVEL_ERROR:
			fprintf(fp, "\x1b[31m" "ERROR:    " "\x1b[0m");
			break;
		case LOGGER_LEVEL_FATAL:
			fprintf(fp, "\x1b[35m" "FATAL:    " "\x1b[0m");
			break;
		default:
			// maybe add assert on bounds
			break;
		}
		fclose(fp);
	}
}
