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
using namespace Phoenix::Core;

class Logger : public Phoenix::Core::Singleton{
	 /*! \brief Declare Factory a friend class
	 *      This allows factory to call Logger's private constructor
	 */
	friend class Phoenix::Core::Factory;
	public:
		const static LoggerLevelType threshold;
		void Log(char const * message, LoggerLevelType level_in);
		void Log(string message, LoggerLevelType level_in);
	private:
		static void Initialize(void);
		bool IsFullyInitialized(void);
		Logger(void);
		~Logger(void);
};

#endif
