/*
 * HotSwapInterface.h
 *
 *  Created on: Dec 29, 2017
 *      Author: cyborg9
 */

#ifndef HOTSWAPINTERFACE_H_
#define HOTSWAPINTERFACE_H_

#include "hal/I2CManager.h"
#include "util/Logger.h"


struct HotSwap{
	HotSwap(int address, float senseR, std::string instance);
	int address;
	float senseR;
	std::string instance;
};

struct HotSwapData{
	float current;
	float voltage;
};

class HotSwapInterface{
public:
	HotSwapInterface(I2CManager& i2cman, int id, float senseR, std::string instance);
	~HotSwapInterface();

	HotSwapData getData();
	void configure();

private:
	//! The reference to the i2c manager
	I2CManager& i2cman;
	//! The id of the i2c device
	int id;
	//! The value of the sense resistor to use for current conversion
	float senseR;
	//! Tags for logging
	LogTags tags;
};

#endif /* HOTSWAPINTERFACE_H_ */
