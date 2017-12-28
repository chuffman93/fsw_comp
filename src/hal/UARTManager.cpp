/*
 * UARTManager.cpp
 *
 *  Created on: Dec 27, 2017
 *      Author: cyborg9
 */

#include <termios.h>
#include "hal/UARTManager.h"
#include "util/EventHandler.h"
#include <fcntl.h>
#include <unistd.h>
#include <sstream>


/*!
 * Initializes the UART Manager with the passed device
 * \param filename the filepath to the uart device in the linux filesystem
 */
UARTManager::UARTManager(std::string filename)
:filename(filename), fd(-1)
{}

UARTManager::~UARTManager(){
	closefd();
}

//! Initializes the uart device
void UARTManager::initialize(){
	LockGuard l(lock);
	EventHandler::event(LEVEL_INFO, "[UART Manager] Initializing UART on " + filename);
	struct termios port;
	fd = open(filename.c_str(), O_RDWR | O_NOCTTY | O_NDELAY);

	if(fd == -1){
		EventHandler::event(LEVEL_ERROR, "[UART Manager] File " + filename + "Not found");
	}

	if(tcgetattr(fd, &port) < 0) {
		EventHandler::event(LEVEL_ERROR, "[UART Manager] Problem getting port attributes for " + filename);
	}
	port.c_iflag &= ~IXON;
	port.c_lflag = 0;

	if(cfsetispeed(&port, B115200) < 0 || cfsetospeed(&port, B115200) < 0){
		EventHandler::event(LEVEL_ERROR, "[UART Manager] Problem setting baud rate for " + filename);
	}

	if(tcsetattr(fd, TCSANOW, &port) < 0){
		EventHandler::event(LEVEL_ERROR, "[UART Manager] Problem setting port attributes for " + filename);
	}
}

/*!
 * Get all pending bytes from the UART
 * \return vector containing the pending bytes
 */
std::vector<uint8_t> UARTManager::readData(){
	LockGuard l(lock);
	uint8_t rxbyte;
	std::vector<uint8_t> data;
	while(read(fd,&rxbyte,1) == 1){
		std::stringstream ss;
		ss << ("[UARTManager] Received byte ");
		ss << rxbyte;
		EventHandler::event(LEVEL_DEBUG, ss.str());
		data.push_back(rxbyte);
	}
	return data;
}

/*!
 * Send data over the UART
 * \param data the data to be sent
 */
void UARTManager::writeData(std::vector<uint8_t> data){
	LockGuard l(lock);
	for(std::vector<uint8_t>::iterator i = data.begin(); i != data.end(); i++){
		uint8_t txbyte = *i;
		write(fd, &txbyte, 1);
	}
}

void UARTManager::closefd(){
	close(fd);
}
