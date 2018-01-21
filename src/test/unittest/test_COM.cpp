/*
 * test_COM.cpp
 *
 *  Created on: Jan 20, 2018
 *      Author: alpa3119
 */




#include "test/catch.hpp"
#include "subsystem/COM.h"
#include "core/FileSystem.h"
#include "hal/GPIOManager.h"

#include <stdio.h>
#include <iostream>
#include <vector>

SPIManager spi("", 0, 0);
GPIOManager gpio("");

class COMMockACPInterface: public ACPInterface{
public:
	COMMockACPInterface(): ACPInterface(spi, gpio, 0 , 0){}
	~COMMockACPInterface(){}
	bool transaction(ACPPacket& packet, ACPPacket& ret){
		sentOpcodes.push_back(packet.opcode);
		return true;
	}
	std::vector<uint8_t> sentOpcodes;
};
