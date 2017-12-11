/*
 * HardwareManager.h
 *
 *  Created on: Dec 2, 2017
 *      Author: fsw
 */

#ifndef HARDWAREMANAGER_H_
#define HARDWAREMANAGER_H_

/*!
 * Class that implements the ideas for the hardware manager
 * Has a pure virtual initilization method to implement hardware dependant initialization
 */
class HardwareManager{
public:
	HardwareManager(){}
	virtual ~HardwareManager(){}
	//! Initialization to be implemented by the derived class
	virtual void initialize() = 0;
};




#endif /* HARDWAREMANAGER_H_ */
