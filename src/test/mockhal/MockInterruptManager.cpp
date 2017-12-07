/*
 * MockInterruptManager.cpp
 *
 *  Created on: Dec 5, 2017
 *      Author: cyborg9
 */

#include "test/mockhal/MockInterruptManager.h"
#include <assert.h>

//! Doesn't really do anything
MockInterruptManager::MockInterruptManager(){

}

//! Doesn't really do anything
MockInterruptManager::~MockInterruptManager(){

}

//! Doesn't really do anything
void MockInterruptManager::initialize(){

}

//! Adds a new spoofed device with no pending interrupts
int MockInterruptManager::attachDevice(char bank, int pin, INTEdge edge){
	LockGuard lock(this);
	pending.push_back(0);
	return pending.size() - 1;
}

//! Will return true if there is a pending interrupt, false otherwise
bool MockInterruptManager::wait(int id, uint32_t timeout){
	LockGuard lock(this);
	assert(id >= 0);
	assert((unsigned int)id < pending.size());
	if(pending[id] > 0){
		pending[id]--;
		return true;
	}
	return false;
}

//! Adds a pending interrupt to the specified device
void MockInterruptManager::addpending(int id){
	LockGuard lock(this);
	assert(id >= 0);
	assert((unsigned int)id < pending.size());
	pending[id]++;
}

