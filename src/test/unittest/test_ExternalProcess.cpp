/*
 * test_ExternalProcess.cpp
 *
 *  Created on: Jan 8, 2018
 *      Author: chuffman93
 */

#include "test/catch.hpp"
#include "interfaces/ExternalProcess.h"

#include<iostream>
using namespace std;

ExternalProcess xp;


TEST_CASE("Test that ExternalProcess launches the different processes","[externalprocess]"){


	SECTION("Check that SLATTACH launches"){
		xp.launchSlattach();
		REQUIRE(xp.checkPID("slattach") != -1);
	}
}


TEST_CASE("Test that the ExternalProcess kills the different processes","[externalprocess]"){

	SECTION("Check that SLATTACH dies"){
		xp.closeSlattach();
		REQUIRE(xp.checkPID("slattach") == -1);
	}
}

