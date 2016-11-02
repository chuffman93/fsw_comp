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
		const static LoggerLevelType threshold;

		void Log(char const * message, LoggerLevelType level_in, LogFile file_out);
		void Log(string message, LoggerLevelType level_in, LogFile file_out);
		void Log(char const * message, int int_in, LoggerLevelType level_in, LogFile file_out);
		void Log(char const * message, uint32 uint_in, LoggerLevelType level_in, LogFile file_out);
		void Log(char const * message, float float_in, LoggerLevelType level_in, LogFile file_out);
		void Log(char const * message, const char * char_in, LoggerLevelType level_in, LogFile file_out);
		void Log(char const * message, string str_in, LoggerLevelType level_in, LogFile file_out);
		void Log(string message, string str_in, LoggerLevelType level_in, LogFile file_out);

		void GetInfo(LoggerLevelType level_in);
	private:
		static void Initialize(void);
		bool IsFullyInitialized(void);
		Logger(void);
		~Logger(void);
};

#endif
