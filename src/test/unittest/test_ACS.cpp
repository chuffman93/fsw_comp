/*
 * test_ACS.cpp
 *
 *  Created on: Dec 28, 2017
 *      Author: fsw
 */

#include "test/mockhal/MockACPManager.h"
#include "subsystem/ACS.h"
#include <iostream>
#include <unistd.h>
#include "test/catch.hpp"

using namespace std;

TEST_CASE("ACS Test Communication" , "[.][hardware][SubsystemBase][ACS][Communication]"){
	MockSubsystemSync sync = MOCK_ACS;
	SECTION("ACS Test Alive"){
		MockACPOpcode opcode = MOCK_TEST_ALIVE;
		MockACPManager test(sync, opcode);
		MockACPReturnPacket ret = test.transaction();
		REQUIRE(ret.syncOut == MOCK_ACS);
		REQUIRE(ret.opcodeOut == MOCK_TEST_ALIVE);
		REQUIRE(ret.success == true);
	}
	SECTION("ACS Test Command LED"){
		MockACPOpcode opcode = MOCK_COMMAND_LED;
		MockACPManager test(sync, opcode);
		MockACPReturnPacket ret = test.transaction();
		REQUIRE(ret.syncOut == MOCK_ACS);
		REQUIRE(ret.opcodeOut == MOCK_COMMAND_LED);
		REQUIRE(ret.success == true);
	}
	SECTION("ACS Test Data LED"){
		MockACPOpcode opcode = MOCK_DATA_LED;
		MockACPManager test(sync, opcode);
		MockACPReturnPacket ret = test.transaction();
		REQUIRE(ret.syncOut == MOCK_ACS);
		REQUIRE(ret.opcodeOut == MOCK_DATA_LED);
		REQUIRE(ret.success == true);
	}
}
TEST_CASE("ACS Test Subsystem Commands", "[.][hardware][SubsystemBase][ACS][Opcodes]" ){
	MockSubsystemSync sync = MOCK_ACS;
	SECTION("ACS Test Point Nadir"){
		MockACPOpcode opcode= MOCK_ACS_POINT_NADIR;
		MockACPManager test(sync, opcode);
		MockACPReturnPacket ret = test.transaction();
		REQUIRE(ret.syncOut == MOCK_ACS);
		REQUIRE(ret.opcodeOut == MOCK_ACS_POINT_NADIR);
		REQUIRE(ret.success == true);
	}
	SECTION("ACS Test Point COM"){
		MockACPOpcode opcode = MOCK_ACS_POINT_COM;
		MockACPManager test(sync, opcode);
		MockACPReturnPacket ret = test.transaction();
		REQUIRE(ret.syncOut == MOCK_ACS);
		REQUIRE(ret.opcodeOut == MOCK_ACS_POINT_COM);
		REQUIRE(ret.success == true);
	}
	SECTION("ACS Test Point Sun Soak"){
		MockACPOpcode opcode = MOCK_ACS_POINT_SUNSOAK;
		MockACPManager test(sync, opcode);
		MockACPReturnPacket ret = test.transaction();
		REQUIRE(ret.syncOut == MOCK_ACS);
		REQUIRE(ret.opcodeOut == MOCK_ACS_POINT_SUNSOAK);
		REQUIRE(ret.success == true);
	}
	SECTION("ACS Test Config GPS"){
		MockACPOpcode opcode = MOCK_ACS_CONFIG_GPS;
		MockACPManager test(sync, opcode);
		MockACPReturnPacket ret = test.transaction();
		REQUIRE(ret.syncOut == MOCK_ACS);
		REQUIRE(ret.opcodeOut == MOCK_ACS_CONFIG_GPS);
		REQUIRE(ret.success == true);
	}
	SECTION("ACS Test Config Gains"){
		MockACPOpcode opcode = MOCK_ACS_CONFIG_GAINS;
		MockACPManager test(sync, opcode);
		MockACPReturnPacket ret = test.transaction();
		REQUIRE(ret.syncOut == MOCK_ACS);
		REQUIRE(ret.opcodeOut == MOCK_ACS_CONFIG_GAINS);
		REQUIRE(ret.success == true);
	}
}
		
                                                      
