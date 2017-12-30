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
	GPIOLevel state;
	Lock lock;

	bool operator ==(const MockGPIO& mg){return false;}
};

/*!
 * Mock class for the InterruptManager
 */
class MockGPIOManager: public GPIOManager, public BusManager<MockGPIO>{
public:
	using BusManager<MockGPIO>::attachDevice;
	using BusManager<MockGPIO>::getDevice;
	MockGPIOManager();
	~MockGPIOManager();

	void initialize();
	int attachDevice(char bank, int pin, GPIOEdge);
	bool wait(int id, uint32_t timeout);

	GPIOLevel get(int id);
	void set(int id, GPIOLevel level);

	void addpending(int id);

private:
	Lock lock;
};


#endif /* MOCKINTERRUPTMANAGER_H_ */
