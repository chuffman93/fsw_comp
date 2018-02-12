/*
 * NMEAInterface.h
 *
 *  Created on: Dec 30, 2017
 *      Author: cyborg9
 */

#ifndef NMEAINTERFACE_H_
#define NMEAINTERFACE_H_

#include "hal/UARTManager.h"
#include "util/Logger.h"

class NMEAInterface{
public:
	NMEAInterface(UARTManager& uart);
	MOCK ~NMEAInterface();

	MOCK std::string getString();
	MOCK void sendCommand(std::string str);

private:
	enum{
		WAIT_FOR_HASH,
		WAIT_FOR_STAR,
		WAIT_FOR_CRC,
	} state;
	std::string databuffer;
	UARTManager& uart;
	LogTags tags;
};



#endif /* NMEAINTERFACE_H_ */
