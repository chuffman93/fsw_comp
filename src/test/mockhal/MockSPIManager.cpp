/*
 * MockSPIManager.cpp
 *
 *  Created on: Dec 5, 2017
 *      Author: cyborg9
 */

#include "test/mockhal/MockSPIManager.h"
#include <assert.h>

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
	LockGuard lock(this);
	TX_queues.push_back(queue<uint8_t>());
	RX_queues.push_back(queue<uint8_t>());

	return TX_queues.size() - 1;
}

//! Does nothings
void MockSPIManager::initialize(){}

//! Adds the byte to the corrisponding TX_queue
void MockSPIManager::sendbyte(int id, uint8_t byte){
	LockGuard lock(this);
	assert(id < TX_queues.size());
	TX_queues[id].push(byte);
}

//! Returns the top item in the corrisponding RX_queue
uint8_t MockSPIManager::receivebyte(int id){
	LockGuard lock(this);
	assert(id < RX_queues.size());
	if(RX_queues[id].size() == 0){
		return 0;
	}
	uint8_t value = RX_queues[id].front();
	RX_queues[id].pop();
	return value;
}

//! Adds the passed data to the corrisponding TX_queue
void MockSPIManager::addBytes(int id, vector<uint8_t> data){
	LockGuard lock(this);
	assert(id < RX_queues.size());
	for(vector<uint8_t>::iterator i = data.begin(); i < data.end(); i++){
		RX_queues[id].push(*i);
	}
}

//! Returns all of the data in the corrisponding TX_queue
vector<uint8_t> MockSPIManager::getBytes(int id){
	LockGuard lock(this);
	assert(id < TX_queues.size());
	vector<uint8_t> data;
	while(TX_queues[id].size() > 0){
		data.push_back(TX_queues[id].front());
		TX_queues[id].pop();
	}
	return data;
}



