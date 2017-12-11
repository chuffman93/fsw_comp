/*
 * UARTManager.h
 *
 *  Created on: Dec 4, 2017
 *      Author: cyborg9
 */

#ifndef UARTMANAGER_H_
#define UARTMANAGER_H_

#include "hal/HardwareManager.h"
#include <string>
#include <vector>

/*!
 * Manages a specific UART device, and provides raw read/write methods
 */
class UARTManager: public HardwareManager{
public:
	UARTManager(std::string filename);
	~UARTManager();

	MOCK void initialize();

	MOCK std::string readbuffer();
	MOCK void writebuffer(std::string data);

private:
	//! Holds the filename corrisponding to the UART
	std::string filename;
	//! Holds the file descriptor corrisponding to filename. Created in initialize
	int fd;
	//! Mutex for the device
	Lock lock;
};



#endif /* UARTMANAGER_H_ */
