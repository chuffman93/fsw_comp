/*
 * test_COM.cpp
 *
 *  Created on: Jan 7, 2018
 *      Author: alpa3119
 */


#include "test/catch.hpp"
#include "subsystem/COM.h"
#include "core/FileSystem.h"
#include "hal/GPIOManager.h"

#include <stdio.h>
#include <iostream>
#include <vector>

using namespace std;
SPIManager dummyS("", 0, 0);
GPIOManager dummyG("");

class COMMockACPInterface: public ACPInterface{
public:
	// COMMockACPInterface(): ACPInterface(dummyS, dummyG, 0 , 0){}
	COMMockACPInterface(): ACPInterface(dummyS, dummyG, 0 , 0, ""){}

	~COMMockACPInterface(){}
	bool transaction(ACPPacket& packet, ACPPacket& ret){
		ret.opcode = packet.opcode;
		sentOpcodes.push_back(packet.opcode);
		return true;
	}
	std::vector<uint8_t> sentOpcodes;
};

TEST_CASE("COM Test Initialization Routine", "[subsystem][COM]"){
	//initialize/setup
	COMMockACPInterface acp;
	SubPowerInterface subPower(dummyG, 0, 0, 0, "");
	COM com(acp,subPower);
	//call COM initialize
	REQUIRE(com.initialize());
	com.initialize();
	//validate that it sends test alive opcode
	REQUIRE(acp.sentOpcodes.end() != std::find(acp.sentOpcodes.begin(), acp.sentOpcodes.end(), OP_TESTALIVE));
	//validate that it sends LED on opcode
	REQUIRE(acp.sentOpcodes.end() != std::find(acp.sentOpcodes.begin(), acp.sentOpcodes.end(), OP_TESTLED));
	//validate that it sends data LED opcode
	REQUIRE(acp.sentOpcodes.end() != std::find(acp.sentOpcodes.begin(), acp.sentOpcodes.end(), OP_TESTCONFIG));
}


TEST_CASE("COM Test Get Health and Status", "[subsystem][COM]"){
	//initialize/setup
	COMMockACPInterface acp;
	SubPowerInterface subPower(dummyG, 0, 0, 0, "");
	COM com(acp,subPower);
	//call COM initialize
	REQUIRE(com.initialize());
	com.initialize();
	//call COM health and status
	com.getHealthStatus();
	//validate that it sends the health and status opcode
	REQUIRE(acp.sentOpcodes.end() != std::find(acp.sentOpcodes.begin(), acp.sentOpcodes.end(), OP_HEALTHSTATUS));

}

TEST_CASE("COM Test Command Reset", "[subsystem][COM]"){
	//initialize/setup
	COMMockACPInterface acp;
	SubPowerInterface subPower(dummyG, 0, 0, 0, "");
	COM com(acp,subPower);
	//call COM initialize
	REQUIRE(com.initialize());
	com.initialize();
	com.handleMode(Mode_Reset);
	//validate that it sends the health and status opcode
	REQUIRE(acp.sentOpcodes.end() != std::find(acp.sentOpcodes.begin(), acp.sentOpcodes.end(), OP_SUBSYSTEMRESET));

}

