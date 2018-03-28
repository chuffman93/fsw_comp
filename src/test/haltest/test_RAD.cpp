/*
 * test_RAD.cpp
 *
 *  Created on: Feb 12, 2018
 *      Author: fsw
 */
#include "test/catch.hpp"
#include "test/testmacros.h"

#include "util/Logger.h"
#include "subsystem/RAD.h"
#include "test/swintegration/MockSubPower.h"

using namespace std;

TEST_CASE("Test Data run with RAD", "[.][hardware][fullrad]"){
	INFO("Setting up Logger");
	Logger::setMode(MODE_PRINT);
	Logger::setLevel(LEVEL_DEBUG);

	INFO("Set up reboot count");
	FileManager::updateRebootCount();

	INFO("Build HAL Layer");
	int spiid, intrid, acpid;
	SPIManager spi("/dev/spidev32765", 0, 1000000);
	GPIOManager gpio("/sys/class/gpio/");

	INFO("Register the Devices");
	int powerid = gpio.attachDevice('B', 17, GPIO_OUTPUT);
	int resetid = gpio.attachDevice('E', 11, GPIO_OUTPUT);
	spiid = spi.attachDevice(3);
	intrid = gpio.attachDevice('A', 25, INT_FALLING);

	INFO("Initialize hardware");
	spi.initialize();
	gpio.initialize();
	gpio.set(powerid, HIGH);
	gpio.set(resetid, HIGH);

	INFO("Create an ACPInterface")
	ACPInterface acp(spi,gpio, spiid, intrid,"RAD");
	acp.setTimeouts(10, 10, 100);

	INFO("Create a SubPowerInterface")
	SubPowerInterface sp(gpio, powerid, resetid, -1, "RAD");
	sp.configDelay(100, 4000);
	sp.powerOn();

	MockSubPower sp2("RAD");

	INFO("Create RAD Object");
	RAD rad(acp, sp2);

	INFO("Initialize RAD");
	rad.initialize();

	INFO("Start Science Collection");
	PROMPT("Ready to start science collection:");
	rad.handleMode(Trans_BusToPayload);

	cout << "Beginning data collection from MiniRAD..." << endl;
	PROMPT("Press enter to end:");

	INFO("Shutting down Science Collection");
	rad.handleMode(Trans_PayloadToBus);
}



