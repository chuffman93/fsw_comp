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

class Logger : public Phoenix::Core::Singleton{
	 /*! \brief Declare Factory a friend class
	 *
	 *      This allows factory to call FileHandler's private constructor
	 */
	friend class Phoenix::Core::Factory;
	public:
		const static LoggerLevelType threshold;
		void Log(char * message, LoggerLevelType level_in);
	private:
		static void Initialize(void);
		bool IsFullyInitialized(void);
		Logger(void);
		~Logger(void);
};

#endif

// --------------------- Below is working class -------------------------------

//#ifndef _LOGGER_H
//#define _LOGGER_H
//
//#include "core/StdTypes.h"
//
//class Logger{
//	public:
//		Logger(void);
//		const static LoggerLevelType threshold;
//		void Log(char * message, LoggerLevelType level_in);
//};
//
//#endif
