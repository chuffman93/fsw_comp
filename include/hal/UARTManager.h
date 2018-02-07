/*
 * UARTManager.h
 *
 *  Created on: Dec 4, 2017
 *      Author: cyborg9
 */

#ifndef UARTMANAGER_H_
#define UARTMANAGER_H_

#include <string>
#include <vector>
#include <stdint.h>
#include "hal/HardwareManager.h"
#include "core/Lock.h"
#include "util/Logger.h"

/*!
 * Manages a specific UART device, and provides raw read/write methods
 */
class UARTManager: public HardwareManager{
public:
	UARTManager(std::string filename);
	~UARTManager();

	MOCK void initialize();

	MOCK std::vector<uint8_t> readData(size_t len);
	MOCK uint8_t readData();
	MOCK void writeData(std::vector<uint8_t> data);

private:
	void closefd();
	//! Holds the filename corrisponding to the UART
	std::string filename;
	//! Holds the file descriptor for the UART
	int fd;
	//! Mutex for the device
	Lock lock;

	//! Tags for logging
	LogTags tags;
};



#endif /* UARTMANAGER_H_ */
