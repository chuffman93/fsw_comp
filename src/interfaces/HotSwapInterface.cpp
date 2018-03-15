/*
 * HotSwapInterface.cpp
 *
 *  Created on: Dec 29, 2017
 *      Author: cyborg9
 */

#include "interfaces/HotSwapInterface.h"

/*!
 * Creates a hot swap interface to the given i2c device
 * \param i2cman the i2c bus
 * \param id the id of the device
 * \param instance A unique string for the instance
 */
HotSwapInterface::HotSwapInterface(I2CManager& i2cman, int id, float senseR, std::string instance)
:i2cman(i2cman), id(id), senseR(senseR)
{
	tags += LogTag("Name", "HotSwapInterface");
	tags += LogTag("Instance", instance);
}

HotSwapInterface::~HotSwapInterface(){}

//! Get the current data from the hotswap
HotSwapData HotSwapInterface::getData(){
	HotSwapData retval;
	std::vector<uint8_t> data = i2cman.readRaw(id, 3);
	uint16_t voltRaw = ((uint16_t)data[0]<<4) | (0x0F & (data[2]>>4));
	uint16_t currRaw = ((uint16_t)data[1]<<4) | (0x0F & data[2]);

	retval.voltage = 26.35 * ((float)voltRaw/4096); //Volts
	retval.current = 0.10584 * ((float)currRaw/4096) / senseR; //Current

	Logger::Stream(LEVEL_INFO, tags) << "Read: " << retval.voltage << "V " << retval.current << "A";

/*
 // TEST CODE GRAB STATUS IF COM DOSEN'T POWER ON
	std::vector<uint8_t> data2;
	data2.push_back(0x40);
	Logger::log(LEVEL_INFO, tags, "Read Status");
	i2cman.writeRaw(id, data2);

	std::vector<uint8_t> data3 = i2cman.readRaw(id, 1);
	Logger::Stream(LEVEL_INFO, tags) << "Read: " << (int)data3[0];
*/
	return retval;
}

void HotSwapInterface::configure(){
	std::vector<uint8_t> data;
	data.push_back(0x05);
	Logger::log(LEVEL_INFO, tags, "Configuring HotSwap");
	i2cman.writeRaw(id, data);
}


