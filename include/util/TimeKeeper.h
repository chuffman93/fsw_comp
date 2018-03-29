/*
 * TimeKeeper.h
 *
 *  Created on: Jan 6, 2018
 *      Author: alpa3119
 */

#ifndef TIMEKEEPER_H_
#define TIMEKEEPER_H_

#include <stdint.h>
#include <iostream>
#include <time.h>

void initializeTime();

uint32_t getCurrentTime();

uint32_t spoofTime(uint32_t addTime);

#endif /* TIMEKEEPER_H_ */
