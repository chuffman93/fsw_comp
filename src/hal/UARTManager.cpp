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


/*!
 * Initializes the UART Manager with the passed device
 * \param filename the filepath to the uart device in the linux filesystem
 */
UARTManager::UARTManager(std::string filename)
:filename(filename)
{}

UARTManager::~UARTManager(){}

//! Initializes the uart device
void UARTManager::initialize(){
	LockGuard l(lock);
	EventHandler::event(LEVEL_INFO, "[UART Manager] Initializing UART on " + filename);
	struct termios port;
	int portfd = open(filename.c_str(), O_RDWR | O_NOCTTY | O_NDELAY);

	if(portfd == -1){
		EventHandler::event(LEVEL_ERROR, "[UART Manager] File " + filename + "Not found");
	}

	if(tcgetattr(portfd, &port) < 0) {
		EventHandler::event(LEVEL_ERROR, "[UART Manager] Problem getting port attributes for " + filename);
	}
	port.c_iflag &= ~IXON;

	if(cfsetispeed(&port, B115200) < 0 || cfsetospeed(&port, B115200) < 0){
		EventHandler::event(LEVEL_ERROR, "[UART Manager] Problem setting baud rate for " + filename);
	}

	if(tcsetattr(portfd, TCSANOW, &port) < 0){
		EventHandler::event(LEVEL_ERROR, "[UART Manager] Problem setting port attributes for " + filename);
	}

	close(portfd);
}

/*!
 * Get all pending bytes from the UART
 * \return vector containing the pending bytes
 */
std::vector<uint8_t> UARTManager::readData(){
	LockGuard l(lock);
	uint8_t rxbyte;
	int fd = open(filename.c_str(), O_RDONLY);
	std::vector<uint8_t> data;
	while(read(fd,&rxbyte,1) == 1){
		data.push_back(rxbyte);
	}
	close(fd);
	return data;
}

/*!
 * Send data over the UART
 * \param data the data to be sent
 */
void UARTManager::writeData(std::vector<uint8_t> data){
	LockGuard l(lock);
	int fd = open(filename.c_str(), O_WRONLY);
	for(std::vector<uint8_t>::iterator i = data.begin(); i != data.end(); i++){
		uint8_t txbyte = *i;
		write(fd, &txbyte, 1);
	}
	close(fd);
}
