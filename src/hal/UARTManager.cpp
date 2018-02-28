/*
 * UARTManager.cpp
 *
 *  Created on: Dec 27, 2017
 *      Author: cyborg9
 */

#include <termios.h>
#include <fcntl.h>
#include <unistd.h>
#include <sstream>
#include "hal/UARTManager.h"


/*!
 * Initializes the UART Manager with the passed device
 * \param filename the filepath to the uart device in the linux filesystem
 */
UARTManager::UARTManager(std::string filename)
:filename(filename), fd(-1)
{
	tags += LogTag("Name", "UARTManager");
}

UARTManager::~UARTManager(){
	closefd();
}

//! Initializes the uart device
void UARTManager::initialize(){
	LockGuard l(lock);
	Logger::Stream(LEVEL_INFO, tags) << "Initializing UART on \"" << filename << "\"";
	struct termios port;
	fd = open(filename.c_str(), O_RDWR | O_NOCTTY | O_NONBLOCK);

	if(fd == -1){
		Logger::Stream(LEVEL_ERROR, tags) << "File \"" + filename + "\" not found";
	}

	if(tcgetattr(fd, &port) < 0) {
		Logger::Stream(LEVEL_ERROR, tags) << "Problem getting port attributes for \"" << filename << "\"";
	}
	port.c_iflag &= ~IXON;
	port.c_lflag &= ~ECHO;

	if(cfsetispeed(&port, B115200) < 0 || cfsetospeed(&port, B115200) < 0){
		Logger::Stream(LEVEL_ERROR, tags) << "Problem setting baud rate for \"" << filename << "\"";
	}

	if(tcsetattr(fd, TCSANOW, &port) < 0){
		Logger::Stream(LEVEL_ERROR, tags) << "Problem setting port attributes for \"" << filename << "\"";
	}
}

/*!
 * Read specified number bytes from the UART
 * \param len The number of bytes to read
 * \return vector containing the pending bytes
 */
std::vector<uint8_t> UARTManager::readData(size_t len){
	LockGuard l(lock);
	std::vector<uint8_t> data(len);
	if((size_t) read(fd, &data[0], len) != len){
		//TODO: Error Handling
	}
	return data;
}

/*!
 * Read single byte from uart
 * \return the byte
 */
uint8_t UARTManager::readData(){
	LockGuard l(lock);
	uint8_t rxbyte;
	if(read(fd, &rxbyte, 1) != 1){
		Logger::Stream(LEVEL_DEBUG, tags) << "Invalid number of bytes read";
		return 0;
	}
	return rxbyte;
}


/*!
 * Send data over the UART
 * \param data the data to be sent
 */
void UARTManager::writeData(std::vector<uint8_t> data){
	LockGuard l(lock);
	if((size_t) write(fd, &data[0], data.size()) != data.size()){
		//TODO: Error Handling
	}
}

void UARTManager::closefd(){
	close(fd);
}
