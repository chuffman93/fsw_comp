/*
 * test_TempInterface.cpp
 *
 *  Created on: Dec 28, 2017
 *      Author: cyborg9
 */

#include "interfaces/TempInterface.h"
#include "test/catch.hpp"
#include "test/mockhal/MockOneWireManager.h"
#include "util/EventHandler.h"
#include <math.h>


TEST_CASE("Test that TempInterface parses data correctly", "[interfaces][tempinterface]"){
	//EventHandler::attachLogger(new Logger());
	MockOneWireManager onewire;
	int id = onewire.attachDevice("");
	TempInterface t(onewire, id);

	SECTION("Ensure that NAN is returned when no data is available"){
		REQUIRE(isnan(t.getSample()) != 0);
	}
	SECTION("Ensure that the right temperature is returned"){
		std::string test = "t=30100\nc0 01 4b 46 71 ff 10 10 9a VALID";
		onewire.setData(id, test);
		REQUIRE(fabs(t.getSample() - 30.100) < 0.001);
	}
	SECTION("Ensure that the valid flag is checked"){
		std::string test = "t=30100\nc0 01 4b 46 71 ff 10 10 9a INVALID";
		onewire.setData(id, test);
		REQUIRE(isnan(t.getSample()) != 0);
	}
	EventHandler::attachLogger(NULL);
}
