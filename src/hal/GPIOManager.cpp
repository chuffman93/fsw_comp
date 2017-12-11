/*
 * GPIOManager.cpp
 *
 *  Created on: Dec 5, 2017
 *      Author: cyborg9
 */

#include "hal/GPIOManager.h"

//! Stub, not yet implemented
GPIOManager::GPIOManager(std::string busbase){

}

//! Stub, not yet implemented
GPIOManager::~GPIOManager(){

}

//! Stub, not yet implemented
void GPIOManager::initialize(){

}

//! Stub, not yet implemented
int GPIOManager::attachDevice(char bank, int pin, GPIOType type){ // TODO: change type to dir
	GPIODevice gpioDev;
	attachDevice<GPIODevice>(gpioDev);
}

//! Stub, not yet implemented
GPIOLevel GPIOManager::get(int id){

}

//! Stub, not yet implemented
void GPIOManager::set(int id, GPIOLevel level){
	Monitor monitor(getDevice(id).lock);
}




