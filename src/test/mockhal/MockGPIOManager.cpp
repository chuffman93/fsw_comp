/*
 * MockInterruptManager.cpp
 *
 *  Created on: Dec 5, 2017
 *      Author: cyborg9
 */

#include "test/mockhal/MockGPIOManager.h"
#include <assert.h>

//! Doesn't really do anything
MockGPIOManager::MockGPIOManager()
:GPIOManager(""){

}

//! Doesn't really do anything
MockGPIOManager::~MockGPIOManager(){

}

//! Doesn't really do anything
void MockGPIOManager::initialize(){

}

//! Adds a new spoofed device with no pending interrupts
int MockGPIOManager::attachDevice(char bank, int pin, GPIOEdge edge){
	MockGPIO gpio;
	gpio.pending = 0;
	return attachDevice(gpio);
}

//! Will return true if there is a pending interrupt, false otherwise
bool MockGPIOManager::wait(int id, uint32_t timeout){
	MockGPIO& gpio = getDevice(id);
	LockGuard lock(gpio.lock);
	if(gpio.pending > 0){
		gpio.pending--;
		return true;
	}
	return false;
}

//! Adds a pending interrupt to the specified device
void MockGPIOManager::addpending(int id){
	MockGPIO& gpio = getDevice(id);
	LockGuard lock(gpio.lock);
	gpio.pending++;
}

GPIOLevel MockGPIOManager::get(int id){
	MockGPIO& gpio = getDevice(id);
	LockGuard lock(gpio.lock);
	return gpio.state;
}

void MockGPIOManager::set(int id, GPIOLevel level){
	MockGPIO& gpio = getDevice(id);
	LockGuard lock(gpio.lock);
	gpio.state = level;
}

