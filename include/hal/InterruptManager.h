/*
 * InterruptManager.h
 *
 *  Created on: Dec 2, 2017
 *      Author: fsw
 */

#ifndef INTERRUPTMANAGER_H_
#define INTERRUPTMANAGER_H_
#include "hal/GPIOManager.h"

enum INTEdge{
	INT_FALLING,
	INT_RISING,
};

/*!
 * Handles GPIOs that have an edge triggered interrupt using the linux fs
 */
class InterruptManager: public GPIOManager{
public:
	InterruptManager();
	~InterruptManager();

	void initialize();
	int attachDevice(char bank, int pin, INTEdge);

	bool wait(int id, uint32_t timeout);
};


#endif /* INTERRUPTMANAGER_H_ */
