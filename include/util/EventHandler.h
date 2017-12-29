/*
 * EventHandler.h
 *
 *  Created on: Dec 23, 2017
 *      Author: fsw
 */

#ifndef EVENTHANDLER_H_
#define EVENTHANDLER_H_
#include "Logger.h"

class EventHandler{
public:
	static void event(LogLevel level, std::string message){}
	static void attachLogger(Logger* logger){}
private:
	static Logger* logger;
};



#endif /* EVENTHANDLER_H_ */
