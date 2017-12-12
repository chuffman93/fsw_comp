/*
 * SPIManager.cpp
 *
 *  Created on: Dec 5, 2017
 *      Author: cyborg9
 */

#include "hal/SPIManager.h"
#include <linux/spi/spidev.h>
#include <fcntl.h>
#include <sstream>
#include <sys/ioctl.h>
using namespace std;

//! Stub, not necessarily final implementation
SPIManager::SPIManager(string busbase, uint8_t mode, uint32_t speed)
:busbase(busbase), speed(speed)
{
	const uint8_t modes[] = {SPI_MODE_0, SPI_MODE_1, SPI_MODE_2, SPI_MODE_3};
	this->mode = modes[mode];
}

//! Stub, not necessarily final implementation
SPIManager::~SPIManager(){

}

/*!
 * Attach a device to the spi bus definied by the passed slave select
 * \param ss the slave select line to use
 * \return the id of the spi device
 */
int SPIManager::attachDevice(int ss){
	SPIDevice dev;
	dev.ss = ss;
	return BusManager<SPIDevice>::attachDevice(dev);
}

//! Initializes all devices
void SPIManager::initialize(){
	BusManager<SPIDevice>::initializeDevices();
}

//! Stub, not yet implemented
void SPIManager::initializeDevice(SPIDevice& device){
	stringstream ss;
	ss << busbase << '.' << device.ss;
	device.fd = open(ss.str().c_str(), O_RDONLY);

	//TODO: error checking on return values?
	ioctl(device.fd, SPI_IOC_WR_MODE, &mode);
	ioctl(device.fd, SPI_IOC_RD_MODE, &mode);
	ioctl(device.fd,SPI_IOC_WR_MAX_SPEED_HZ, &speed);
	ioctl(device.fd,SPI_IOC_RD_MAX_SPEED_HZ, &speed);
}

//! Stub, not yet implemented
void SPIManager::sendbyte(int id, uint8_t byte){
	LockGuard l(&lock);
	SPIDevice dev = getDevice(id);
	uint8_t rx, tx = byte;
	struct spi_ioc_transfer tr;
	tr.tx_buf = (unsigned long)&tx;
	tr.rx_buf = (unsigned long)&rx;
	tr.len = 1;
	tr.delay_usecs = 0;
	tr.speed_hz = speed;
	tr.cs_change = 0;
	tr.bits_per_word = 8;

	ioctl(dev.fd, SPI_IOC_MESSAGE(1), &tr);
}

//! Stub, not yet implemented
uint8_t SPIManager::receivebyte(int id){
	LockGuard l(&lock);
	SPIDevice dev = getDevice(id);
	uint8_t rx, tx = 0;
	struct spi_ioc_transfer tr;
	tr.tx_buf = (unsigned long)&tx;
	tr.rx_buf = (unsigned long)&rx;
	tr.len = 1;
	tr.delay_usecs = 0;
	tr.speed_hz = speed;
	tr.cs_change = 0;
	tr.bits_per_word = 8;

	ioctl(dev.fd, SPI_IOC_MESSAGE(1), &tr);
	return rx;
}

