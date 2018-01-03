/*
 * test_EPS.cpp
 *
 *  Created on: Dec 30, 2017
 *      Author: alpa3119
 */

#include "test/catch.hpp"
#include "subsystem/EPS.h"
#include "test/mockhal/MockACPManager.h"
#include "core/FileSystem.h"
#include "hal/GPIOManager.h"

#include <stdio.h>
#include <vector>

GPIOManager dummy("");

TEST_CASE("EPS Test Initialization Routine", "[subsystem][EPS]"){
	//initialize/setup
	MockACPInterface acp;
	SubPowerInterface subPower(dummy, 0, 0, 0, "");
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
	MockACPInterface acp;
	SubPowerInterface subPower(dummy, 0, 0, 0, "");
	EPS eps(acp,subPower);
	//call EPS initialize
	eps.initialize();
	//call EPS health and status
	eps.getHealthStatus();
	//validate that it sends the health and status opcode
	REQUIRE(acp.sentOpcodes.end() != std::find(acp.sentOpcodes.begin(), acp.sentOpcodes.end(), OP_HEALTHSTATUS));
	//validate that data is process correctly
	REQUIRE(eps.getBatteryCapacity() == 100);
	//TODO: validate that correct message buffer is passed to file interface

}


