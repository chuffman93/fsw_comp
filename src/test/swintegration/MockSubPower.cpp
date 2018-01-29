/*
 * MockSubPower.cpp
 *
 *  Created on: Jan 29, 2018
 *      Author: cyborg9
 */

#include "test/swintegration/MockSubPower.h"
static GPIOManager dummyg("");


MockSubPower::MockSubPower(std::string instance)
:SubPowerInterface(dummyg, -1, -1, -1, "")
{
	tags += LogTag("Name", "MockSubPower");
	tags += LogTag("Instance", instance);
}
MockSubPower::~MockSubPower(){}

void MockSubPower::configDelay(size_t resetdelay, size_t startupdelay){
	Logger::Stream(LEVEL_INFO, tags) << "Setting resetdelay: " << resetdelay << " startupdelay: " << startupdelay;
}

void MockSubPower::powerOff(){
	Logger::log(LEVEL_INFO, tags, "Power Off");
}
void MockSubPower::powerOn(){
	Logger::log(LEVEL_INFO, tags, "Power On");
}
void MockSubPower::reset(){
	Logger::log(LEVEL_INFO, tags, "Resetting");
}

bool MockSubPower::faultOccurred(){
	return false;
}

