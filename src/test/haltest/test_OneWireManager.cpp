/*
 * test_OneWireManager.cpp
 *
 *  Created on: Dec 27, 2017
 *      Author: cyborg9
 */

#include "hal/OneWireManager.h"
#include "test/catch.hpp"
#include "test/testmacros.h"
#include "util/EventHandler.h"
#include <unistd.h>

using namespace std;


/*
 * Should get something like
 * t=28000
 * c0 01 4b 46 71 ff 10 10 9a VALID
 *
 */


TEST_CASE("Test OneWireManager", "[.][hardware][onewire]"){
	EventHandler::attachLogger(new Logger());
	OneWireManager onewire("/sys/bus/w1/devices/w1_bus_master1/");
	int a5monitor = onewire.attachDevice("TEMP1");
	onewire.initialize();

	onewire.writeToFile(a5monitor, "start", "1");
	usleep(1000*1000);
	string data = onewire.readFromFile(a5monitor, "temp");

	cout << "Read the following data from A5 temp sensor" << endl << data;
	EventHandler::attachLogger(NULL);
}


