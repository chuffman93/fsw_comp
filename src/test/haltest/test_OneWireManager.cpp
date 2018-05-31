/*
 * test_OneWireManager.cpp
 *
 *  Created on: Dec 27, 2017
 *      Author: cyborg9
 */

#include "hal/OneWireManager.h"
#include "interfaces/TempInterface.h"
#include "test/catch.hpp"
#include "test/testmacros.h"
#include "util/Logger.h"
#include <unistd.h>

using namespace std;


/*
 * Should get something like
 * t=28000
 * c0 01 4b 46 71 ff 10 10 9a VALID
 *
 */


TEST_CASE("Test OneWireManager", "[.][hardware][onewire]"){
	Logger::setMode(MODE_PRINT);
	OneWireManager onewire("/sys/bus/w1/devices/w1_bus_master1/");
	int a5monitor = onewire.attachDevice("TEMP1");
	onewire.initialize();

	onewire.writeToFile(a5monitor, "start", "1");
	usleep(1000*1000);
	string data = onewire.readFromFile(a5monitor, "TEMP1");

	cout << "Read the following data from A5 temp sensor" << endl << data;
	Logger::setMode(MODE_NOTHING);
}

TEST_CASE("Test Thermal Sensors","[.][hardware][onewire][tempinterface]" ){
	Logger::setMode(MODE_PRINT);

	OneWireManager onewire("/sys/bus/w1/devices/w1_bus_master1/");

	TempInterface t0(onewire, onewire.attachDevice("TEMP0"));
	TempInterface t1(onewire, onewire.attachDevice("TEMP1"));

	onewire.initialize();


	while (1) {
		t0.beginSample();
		usleep(1000000);
		t0.getSample();
	}

	t1.beginSample();
	usleep(750000);
	t1.getSample();







}


