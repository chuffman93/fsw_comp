/*
 * test_EPS.cpp
 *
 *  Created on: Dec 30, 2017
 *      Author: alpa3119
 */

#include "test/catch.hpp"
#include "subsystem/EPS.h"
#include "core/FileSystem.h"
#include "hal/GPIOManager.h"

#include <stdio.h>
#include <iostream>
#include <vector>

using namespace std;
SPIManager dummys("", 0, 0);
GPIOManager dummygpi("");

class EPSMockACPInterface: public ACPInterface{
public:
	EPSMockACPInterface(): ACPInterface(dummys, dummygpi, 0 , 0, ""){}
	~EPSMockACPInterface(){}
	bool transaction(ACPPacket& packet, ACPPacket& ret){
		sentOpcodes.push_back(packet.opcode);
		ret.opcode = packet.opcode;
		if (packet.opcode == OP_HEALTHSTATUS){
			std::vector<uint8_t> buff;
			buff.resize(36);
			buff.assign(36,5);
			buff[12] = 0;
			buff[13] = 1;
			ret.message = buff;
		}
		return true;
	}
	std::vector<uint8_t> sentOpcodes;
};

TEST_CASE("EPS Test Initialization Routine", "[subsystem][EPS]"){
	//initialize/setup
	EPSMockACPInterface acp;
	SubPowerInterface subPower(dummygpi, 0, 0, 0, "");
	EPS eps(acp,subPower);
	//call EPS initialize
	REQUIRE(eps.initialize());
	//validate that it sends test alive opcode
	REQUIRE(acp.sentOpcodes.end() != std::find(acp.sentOpcodes.begin(), acp.sentOpcodes.end(), OP_TESTALIVE));
	//validate that it sends LED on opcode
	REQUIRE(acp.sentOpcodes.end() != std::find(acp.sentOpcodes.begin(), acp.sentOpcodes.end(), OP_TESTLED));
	//validate that it sends data LED opcode
	REQUIRE(acp.sentOpcodes.end() != std::find(acp.sentOpcodes.begin(), acp.sentOpcodes.end(), OP_TESTCONFIG));
}


TEST_CASE("EPS Test Get Health and Status", "[subsystem][EPS]"){
	//initialize/setup
	EPSMockACPInterface acp;
	SubPowerInterface subPower(dummygpi, 0, 0, 0, "");
	EPS eps(acp,subPower);
	//call EPS initialize
	REQUIRE(eps.initialize());
	//call EPS health and status
	eps.getHealthStatus();
	//validate that it sends the health and status opcode
	REQUIRE(acp.sentOpcodes.end() != std::find(acp.sentOpcodes.begin(), acp.sentOpcodes.end(), OP_HEALTHSTATUS));
	//validate that data is process correctly
	REQUIRE(eps.getBatteryStateOfCharge() == 1);
	//TODO: validate that correct message buffer is passed to file interface

}

TEST_CASE("EPS Test Command Reset", "[.][subsystem][EPS]"){
	//initialize/setup
	EPSMockACPInterface acp;
	SubPowerInterface subPower(dummygpi, 0, 0, 0, "");
	EPS eps(acp,subPower);
	//call EPS initialize
	REQUIRE(eps.initialize());
	eps.handleMode(Mode_Reset);
	REQUIRE(acp.sentOpcodes.end() != std::find(acp.sentOpcodes.begin(), acp.sentOpcodes.end(), OP_SUBSYSTEMRESET));

}


