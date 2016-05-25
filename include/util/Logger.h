/*
 * Logger.h
 *
 *  Created on: May 24, 2016
 *      Author: Alex St. Clair
 */

#ifndef LOGGER_H_
#define LOGGER_H_

#include "core/StdTypes.h"
#include "core/Singleton.h"

using namespace Phoenix::Core;

typedef enum{
	LEVEL_MIN = 1,
	LEVEL_DEBUG = LEVEL_MIN,
	LEVEL_INFO,
	LEVEL_WARNING,
	LEVEL_ERROR,
	LEVEL_CRITICAL,
}Logger_Level;

class Logger : public Singleton
{
	/*! \brief Declare Factory a friend class
	*
	*	This allows factory to call Logger's private constructor
	*/
	friend class Factory;
public:
	void Log(char * message, Logger_Level level_in);
protected:
	Logger(void);
private:
	virtual ~Logger(void);
	static void Initialize(void);
//#ifdef TEST
//			static void Destroy(void);
//#endif
	bool IsFullyInitialized(void);
};

#endif /* LOGGER_H_ */
