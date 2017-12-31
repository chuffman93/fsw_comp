/*
 * test_EPS.cpp
 *
 *  Created on: Dec 30, 2017
 *      Author: alpa3119
 */

#include "test/catch.hpp"
#include "subsystem/EPS.h"
#include "interfaces/ACPInterface.h"

#include <vector>

SPIManager dummys("", 0, 0);
GPIOManager dummyg("");

class MockACPInterface: public ACPInterface{
public:
	MockACPInterface()
	:ACPInterface(dummys, dummyg, 0 , 0)
	{}
	~MockACPInterface(){}
	bool transaction(ACPPacket& packet, ACPPacket& ret){
		sentOpcodes.push_back(packet.opcode);
		return true;
	}

	std::vector<uint8_t> sentOpcodes;

};

TEST_CASE("EPS Test Initialization Routine", "[subsystem][EPS]"){
	//initialize/setup
	MockACPInterface acp;
	SubPowerInterface subPower(dummyg, 0, 0, 0, "");
	EPS eps(acp,subPower);
	//call EPS initialize
	eps.initialize();
	//validate that it sends test alive opcode
	REQUIRE(acp.sentOpcodes.end() != std::find(acp.sentOpcodes.begin(), acp.sentOpcodes.end(), OP_TESTALIVE));
	//validate that it sends LED on opcode
	REQUIRE(acp.sentOpcodes.end() != std::find(acp.sentOpcodes.begin(), acp.sentOpcodes.end(), OP_TESTLED));
	//validate that it sends data LED opcode
	REQUIRE(acp.sentOpcodes.end() != std::find(acp.sentOpcodes.begin(), acp.sentOpcodes.end(), OP_TESTCONFIG));
}


TEST_CASE("EPS Test Get Health and Status", "[subsystem][EPS]"){
	//initialize/setup
	//call EPS initialize
	//call EPS health and status
		//validate that it sends the health and status opcode
		//respond with dummy data
		//validate that data is process correctly
		//TODO: validate that correct message buffer is passed to file interface


}


