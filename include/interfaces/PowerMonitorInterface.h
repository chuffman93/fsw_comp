/*
 * PowerMonitorInterface.h
 *
 *  Created on: Dec 29, 2017
 *      Author: cyborg9
 */

#ifndef POWERMONITORINTERFACE_H_
#define POWERMONITORINTERFACE_H_

#include "hal/I2CManager.h"

struct PowerMonitorData{
	float current;
	float voltage;
};

class PowerMonitorInterface{
public:
	PowerMonitorInterface(I2CManager& i2cman, int id, float senseR, std::string instance);
	~PowerMonitorInterface();

	PowerMonitorData getData();
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



#endif /* POWERMONITORINTERFACE_H_ */
