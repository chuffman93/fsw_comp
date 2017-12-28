/*
 * test_ACPInterface.cpp
 *
 *  Created on: Dec 27, 2017
 *      Author: fsw
 */
#include "test/catch.hpp"
#include "test/testmacros.h"

#include "util/EventHandler.h"
#include "interfaces/ACPInterface.h"

using namespace std;


TEST_CASE("Communicate with RAD", "[.][hardware][acp][rad]"){
	INFO("Setting up Event Handler");
	EventHandler::attachLogger(new Logger());

	int spiid, intrid, acpid;
	SPIManager spi("/dev/spidev32765", 0, 1000000);
	GPIOManager gpio("/sys/class/gpio/");
	ACPPacket test_send(PLD, 8), test_recv;

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
	ACPInterface acp(spi, gpio, spiid, intrid);

	PROMPT("Ready to send to PLD...");
	REQUIRE(acp.transaction(test_send, test_recv) == true);
	cout << "Sync: " << (unsigned int)test_recv.sync << ", Op: " << (unsigned int)test_recv.opcode << ", Length: " << test_recv.message.size() << endl;

	INFO("Tearing down event handler");
	EventHandler::attachLogger(NULL);
}
