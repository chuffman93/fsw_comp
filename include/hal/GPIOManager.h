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
};

/*!
 * Handles the gpios on the SAMA5 through the linux file system API
 */
class GPIOManager: public BusManager<GPIODevice>{
public:
	GPIOManager();
	~GPIOManager();

	void initialize();
	MOCK int attachDevice(char bank, int pin, GPIOType);

	MOCK GPIOLevel get(int id);
	MOCK void set(int id, GPIOLevel);

private:
	std::string gpiobase;
};


#endif /* GPIOMANAGER_H_ */
