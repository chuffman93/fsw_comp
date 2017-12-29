/*
 * TempInterface.h
 *
 *  Created on: Dec 28, 2017
 *      Author: cyborg9
 */

#ifndef TEMPINTERFACE_H_
#define TEMPINTERFACE_H_

#include "hal/OneWireManager.h"
#include "util/Logger.h"

class TempInterface{
public:
	TempInterface(OneWireManager& onewireman, int id);
	~TempInterface();

	void beginSample();
	float getSample();
private:
	//! The reference to the one wire manager
	OneWireManager& onewireman;

	//! The id of the device on the bus
	int id;

	//! Tags to use when logging
	LogTags tags;
};




#endif /* TEMPINTERFACE_H_ */
