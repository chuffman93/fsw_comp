/*
 * MockInterruptManager.h
 *
 *  Created on: Dec 5, 2017
 *      Author: cyborg9
 */

#ifndef MOCKINTERRUPTMANAGER_H_
#define MOCKINTERRUPTMANAGER_H_

#include "hal/GPIOManager.h"

struct MockGPIO{
	int pending;
	Lock lock;
};

/*!
 * Mock class for the InterruptManager
 */
class MockGPIOManager: public GPIOManager, public BusManager<MockGPIO>{
using BusManager<MockGPIO>::attachDevice;
using BusManager<MockGPIO>::getDevice;
public:
	MockGPIOManager();
	~MockGPIOManager();

	void initialize();
	int attachDevice(char bank, int pin, GPIOEdge);
	bool wait(int id, uint32_t timeout);

	void addpending(int id);

private:
};


#endif /* MOCKINTERRUPTMANAGER_H_ */
