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
#include <iomanip>
#include <unistd.h>
#include "util/EventHandler.h"
using namespace std;

// NOTE: Consider not remapping linux SPI enum

/*
 * \param busbase the root path to the spi device
 * \param mode the spi mode to be used (0-4)
 * \param speed the speed the bus should run on [Hz]
 */
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

/*!
 * Handles the initialization of a single SPI device
 * Does ioctl calls to set the mode and speed
 * \param device the device to initialize
 */
void SPIManager::initializeDevice(SPIDevice& device){
	stringstream ss;
	ss << "[SPIManager] Initializing device on bus \"" << busbase <<"\" with cs " << device.ss;
	EventHandler::event(LEVEL_INFO, ss.str());

	stringstream devstring;
	devstring << busbase << '.' << device.ss;
	device.fd = open(devstring.str().c_str(), O_RDWR); //TODO: Check fd for error


	//TODO: error checking on return values?
	ioctl(device.fd, SPI_IOC_WR_MODE, &mode);
	ioctl(device.fd, SPI_IOC_RD_MODE, &mode);
	ioctl(device.fd,SPI_IOC_WR_MAX_SPEED_HZ, &speed);
	ioctl(device.fd,SPI_IOC_RD_MAX_SPEED_HZ, &speed);
	// TODO: consider logging registration
}

/*!
 * Sends a single byte to the specified device
 * \param id the id of the device to send the byte to
 * \param byte the byte to be sent
 */
void SPIManager::sendbyte(int id, uint8_t byte){
	LockGuard l(lock);
	SPIDevice dev = getDevice(id);
	stringstream ss;
	ss << "[SPIManager][CS: " << dev.ss <<"] Sending byte: 0x" << hex <<  setfill('0') << setw(2) << (unsigned int)byte;
	EventHandler::event(LEVEL_DEBUG, ss.str());
	write(dev.fd, &byte, 1);
}

/*!
 * Reads a single byte from the specified device
 * \param id the id of the device to be read from
 * \return the byte that was reads
 */
uint8_t SPIManager::receivebyte(int id){
	LockGuard l(lock);
	SPIDevice dev = getDevice(id);
	uint8_t rx_byte = 0;
	read(dev.fd, &rx_byte, 1);
	stringstream ss;
	ss << "[SPIManager][CS: " << dev.ss <<"] Received byte: 0x" << hex <<  setfill('0') << setw(2) << (unsigned int)rx_byte;
	EventHandler::event(LEVEL_DEBUG, ss.str());
	return rx_byte;
}

