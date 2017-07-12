/*
 * 	Logger.h
 *
 * 	Created on: May 26, 2016
 * 	    Author: Alex St. Clair
 */

#ifndef _LOGGER_H
#define _LOGGER_H

#include "core/StdTypes.h"
#include "core/Singleton.h"
#include "core/Factory.h"
#include <string>

using namespace std;
using namespace AllStar::Core;

class Logger : public AllStar::Core::Singleton{
	 /*! \brief Declare Factory a friend class
	 *      This allows factory to call Logger's private constructor
	 */
	friend class AllStar::Core::Factory;
	public:
		static LoggerLevelType threshold;
		const static bool printToFile;
		const static bool printToStdOut;

		void SuperDebug(const char * fmt, ...);
		void Debug(const char * fmt, ...);
		void Info(const char * fmt, ...);
		void Warning(const char * fmt, ...);
		void Error(const char * fmt, ...);
		void Fatal(const char * fmt, ...);

		void PrintInfo(LoggerLevelType level_in);
	private:
		void Log(LoggerLevelType level_in, char const * fmt, va_list args);
		bool IsFullyInitialized(void);
		Logger(void);
		~Logger(void);
};

#endif
