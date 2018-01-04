/*
 * MockACPManager.h
 *
 *  Created on: Dec 22, 2017
 *      Author: alex
 */
/*
#ifndef MOCKACPMANAGER_H_
#define MOCKACPMANAGER_H_

#include "interfaces/ACPInterface.h"
#include "hal/GPIOManager.h"
#include "hal/SPIManager.h"

#include <stdint.h>
#include <vector>
/*
class MockACPInterface: public ACPInterface{
public:
	MockACPInterface();
	~MockACPInterface();
	bool transaction(ACPPacket& packet, ACPPacket& ret);
	std::vector<uint8_t> sentOpcodes;
};

#endif /* MOCKACPMANAGER_H_ */


