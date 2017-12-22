/*
 * BusManager.h
 *
 *  Created on: Dec 2, 2017
 *      Author: fsw
 */

#ifndef BUSMANAGER_H_
#define BUSMANAGER_H_

#include "hal/HardwareManager.h"
#include <assert.h>
#include <vector>
#include <algorithm>

// Look into typedefing types; i.e. ID types
// Maybe add strings to assets?


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
		//Assert that the device is not already in the list of devices
		assert(std::find(devices.begin(), devices.end(), dev) == devices.end());
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
		assert(id >= 0);
		assert((unsigned int) id < devices.size());
		return devices.at(id);
	}

	/*!
	 * Helper function to go through the lists of devices and call initializeDevice on them
	 */
	void initializeDevices(){
		for(typename std::vector<device>::iterator i = devices.begin(); i < devices.end(); i++){
			initializeDevice(*i);
		}
	}

	/*!
	 * Virtual method to initialize a device
	 * Should be overriden by the inheireting class if something needs
	 * to be done here.
	 * By default does nothing
	 * \param dev the device to be initialized
	 */
	virtual void initializeDevice(device& dev){};

private:
	//! The vector containing all registered devices
	std::vector<device> devices;
};



#endif /* BUSMANAGER_H_ */
