/*
 * SubPowerInterface.h
 *
 *  Created on: Dec 29, 2017
 *      Author: cyborg9
 */

#ifndef SUBPOWERINTERFACE_H_
#define SUBPOWERINTERFACE_H_

#include <stdint.h>
#include "hal/GPIOManager.h"
#include "util/Logger.h"

class SubPowerInterface{
public:
	SubPowerInterface(GPIOManager& gpioman, int powerid, int resetid, int faultid, std::string instance);
	MOCK ~SubPowerInterface();

	MOCK void configDelay(size_t resetdelay, size_t startupdelay);

	MOCK void powerOff();
	MOCK void powerOn();
	MOCK void reset();

	MOCK bool faultOccurred();
private:
	//! Minimum time to assert reset [ms]
	size_t resetdelay;
	//! Minimum time to wait after deasserting reset [ms]
	size_t startupdelay;
	//! Reference to gpio manager
	GPIOManager& gpioman;
	//! The power line gpio
	int powerid;
	//! The reset line gpio
	int resetid;
	//! The fault line gpio
	int faultid;

	//! Tags to use for logging
	LogTags tags;
};




#endif /* SUBPOWERINTERFACE_H_ */
