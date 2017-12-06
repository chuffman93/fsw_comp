/*
 * MockInterruptManager.h
 *
 *  Created on: Dec 5, 2017
 *      Author: cyborg9
 */

#ifndef MOCKINTERRUPTMANAGER_H_
#define MOCKINTERRUPTMANAGER_H_

#include "hal/InterruptManager.h"

/*!
 * Mock class for the InterruptManager
 */
class MockInterruptManager: public InterruptManager{
public:
	MockInterruptManager();
	~MockInterruptManager();

	void initialize();
	int attachDevice(char bank, int pin, INTEdge);
	bool wait(int id, uint32_t timeout);

	void addpending(int id);

private:
	//! Contains the number of pending interrupts
	std::vector<int> pending;
};


#endif /* MOCKINTERRUPTMANAGER_H_ */
