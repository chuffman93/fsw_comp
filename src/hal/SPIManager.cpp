/*
 * SPIManager.cpp
 *
 *  Created on: Dec 5, 2017
 *      Author: cyborg9
 */

#include "hal/SPIManager.h"
using namespace std;

//! Stub, not necessarily final implementation
SPIManager::SPIManager(string busbase, uint8_t mode, uint32_t speed)
:busbase(busbase), mode(mode), speed(speed)
{

}

//! Stub, not necessarily final implementation
SPIManager::~SPIManager(){

}

//! Stub, not yet implemented
int SPIManager::attachDevice(int ss){

}

//! Stub, not yet implemented
void SPIManager::initialize(){

}

//! Stub, not yet implemented
void SPIManager::sendbyte(int id, uint8_t byte){

}

//! Stub, not yet implemented
uint8_t SPIManager::receivebyte(int id){

}

