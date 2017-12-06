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

/*!
 * Mock class to allow us to test things that depend on SPIManager.
 */
class MockSPIManager: public SPIManager{
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
	//! Holds all of the queues of bytes sent to the spoofed devices
	std::vector<std::queue<uint8_t> > TX_queues;
	//! Holds all of the queues of bytes to be received from the spoofed devices
	std::vector<std::queue<uint8_t> > RX_queues;
};



#endif /* MOCKSPIMANAGER_H_ */
