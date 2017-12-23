/*
 * FSWThreads.h
 *
 *  Created on: Dec 22, 2017
 *      Author: adam
 */

#ifndef INCLUDE_FSWTHREADS_H_
#define INCLUDE_FSWTHREADS_H_

#include <unistd.h>
#include <stdio.h>
#include "core/Watchdog.h"

struct ModeManagerStruct{
	Sequence seq;
	Watchdog* watchdog;
};

class FSWThreads
{

public:

	static void * GetHealthStatusThread(void * args);
	static void * ModeManagerThread(void * args);
	static void * GPSManagerThread(void * args);
	static void * FileManagerThread(void * args);
	static void * WatchdogThread(void * args);
};

#endif /* INCLUDE_FSWTHREADS_H_ */
