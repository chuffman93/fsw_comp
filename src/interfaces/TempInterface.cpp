/*
 * TempInterface.cpp
 *
 *  Created on: Dec 28, 2017
 *      Author: cyborg9
 */

#include "interfaces/TempInterface.h"
#include "util/Logger.h"
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
	tags += LogTag("Name", "TempInterface");
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
		Logger::Stream(LEVEL_WARN, tags) << "Invalid data string: \"" << data << "\"";
		return NAN;
	}
	std::getline(ss, line2, '\n');
	Logger::Stream(LEVEL_DEBUG, tags) << ss;
	if(line2.c_str()[27] != 'V'){
		Logger::Stream(LEVEL_WARN, tags) << "Data fails crc: \"" << line2 << "\"";
		return NAN;
	}
	Logger::Stream(LEVEL_DEBUG, tags) << "Read \"" << line1 << "\" from the sensor";
	int rawval;
	if(sscanf(line1.c_str(), "t=%d", &rawval) != 1){
		Logger::Stream(LEVEL_WARN, tags) << "Failed to parse \"" << line1 << "\"";
	}

	Logger::Stream(LEVEL_INFO, tags) << "Read " << rawval << "mC from the sensor";
	return (float) rawval / 1000.0;

}
