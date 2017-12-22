/*
 * MockSPIManager.cpp
 *
 *  Created on: Dec 5, 2017
 *      Author: cyborg9
 */

#include "test/mockhal/MockSPIManager.h"
#include <assert.h>
#include "test/catch.hpp"
using namespace std;

//! Calls the constructor for SPIManager with dummy arguments
MockSPIManager::MockSPIManager()
:SPIManager("", 0, 0)
{

}

MockSPIManager::~MockSPIManager(){

}

//! Adds a new entry to TX_queues and RX_queues
int MockSPIManager::attachDevice(int ss){
	LockGuard l(lock);
	MockSPI spidev;
	return attachDevice(spidev);
}

//! Does nothings
void MockSPIManager::initialize(){}

//! Adds the byte to the corrisponding TX_queue
void MockSPIManager::sendbyte(int id, uint8_t byte){
	LockGuard l(lock);
	MockSPI& spidev = getDevice(id);
	spidev.TX_queue.push(byte);
}

//! Returns the top item in the corrisponding RX_queue
uint8_t MockSPIManager::receivebyte(int id){
	LockGuard l(lock);
	MockSPI& spidev = getDevice(id);

	if(spidev.RX_queue.size() == 0){
		return 0;
	}
	uint8_t value = spidev.RX_queue.front();
	spidev.RX_queue.pop();
	return value;
}

//! Adds the passed data to the corrisponding TX_queue
void MockSPIManager::addBytes(int id, vector<uint8_t> data){
	LockGuard l(lock);
	MockSPI& spidev = getDevice(id);
	for(vector<uint8_t>::iterator i = data.begin(); i < data.end(); i++){
		spidev.RX_queue.push(*i);
	}
}

//! Returns all of the data in the corrisponding TX_queue
vector<uint8_t> MockSPIManager::getBytes(int id){
	LockGuard l(lock);
	MockSPI& spidev = getDevice(id);
	vector<uint8_t> data;
	while(spidev.TX_queue.size() > 0){
		data.push_back(spidev.TX_queue.front());
		spidev.TX_queue.pop();
	}
	return data;
}



