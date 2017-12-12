/*
 * SPIManager.h
 *
 *  Created on: Dec 2, 2017
 *      Author: fsw
 */

#ifndef SPIMANAGER_H_
#define SPIMANAGER_H_
#include "hal/BusManager.h"
#include "core/Lock.h"
#include <string>
#include <stdint.h>


/*!
 * Contains all of the information relavent to a single device on the spi bus
 */
struct SPIDevice{
	//! File Descriptor for the spi device
	int fd;
};

/*!
 * Handles a spi bus through the linux files system interface
 */
class SPIManager: public BusManager<SPIDevice>{
public:
	SPIManager(std::string spibusbase, uint8_t mode, uint32_t speed);
	~SPIManager();

	MOCK int attachDevice(int ss);
	MOCK void initialize();
	void initializeDevice(SPIDevice& dev);

	MOCK void sendbyte(int id, uint8_t byte);
	MOCK uint8_t receivebyte(int id);

private:
	//! Base name for the spi bus (spi devices show up as <base>.x in linux)
	std::string busbase;

	//! SPI Mode (0-3)
	uint8_t mode;
	//! Bus speed in Hz
	uint32_t speed;
	//! Mutex for the bus
	Lock lock;
};




#endif /* SPIMANAGER_H_ */