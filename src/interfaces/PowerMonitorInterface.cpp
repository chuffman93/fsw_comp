/*
 * PowerMonitorInterface.cpp
 *
 *  Created on: Dec 29, 2017
 *      Author: cyborg9
 */

#include "interfaces/PowerMonitorInterface.h"

/*!
 * Creates a power monitor interface to the given i2c device
 * \param i2cman the i2c bus
 * \param id the id of the device
 * \param instance A unique string for the instance
 */
PowerMonitorInterface::PowerMonitorInterface(I2CManager& i2cman, int id, float senseR, std::string instance)
:i2cman(i2cman), id(id), senseR(senseR)
{
	tags += LogTag("Name", "PowerMonitor");
	tags += LogTag("Instance", instance);
}

PowerMonitorInterface::~PowerMonitorInterface(){}

//! Get the current data from the power monitor
PowerMonitorData PowerMonitorInterface::getData(){
	PowerMonitorData retval;
	uint16_t voltRaw = ((uint16_t)i2cman.readReg(id, 0x1E) << 4) | ((i2cman.readReg(id, 0x1F)>>4)&0x0F);
	uint16_t currRaw = ((uint16_t)i2cman.readReg(id, 0x14) << 4) | ((i2cman.readReg(id, 0x15)>>4)&0x0F);

	retval.voltage = (float)voltRaw * (25.0/1000.0); //25mV to convert to V
	retval.current = (float)currRaw *(25.0/1000000.0)*(1/senseR); //25uV * 1/SenseResistor to convert to A

	Logger::Stream(LEVEL_INFO, tags) << "Read: " << retval.voltage << "V " << retval.current << "A";
	return retval;
}

void PowerMonitorInterface::configure(){
	Logger::log(LEVEL_INFO, tags, "Configuring Power Monitor");
	i2cman.writeReg(id, 0x00, 0x05);
}




