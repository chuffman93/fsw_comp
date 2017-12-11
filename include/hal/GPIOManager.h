/*
 * GPIOManager.h
 *
 *  Created on: Dec 2, 2017
 *      Author: fsw
 */

#ifndef GPIOMANAGER_H_
#define GPIOMANAGER_H_
#include "hal/BusManager.h"
#include <string>

enum GPIODirection{
	GPIO_INPUT,
	GPIO_OUTPUT,
};

enum GPIOEdge{
	INT_FALLING,
	INT_RISING,
	NONE,
};

enum GPIOLevel{
	HIGH,
	LOW,
};

struct GPIODevice{
	//! Whether the GPIO is muxed as an input or output
	GPIODirection type;
	//! If the GPIO is  edge sensitive, what edge
	GPIOEdge edge;
	//! The name of the GPIO folder in the linux fs
	std::string gpioref;
	//! Mutex for each GPIO
	Lock lock;
};

/*!
 * Handles the gpios on the SAMA5 through the linux file system API
 */
class GPIOManager: public BusManager<GPIODevice>{
public:
	GPIOManager(std::string gpiobase);
	~GPIOManager();

	virtual void initialize();
	MOCK int attachDevice(char bank, int pin, GPIODirection dir);
	MOCK int attachDevice(char bank, int pin, GPIOEdge edge);

	MOCK GPIOLevel get(int id);
	MOCK void set(int id, GPIOLevel level);
	MOCK void wait(int id, uint32_t timeout);

private:


	//! Contains the path to the gpio directory
	std::string gpiobase;
};


#endif /* GPIOMANAGER_H_ */
