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
	RADMockACPInterface(): ACPInterface(dummySR, dummyGR, 0, 0){}
	~RADMockACPInterface(){}
	bool transaction(ACPPacket& packet, ACPPacket& ret){
		sentOpcodes.push_back(packet.opcode);
		return true;
	}
	std::vector<uint8_t> sentOpcodes;
};

RADMockACPInterface acp;
SubPowerInterface subPower(dummyGR, 0, 0, 0, "");
RAD rad(acp,subPower);

TEST_CASE("TEST INITIALIZATION ROUTINE","[PLD]"){

	rad.commandCollectionBegin();
	REQUIRE(acp.sentOpcodes.end() != std::find(acp.sentOpcodes.begin(), acp.sentOpcodes.end(), OP_TESTALIVE));
	//validate that it sends LED on opcode
	REQUIRE(acp.sentOpcodes.end() != std::find(acp.sentOpcodes.begin(), acp.sentOpcodes.end(), OP_TESTLED));
	//validate that it sends data LED opcode
	REQUIRE(acp.sentOpcodes.end() != std::find(acp.sentOpcodes.begin(), acp.sentOpcodes.end(), OP_TESTCONFIG));

}

TEST_CASE("SPLIT DATA","[PLD]"){

	rad.commandCollectionBegin();


	std::vector<uint8_t> buff;
	buff.assign(1000000,1);

	std::ofstream in("/home/RAD_1");
	for(int i = 0; i < buff.size(); i++){
		in << buff[i];
	}

	REQUIRE(access("/home/RAD_1",F_OK) == 0);
	REQUIRE(access("/home/RAD_1009.tar.gz",F_OK) == -1);
	rad.commandCollectionEnd();
	REQUIRE(access("/home/RAD_1",F_OK) == -1);
	REQUIRE(access("/home/RAD_1009.tar.gz",F_OK) == 0);

	char argv[100];
	sprintf(argv,"rm -r /home/RAD*");
	FILE * fd;
	fd = popen(argv, "r");
	pclose(fd);
	REQUIRE(access("/home/RAD_1009.tar.gz",F_OK) == -1);
}

TEST_CASE("Recieve Data","[PLD]"){

}

