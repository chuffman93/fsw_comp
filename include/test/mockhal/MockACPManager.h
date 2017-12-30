/*
 * MockACPManager.h
 *
 *  Created on: Dec 22, 2017
 *      Author: alex
 */

#ifndef MOCKACPMANAGER_H_
#define MOCKACPMANAGER_H_

#include <stdint.h>
#include <vector>

using namespace std;

enum MockACPOpcode {
	MOCK_POWEROFF,
	MOCK_POWERON,
	MOCK_TEST_ALIVE,
	MOCK_COMMAND_LED,
	MOCK_DATA_LED,

	MOCK_ACS_POINT_NADIR,
	MOCK_ACS_POINT_COM,
	MOCK_ACS_POINT_SUNSOAK,
	MOCK_ACS_CONFIG_GPS,
	MOCK_ACS_CONFIG_GAINS,
};

enum MockSubsystemSync {
	MOCK_EPS = 0xC4 | 0,
	MOCK_COM = 0xC4 | 1,
	MOCK_ACS = 0xC4 | 2,
	MOCK_PLD = 0xC4 | 3,
	MOCK_GPS = 0xC4 | 4,
};


struct MockACPReturnPacket{
	uint8_t syncOut;
	uint8_t opcodeOut;
	bool success;
};

struct MockACPManager{

	uint8_t sync;
	uint16_t id;
	uint8_t opcode;
	std::vector<uint8_t> message;
	uint16_t crc;

	MockACPManager(){};
	MockACPManager(MockSubsystemSync syncIn, MockACPOpcode opcodeIn){};
	MockACPManager(MockSubsystemSync syncIn, MockACPOpcode opcodeIn, uint16_t messageIn){};
	~MockACPManager(){};

	MockACPReturnPacket transaction();
};

#endif /* MOCKACPMANAGER_H_ */
