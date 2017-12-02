/*
 * BusManager.h
 *
 *  Created on: Dec 2, 2017
 *      Author: fsw
 */

#ifndef BUSMANAGER_H_
#define BUSMANAGER_H_

#include "hal/HardwareManager.h"
#include <vector>

/*!
 * Handles a hardware device with a bus, e.g. I2C or SPI
 * Keeps track of registered devices on bus and implements hardware calls
 */
template <struct device> class BusManager: public HardwareManager{
public:
	BusManager();
	virtual ~BusManager();

protected:
	/*!
	 * Adds the device into the registered devices
	 * \param device the device struct to be added
	 * \return the id of the added device
	 */
	virtual int attachDevice(device dev){
		devices.push_back(dev);
		return devices.size() - 1;
	}
	/*!
	 * Grabs the device corrisponding to the id
	 * \param id the id of the device to be return
	 * \return the device corrisponding to the id
	 */
	device& getDevice(int id) {
		return devices.at(id);
	}

private:
	//! The vector containing all registered devices
	std::vector<device> devices;
};



#endif /* BUSMANAGER_H_ */
