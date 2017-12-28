/*
 * I2CManager.cpp
 *
 *  Created on: Dec 27, 2017
 *      Author: cyborg9
 */

#include <unistd.h>
#include <fcntl.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include "hal/I2CManager.h"
#include "util/EventHandler.h"

using namespace std;

/*!
 * Initializes the I2CManager with the provided device path
 * \param file the path the file corrisponding to the i2c bus
 */
I2CManager::I2CManager(string file)
:devfilename(file)
{}

I2CManager::~I2CManager(){

}

/*!
 * Attaches an I2C device to the bus
 * \param addr the 8-bit I2C address of the device
 * \return the device id
 */
int I2CManager::attachDevice(uint8_t addr){
	I2CDevice dev;
	dev.address = addr >> 1; //Use the 7-bit I2C address
	return BusManager<I2CDevice>::attachDevice(dev);
}

//! Initializes the I2C bus
void I2CManager::initialize(){
	EventHandler::event(LEVEL_INFO, "[I2CManager] Initializing bus " + devfilename);
	BusManager<I2CDevice>::initializeDevices();
}

//! Initialize an I2C Device (currently does nothing)
void I2CManager::initializeDevice(I2CDevice& dev){
	//TODO: add info into this shit
	EventHandler::event(LEVEL_INFO, "[I2CManager] Initializing device");
}

/*!
 * Handles SMBUS style register transaction
 * \param id the id of the i2c device
 * \param reg the register to write to
 * \param value the value to be written
 */
void I2CManager::writeReg(int id, uint8_t reg, uint8_t value){
	vector<uint8_t> data;
	data.push_back(reg);
	data.push_back(value);
	writeRaw(id, data);
}

/*!
 * Handles SMBUS style register transaction
 * \param id the id of the i2c device
 * \param reg the register to read from
 * \return the value read
 */
uint8_t I2CManager::readReg(int id, uint8_t reg){
	vector<uint8_t> data;
	data.push_back(reg);
	writeRaw(id, data);
	data = readRaw(id, 1);
	return data[0];
}

/*!
 * Handles a raw write of data on the i2c bus
 * \param id the id of the device
 * \param data the data to be written
 */
void I2CManager::writeRaw(int id, vector<uint8_t> data){
	LockGuard l(lock);
	I2CDevice& dev = BusManager<I2CDevice>::getDevice(id);

	int fd = open(devfilename.c_str(), O_WRONLY);
	if(fd < 0){
		EventHandler::event(LEVEL_ERROR, "[I2CManager] Unable to open I2C bus " + devfilename);
	}
	if(ioctl(fd, I2C_SLAVE, dev.address) < 0){
		EventHandler::event(LEVEL_WARN, "[I2CManager] Unable to select i2c device");
	}

	int retval = write(fd, &data[0], data.size());
	close(fd);

	if(retval != (signed)data.size()){
		EventHandler::event(LEVEL_WARN, "[I2CManager] Unable to write correct number of bytes to i2c device");
	}
}

/*!
 * Handles a raw read of data on the i2c bus
 * \param id the id of the device
 * \return the data read
 */
vector<uint8_t> I2CManager::readRaw(int id, size_t len){
	LockGuard l(lock);
	I2CDevice& dev = BusManager<I2CDevice>::getDevice(id);

	int fd = open(devfilename.c_str(), O_WRONLY);
	if(fd < 0){
		EventHandler::event(LEVEL_ERROR, "[I2CManager] Unable to open I2C bus " + devfilename);
	}
	if(ioctl(fd, I2C_SLAVE, dev.address) < 0){
		EventHandler::event(LEVEL_WARN, "[I2CManager] Unable to select i2c device");
	}

	vector<uint8_t> data;
	for(size_t i = 0; i < len; i++){
		uint8_t byte;
		int retval = read(fd, &byte, 1);

		if(retval != 1){
			EventHandler::event(LEVEL_WARN, "[I2CManager] Unable to read correct number of bytes from i2c device");
		}
		data.push_back(byte);
	}
	close(fd);
	return data;
}
