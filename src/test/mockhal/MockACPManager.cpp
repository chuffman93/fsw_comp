/*
 * MockACPManager.h
 *
 *  Created on: Dec 22, 2017
 *      Author: alex
 */

#include "test/mockhal/MockACPManager.h"
#include "interfaces/ACPInterface.h"
#include "core/FileSystem.h"

#include <stdio.h>

SPIManager dummys("", 0, 0);
GPIOManager dummyg("");

MockACPInterface::MockACPInterface() : ACPInterface(dummys, dummyg, 0 , 0){

}

MockACPInterface::~MockACPInterface(){

}

bool MockACPInterface::transaction(ACPPacket& packet, ACPPacket& ret){
	sentOpcodes.push_back(packet.opcode);
}


