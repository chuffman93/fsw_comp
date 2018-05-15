/*
 * test_ACS.cpp
 *
 *  Created on: Jan 7, 2018
 *      Author: alpa3119
 */


#include "test/catch.hpp"
#include "subsystem/ACS.h"
#include "core/FileSystem.h"
#include "hal/GPIOManager.h"

#include <stdio.h>
#include <iostream>
#include <vector>

SPIManager dummyspi("", 0, 0);
GPIOManager dummygpio("");

class ACSMockACPInterface: public ACPInterface{
public:
	ACSMockACPInterface(): ACPInterface(dummyspi, dummygpio, 0 , 0,"ACS"){}
	~ACSMockACPInterface(){}
	bool transaction(ACPPacket& packet, ACPPacket& ret){
		sentOpcodes.push_back(packet.opcode);
		ret.opcode = packet.opcode;
		if (packet.opcode == OP_HEALTHSTATUS){
			ret.message.assign(100,0);
		}
		return true;
	}
	std::vector<uint8_t> sentOpcodes;
};

TEST_CASE("ACS Test Initialization Routine", "[subsystem][ACS]"){
	//initialize/setup
	ACSMockACPInterface acp;
	SubPowerInterface subPower(dummygpio, 0, 0, 0, "");
	ACS acs(acp,subPower);
	//call ACS initialize
	acs.initialize();
	//validate that it sends test alive opcode
	REQUIRE(acp.sentOpcodes.end() != std::find(acp.sentOpcodes.begin(), acp.sentOpcodes.end(), OP_TESTALIVE));
	//validate that it sends LED on opcode
	REQUIRE(acp.sentOpcodes.end() != std::find(acp.sentOpcodes.begin(), acp.sentOpcodes.end(), OP_TESTLED));
	//validate that it sends data LED opcode
	REQUIRE(acp.sentOpcodes.end() != std::find(acp.sentOpcodes.begin(), acp.sentOpcodes.end(), OP_TESTCONFIG));
}

TEST_CASE("ACS Test Get Health and Status", "[subsystem][COM]"){
	//initialize/setup
	ACSMockACPInterface acp;
	SubPowerInterface subPower(dummygpio, 0, 0, 0, "");
	ACS acs(acp,subPower);
	//call COM initialize
	acs.initialize();
	//call COM health and status
	acs.getHealthStatus();
	//validate that it sends the health and status opcode
	REQUIRE(acp.sentOpcodes.end() != std::find(acp.sentOpcodes.begin(), acp.sentOpcodes.end(), OP_HEALTHSTATUS));
	REQUIRE(acs.getTimeSinceLock() == 0);

}

TEST_CASE("ACS Test Handle Mode", "[subsystem][ACS]"){
	//initialize/setup
	ACSMockACPInterface acp;
	SubPowerInterface subPower(dummygpio, 0, 0, 0, "");
	ACS acs(acp,subPower);
	//call ACS initialize
	acs.initialize();
	acs.handleMode(Mode_Bus);
	acs.handleMode(Mode_Reset);
	acs.handleMode(Trans_BusToPayload);
	acs.handleMode(Trans_PayloadToBus);
	acs.handleMode(Trans_BusToCom);
	REQUIRE(acp.sentOpcodes.end() != std::find(acp.sentOpcodes.begin(), acp.sentOpcodes.end(), OP_SENDGPS));
	REQUIRE(acp.sentOpcodes.end() != std::find(acp.sentOpcodes.begin(), acp.sentOpcodes.end(), OP_POINTNADIR));
	REQUIRE(acp.sentOpcodes.end() != std::find(acp.sentOpcodes.begin(), acp.sentOpcodes.end(), OP_POINTSUN));
	REQUIRE(acp.sentOpcodes.end() != std::find(acp.sentOpcodes.begin(), acp.sentOpcodes.end(), OP_POINTCOM));
	REQUIRE(acp.sentOpcodes.end() != std::find(acp.sentOpcodes.begin(), acp.sentOpcodes.end(), OP_SUBSYSTEMRESET));


}


