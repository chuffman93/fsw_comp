/*
 * SubPowerInterface.cpp
 *
 *  Created on: Dec 29, 2017
 *      Author: cyborg9
 */

#include "interfaces/SubPowerInterface.h"
#include <unistd.h>

/*!
 * Create a sub power interface
 * Pass in -1 for any of the references are unavailable
 */
SubPowerInterface::SubPowerInterface(GPIOManager& gpioman, int powerid, int resetid, int faultid, std::string instance)
:gpioman(gpioman), powerid(powerid), resetid(resetid), faultid(faultid){
	resetdelay = 0;
	startupdelay = 0;
	tags += LogTag("Name", "SubPower");
	tags += LogTag("Instance", instance);
}

SubPowerInterface::~SubPowerInterface(){}

//! Configures the delays using the provided values
void SubPowerInterface::configDelay(size_t resetdelay, size_t startupdelay){
	this->resetdelay = resetdelay;
	this->startupdelay = startupdelay;
}

//! Turn the power off on the subsystem. Will set reset and power low if available
void SubPowerInterface::powerOff(){
	Logger::log(LEVEL_INFO, tags, "Powering off");
	if(resetid != -1) gpioman.set(resetid, LOW);
	if(powerid != -1) gpioman.set(powerid, LOW);
}

//! Turn the power on on the subsystem. Will set reset and power high if available
void SubPowerInterface::powerOn(){
	Logger::log(LEVEL_INFO, tags, "Powering on");
	if(resetid != -1) gpioman.set(resetid, LOW);
	if(powerid != -1) gpioman.set(powerid, HIGH);
	usleep(resetdelay*1000);
	if(resetid != -1) gpioman.set(resetid, HIGH);
	usleep(startupdelay*1000);
}

//! Reset the subsystem. Will use reset and power lines if available
void SubPowerInterface::reset(){
	Logger::log(LEVEL_INFO, tags, "Resetting...");
	if(resetid != -1) gpioman.set(resetid, LOW);
	usleep(resetdelay*1000);
	if(resetid != -1) gpioman.set(resetid, HIGH);
	usleep(startupdelay*1000);
}

//! Returns true if the fault line is asserted
bool SubPowerInterface::faultOccurred(){
	if(faultid != -1){
		return gpioman.get(faultid) == HIGH;
	}
	return false;
}


