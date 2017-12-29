#ifndef ONEWIREMANAGER_H
#define ONEWIREMANAGER_H

#include <string>
#include "hal/BusManager.h"
#include "core/Lock.h"
#include "util/Logger.h"

/*!
 * Contains all of the information relavent to a single device on the one wire bus
 */
struct OneWireDevice{
	//! The address of the device on the one wire bus
	std::string devname;

	//! Allows for detection of duplicate devices
	bool operator ==(const OneWireDevice dev){return devname == dev.devname;}
};


/*!
 * Handles a One Wire bus through the linux files system interface
 */
class OneWireManager: public BusManager<OneWireDevice>{
public:
	OneWireManager(std::string busbase);
	~OneWireManager();

	MOCK int attachDevice(std::string devname);
	MOCK void initialize();
	void initializeDevice(OneWireDevice& dev);

	MOCK void writeToFile(int id, std::string filename, std::string data);
	MOCK std::string readFromFile(int id, std::string filename);

private:
	//! File for the one wire bus
	std::string busbase;
	//! Mutex for the bus
	Lock lock;

	//! Tags for logging
	LogTags tags;
};

#endif
