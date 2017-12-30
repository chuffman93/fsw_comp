/*
 * MockACPManager.h
 *
 *  Created on: Dec 22, 2017
 *      Author: alex
 */

#include "test/mockhal/MockACPManager.h"

MockACPManager::MockACPManager(void) : id(0), sync(0), opcode(0), message(0), crc(0) {};

MockACPManager::MockACPManager(MockSubsystemSync syncIn, MockACPOpcode opcodeIn) : id(0), sync(syncIn), opcode(opcodeIn), message(0), crc(0) {}

MockACPManager::MockACPManager(MockSubsystemSync syncIn, MockACPOpcode opcodeIn, uint16_t messageIn) : id(0), sync(syncIn), opcode(opcodeIn), message(messageIn), crc(0) {};

MockACPManager::~MockACPManager(){}

MockACPReturnPacket MockACPManager::transaction(){
MockACPReturnPacket returnPacket;
returnPacket.syncOut = sync;
returnPacket.opcodeOut = opcode;
switch(opcode){
	case MOCK_TEST_ALIVE:
		returnPacket.success = true;
		break;
	case MOCK_COMMAND_LED:
		returnPacket.success = true;
		break;
	case MOCK_DATA_LED:
		returnPacket.success = true;
		break;
	case MOCK_ACS_POINT_NADIR:
		returnPacket.success = true;
		break;
	case MOCK_ACS_POINT_COM:
		returnPacket.success = true;
		break;
	case MOCK_ACS_POINT_SUNSOAK:
		returnPacket.success = true;
		break;
	case MOCK_ACS_CONFIG_GPS:
		returnPacket.success = true;
		break;
	case MOCK_ACS_CONFIG_GAINS:
		returnPacket.success = true;
		break;
	default:
		returnPacket.success = false;

}
return returnPacket;
}
