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
template <typename device> class BusManager: public HardwareManager{
public:
	BusManager():devices(){}
	virtual ~BusManager(){}

protected:
	/*!
	 * Adds the device into the registered devices
	 * Assumes that mutex is not locked when called
	 * \param device the device struct to be added
	 * \return the id of the added device
	 */
	int attachDevice(device& dev){
		Monitor monitor(this->lock);
		devices.push_back(dev);
		return devices.size() - 1;
	}
	/*!
	 * Grabs the device corrisponding to the id
	 * Assumes that mutex is locked when called
	 * \param id the id of the device to be return
	 * \return the device corrisponding to the id
	 */
	device& getDevice(int id) {
		return devices.at(id);
	}

private:
	//! The vector containing all registered devices
	std::vector<device> devices;
	LockClass lock;
};



#endif /* BUSMANAGER_H_ */
