/*
 * I2CManager.h
 *
 *  Created on: Dec 4, 2017
 *      Author: cyborg9
 */

#ifndef I2CMANAGER_H_
#define I2CMANAGER_H_
#include <string>
#include <stdint.h>
#include <stdlib.h>
#include <vector>
#include "hal/BusManager.h"
#include "core/Lock.h"
#include "util/Logger.h"

/*!
 * Contains all of the information relavent to a single device on the i2c bus
 */
struct I2CDevice{
	//! I2C address of the device
	uint8_t address;

	//! Allows for detection of duplicate devices
	bool operator ==(const I2CDevice& dev){return address == dev.address;}
};

/*!
 * Handles a i2c bus through the linux files system interface
 */
class I2CManager: public BusManager<I2CDevice>{
public:
	I2CManager(std::string file);
	~I2CManager();

	MOCK int attachDevice(uint8_t addr);
	MOCK void initialize();
	void initializeDevice(I2CDevice& dev);

	uint32_t i2c_smbus_access(int file, char read_write, uint8_t command, int size, union i2c_smbus_data *data);
	uint32_t i2c_smbus_read_word_data(int file, uint8_t command);

	MOCK uint8_t readReg(int id, uint8_t reg);
	MOCK void writeReg(int id, uint8_t reg, uint8_t value);
	MOCK std::vector<uint8_t> readRaw(int id, size_t len);
	MOCK void writeRaw(int id, std::vector<uint8_t> data);

private:
	//! File for the I2C bus
	std::string devfilename;
	//! Mutex for the bus
	Lock lock;

	//! Tags for logging
	LogTags tags;
};






#endif /* I2CMANAGER_H_ */
