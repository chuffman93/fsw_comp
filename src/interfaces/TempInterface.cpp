/*
 * TempInterface.cpp
 *
 *  Created on: Dec 28, 2017
 *      Author: cyborg9
 */

#include "interfaces/TempInterface.h"
#include "util/EventHandler.h"
#include <stdio.h>
#include <sstream>
#include <stdint.h>
#include <math.h>

/*!
 * Creates temperature interface with the provided device
 * \param onewireman the OneWireManager to be used
 * \param id the id of the device
 */
TempInterface::TempInterface(OneWireManager& onewireman, int id)
:onewireman(onewireman), id(id)
{
}

TempInterface::~TempInterface(){}

//! Start the temperature sampling process. At least 750ms must pass before calling getSample
void TempInterface::beginSample(){
	onewireman.writeToFile(id, "start", "1");
}

/*!
 * Returns the sampled temperature from the sensor. Returns NAN on error
 * \return the temperature in C
 */
float TempInterface::getSample(){
	std::string data = onewireman.readFromFile(id, "temp");
	std::stringstream ss(data);
	std::string line1, line2;

	if(!std::getline(ss, line1, '\n')){
		EventHandler::event(LEVEL_WARN, "[TempInterface] Invalid data string from sensor");
		return NAN;
	}
	std::getline(ss, line2, '\n');

	if(line2.c_str()[27] != 'V'){
		std::stringstream ss;
		ss << "[TempInterface] Data from sensor not valid char: ";
		ss << line2;
		EventHandler::event(LEVEL_WARN,  ss.str());
		return NAN;
	}
	EventHandler::event(LEVEL_DEBUG, "[TempInterface] Read \"" + line1 + "\" from the sensor");
	int rawval;
	if(sscanf(line1.c_str(), "t=%d", &rawval) != 1){
		EventHandler::event(LEVEL_WARN, "[TempInterface] string parse failed");
	}

	std::stringstream ss2;
	ss2 << "[TempInterface] Read " << rawval << "mC from sensor";
	EventHandler::event(LEVEL_INFO, ss2.str());

	return (float) rawval / 1000.0;

}
