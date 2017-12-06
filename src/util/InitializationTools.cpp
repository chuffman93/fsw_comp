/*
 * InitializationTools.cpp
 *
 *  Created on: Dec 4, 2017
 *      Author: cyborg9
 */

#include "util/InitializationTools.h"
#include <vector>

using namespace std;

/*!
 * Takes the provided list of hardware devices in the system,
 * and calls the initialization method for each of them.
 */
void initializeHardware(vector<HardwareManager*> hardware){
	for(vector<HardwareManager*>::iterator i = hardware.begin(); i < hardware.end(); i++){
		(*i)->initialize();
	}
}
