/*
 * test_RAD.cpp
 *
 *  Created on: Jan 18, 2018
 *      Author: chuffman93
 */


#include "test/catch.hpp"
#include <subsystem/RAD.h>
#include <iterator>
#include <iostream>
#include "hal/GPIOManager.h"

SPIManager dummySR("", 0, 0);
GPIOManager dummyGR("");

class RADMockACPInterface: public ACPInterface{
public:
	RADMockACPInterface(): ACPInterface(dummySR, dummyGR, 0, 0, ""){}
	~RADMockACPInterface(){}
	bool transaction(ACPPacket& packet, ACPPacket& ret){
		sentOpcodes.push_back(packet.opcode);
		ret.opcode = packet.opcode;
		return true;
	}
	std::vector<uint8_t> sentOpcodes;
};

RADMockACPInterface acp;
SubPowerInterface subPower(dummyGR, 0, 0, 0, "");
RAD rad(acp,subPower);

TEST_CASE("TEST INITIALIZATION ROUTINE","[PLD]"){

	REQUIRE(rad.commandCollectionBegin());
	REQUIRE(acp.sentOpcodes.end() != std::find(acp.sentOpcodes.begin(), acp.sentOpcodes.end(), OP_TESTALIVE));
	//validate that it sends LED on opcode
	REQUIRE(acp.sentOpcodes.end() != std::find(acp.sentOpcodes.begin(), acp.sentOpcodes.end(), OP_TESTLED));
	//validate that it sends data LED opcode
	REQUIRE(acp.sentOpcodes.end() != std::find(acp.sentOpcodes.begin(), acp.sentOpcodes.end(), OP_TESTCONFIG));

}

TEST_CASE("SPLIT DATA","[.][PLD][splt]"){


	REQUIRE(rad.commandCollectionBegin());


	std::vector<uint8_t> buff;
	buff.assign(1000000,1);
	int num = rad.readDataNumber();
	char mockDataFile[100];
	sprintf(mockDataFile,MOCK_RAD_PATH "/RAD_%d",num);
	std::ofstream in(mockDataFile);
	for(std::vector<uint8_t>::iterator i = buff.begin(); i != buff.end(); i++){
		in << *i;
	}

	REQUIRE(access(mockDataFile,F_OK) == 0);
	REQUIRE(rad.commandCollectionEnd());
	REQUIRE(access(mockDataFile,F_OK) == -1);
}


