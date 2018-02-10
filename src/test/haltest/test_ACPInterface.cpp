/*
 * test_ACPInterface.cpp
 *
 *  Created on: Dec 27, 2017
 *      Author: fsw
 */
#include "test/catch.hpp"
#include "test/testmacros.h"

#include "util/Logger.h"
#include "interfaces/ACPInterface.h"

using namespace std;


TEST_CASE("Communicate with COM", "[hardware][acp][com]"){
	INFO("Setting up Event Handler");
	Logger::setMode(MODE_PRINT);

	int spiid, intrid, acpid;
	SPIManager spi("/dev/spidev32765", 0, 1000000);
	GPIOManager gpio("/sys/class/gpio/");
	ACPPacket test_send(COM_SYNC, 8), test_recv;

	INFO("Register the Devices");
	int powerid = gpio.attachDevice('B', 15, GPIO_OUTPUT);
	int resetid = gpio.attachDevice('A', 11, GPIO_OUTPUT);
	spiid = spi.attachDevice(1);
	intrid = gpio.attachDevice('D', 6, INT_FALLING);

	INFO("Initialize hardware");
	spi.initialize();
	gpio.initialize();
	gpio.set(powerid, HIGH);
	gpio.set(resetid, HIGH);

	INFO("Create an ACPInterface")
	ACPInterface acp(spi, gpio, spiid, intrid,"");

	PROMPT("Ready to send to COM...");
	REQUIRE(acp.transaction(test_send, test_recv) == true);
	cout << "Sync: " << (unsigned int)test_recv.sync << ", Op: " << (unsigned int)test_recv.opcode << ", Length: " << test_recv.message.size() << endl;

	INFO("Tearing down event handler");
	Logger::setMode(MODE_NOTHING);
}

