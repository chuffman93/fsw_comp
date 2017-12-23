/*
 * MockSPIManager.h
 *
 *  Created on: Dec 5, 2017
 *      Author: cyborg9
 */

#ifndef MOCKSPIMANAGER_H_
#define MOCKSPIMANAGER_H_

#include "hal/SPIManager.h"
#include <vector>
#include <queue>

//TODO: make mock BusManager

struct MockSPI{
	std::queue<uint8_t> TX_queue;
	std::queue<uint8_t> RX_queue;

	bool operator ==(const MockSPI& ms){return false;}
};

/*!
 * Mock class to allow us to test things that depend on SPIManager.
 */
class MockSPIManager: public SPIManager, public BusManager<MockSPI>{
using BusManager<MockSPI>::attachDevice;
using BusManager<MockSPI>::getDevice;
public:
	MockSPIManager();
	~MockSPIManager();

	int attachDevice(int ss);
	void initialize();

	void sendbyte(int id, uint8_t byte);
	uint8_t receivebyte(int id);

	void addBytes(int id, std::vector<uint8_t> data);
	std::vector<uint8_t> getBytes(int id);

private:
	Lock lock;
};



#endif /* MOCKSPIMANAGER_H_ */
