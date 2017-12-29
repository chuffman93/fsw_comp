/*
 * GPIOManager.h
 *
 *  Created on: Dec 2, 2017
 *      Author: fsw
 */

#ifndef GPIOMANAGER_H_
#define GPIOMANAGER_H_
#include <poll.h>
#include <stdint.h>
#include <string>
#include "hal/BusManager.h"
#include "core/Lock.h"
#include "util/Logger.h"

//TODO Consider typedefs

enum GPIODirection{
	GPIO_INPUT,
	GPIO_OUTPUT,
};

enum GPIOEdge{
	INT_FALLING,
	INT_RISING,
	INT_NONE,
};

enum GPIOLevel{
	HIGH,
	LOW,
};

struct GPIODevice{
	//! Whether the GPIO is muxed as an input or output
	GPIODirection direction;
	//! If the GPIO is  edge sensitive, what edge
	GPIOEdge edge;
	//! The unique number that corrisponds to the gpio
	int gpionum;
	//! The name of the GPIO folder in the linux fs
	std::string gpioref;
	//! If edge triggered, contains the poll file descriptor
	pollfd pfd;
	//! Mutex for each GPIO
	Lock lock;

	//! Allows for detection of duplicate devices
	bool operator ==(const GPIODevice& dev){ return gpionum == dev.gpionum;}
};

/*!
 * Handles the gpios on the SAMA5 through the linux file system API
 */
class GPIOManager: public BusManager<GPIODevice>{
public:
	GPIOManager(std::string gpiobase);
	~GPIOManager();

	MOCK void initialize();
	MOCK int attachDevice(char bank, int pin, GPIODirection dir);
	MOCK int attachDevice(char bank, int pin, GPIOEdge edge);

	MOCK GPIOLevel get(int id);
	MOCK void set(int id, GPIOLevel level);
	MOCK bool wait(int id, uint32_t timeout);

private:
	void initializeDevice(GPIODevice& dev);
	//! Contains the path to the gpio directory
	std::string gpiobase;

	//! Tags to use when logging
	LogTags tags;
};


#endif /* GPIOMANAGER_H_ */
