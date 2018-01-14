/*
 * TimeKeeper.cpp
 *
 *  Created on: Jan 6, 2018
 *      Author: alpa3119
 */

#include "util/TimeKeeper.h"
#include<iostream>

uint32_t getCurrentTime(){
	timespec t;
	clock_gettime(CLOCK_REALTIME, &t);
	uint32_t time = t.tv_sec;
	return time;
}

