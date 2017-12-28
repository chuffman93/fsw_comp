/*
 * test_OneWireManager.cpp
 *
 *  Created on: Dec 27, 2017
 *      Author: cyborg9
 */

#include "hal/OneWireManager.h"
#include "test/catch.hpp"
#include "test/testmacros.h"
#include <unistd.h>

using namespace std;



TEST_CASE("Test OneWireManager", "[.][hardware][onewire]"){
	OneWireManager onewire("/sys/bus/w1/devices/w1_bus_master/0/");
	int a5monitor = onewire.attachDevice("TEMP1");

	onewire.writeToFile(a5monitor, "start", "1");
	usleep(1000*1000);
	string data = onewire.readFromFile(a5monitor, "temp");

	cout << "Read the following data from A5 temp sensor" << endl << data;
}


