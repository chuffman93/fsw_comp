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

enum GPIOType{
	GPIO_INPUT,
	GPIO_OUTPUT,
};

enum GPIONumber{
	GPIO1 = 0,
	GPIO2,
	GPIO3,
	NUM_GPIOS
};

enum GPIOBank{
	GPIO_BANK_A = 32*0,
	GPIO_BANK_B = 32*1,
	GPIO_BANK_C = 32*2,
	GPIO_BANK_D = 32*3,
	GPIO_BANK_E = 32*4,
};

enum GPIOLevel{
	HIGH,
	LOW,
};

struct GPIODevice{
	//! Whether the GPIO is muxed as an input or output
	GPIOType type;
	//! The name of the GPIO folder in the linux fs
	std::string gpioref;

	LockClass lock;
};

/*!
 * Handles the gpios on the SAMA5 through the linux file system API
 */
class GPIOManager: public BusManager<GPIODevice>{
public:
	GPIOManager(std::string gpiobase);
	~GPIOManager();

	virtual void initialize();
	MOCK int attachDevice(char bank, int pin, GPIOType);

	MOCK GPIOLevel get(int id);
	MOCK void set(int id, GPIOLevel);

private:


	//! Contains the path to the gpio directory
	std::string gpiobase;
};


#endif /* GPIOMANAGER_H_ */
