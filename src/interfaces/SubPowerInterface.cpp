/*
 * SubPowerInterface.cpp
 *
 *  Created on: Dec 29, 2017
 *      Author: cyborg9
 */

#include "interfaces/SubPowerInterface.h"
#include <unistd.h>

SubPowerInterface::SubPowerInterface(GPIOManager& gpioman, int powerid, int resetid, int faultid, std::string instance)
:gpioman(gpioman), powerid(powerid), resetid(resetid), faultid(faultid){
	resetdelay = 0;
	startupdelay = 0;
	tags += LogTag("Name", "SubPower");
	tags += LogTag("Instance", instance);
}

SubPowerInterface::~SubPowerInterface(){}

void SubPowerInterface::configDelay(size_t resetdelay, size_t startupdelay){
	this->resetdelay = resetdelay;
	this->startupdelay = startupdelay;
}

void SubPowerInterface::powerOff(){
	Logger::log(LEVEL_INFO, tags, "Powering off");
	gpioman.set(resetid, LOW);
	gpioman.set(powerid, LOW);
}
void SubPowerInterface::powerOn(){
	Logger::log(LEVEL_INFO, tags, "Powering on");
	gpioman.set(resetid, LOW);
	gpioman.set(powerid, HIGH);
	usleep(resetdelay*1000);
	gpioman.set(resetid, HIGH);
	usleep(startupdelay*1000);
}
void SubPowerInterface::reset(){
	Logger::log(LEVEL_INFO, tags, "Resetting...");
	gpioman.set(resetid, LOW);
	usleep(resetdelay*1000);
	gpioman.set(resetid, HIGH);
	usleep(startupdelay*1000);
}


