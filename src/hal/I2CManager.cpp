/*
 * I2CManager.cpp
 *
 *  Created on: Dec 27, 2017
 *      Author: cyborg9
 */

#include <sstream>
#include <unistd.h>
#include <fcntl.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <iomanip>
#include "hal/I2CManager.h"

using namespace std;

/*!
 * Initializes the I2CManager with the provided device path
 * \param file the path the file corrisponding to the i2c bus
 */
I2CManager::I2CManager(string file)
:devfilename(file)
{
	tags += LogTag("Name", "I2CManager");
}

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
	Logger::Stream(LEVEL_INFO, tags) << "Initializing bus " << devfilename;
	BusManager<I2CDevice>::initializeDevices();
}

//! Initialize an I2C Device (currently does nothing)
void I2CManager::initializeDevice(I2CDevice& dev){
	Logger::Stream(LEVEL_INFO, tags) << "Initializing device with address 0x"
			 << hex <<  setfill('0') << setw(2) << (dev.address << 1); //Display the 8-bit address
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
		Logger::Stream(LEVEL_ERROR, tags) << "Unable to open I2C bus " << devfilename;
	}
	if(ioctl(fd, I2C_SLAVE, dev.address) < 0){
		Logger::Stream(LEVEL_WARN, tags) << "Unable to select i2c device on address "
				 << hex <<  setfill('0') << setw(2) << (dev.address << 1); //Display the 8-bit address
	}

	int retval = write(fd, &data[0], data.size());
	close(fd);

	if(retval != (signed)data.size()){
		Logger::Stream(LEVEL_WARN, tags) << "Write failed. Wrote " << retval << " not " << data.size();
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

	int fd = open(devfilename.c_str(), O_RDONLY);
	if(fd < 0){
		Logger::Stream(LEVEL_ERROR, tags) << "Unable to open I2C bus " << devfilename;
	}
	if(ioctl(fd, I2C_SLAVE, dev.address) < 0){
		Logger::Stream(LEVEL_WARN, tags) << "Unable to select i2c device on address "
				 << hex <<  setfill('0') << setw(2) << (dev.address << 1); //Display the 8-bit address
	}

	vector<uint8_t> data(len);
	int retval = read(fd, &data[0], len);
	if(retval != 1){
		Logger::Stream(LEVEL_WARN, tags) << "Read failed. read " << retval << " not " << len;
	}

	close(fd);
	return data;
}
