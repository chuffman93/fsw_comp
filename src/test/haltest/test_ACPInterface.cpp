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


TEST_CASE("Communicate with COM", "[.][hardware][acp][com]"){
	INFO("Setting up Event Handler");
	Logger::setMode(MODE_PRINT);
	Logger::setLevel(LEVEL_DEBUG);


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

TEST_CASE("Communicate with RAD", "[.][hardware][acp][rad]"){
	INFO("Setting up Event Handler");
	Logger::setMode(MODE_PRINT);
	Logger::setLevel(LEVEL_DEBUG);


	int spiid, intrid, acpid;
	SPIManager spi("/dev/spidev32765", 0, 1000000);
	GPIOManager gpio("/sys/class/gpio/");
	ACPPacket test_send(RAD_SYNC, 8), test_recv;

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
	ACPInterface acp(spi, gpio, spiid, intrid,"");

	PROMPT("Ready to send to PLD...");
	REQUIRE(acp.transaction(test_send, test_recv) == true);
	cout << "Sync: " << (unsigned int)test_recv.sync << ", Op: " << (unsigned int)test_recv.opcode << ", Length: " << test_recv.message.size() << endl;

	INFO("Tearing down event handler");
	Logger::setMode(MODE_NOTHING);
}

TEST_CASE("Communicate with ACS", "[.][hardware][acp][acs]"){
	INFO("Setting up Event Handler");
	Logger::setMode(MODE_PRINT);
	Logger::setLevel(LEVEL_DEBUG);

	int spiid, intrid, acpid;
	SPIManager spi("/dev/spidev32765", 0, 1000000);
	GPIOManager gpio("/sys/class/gpio/");
	ACPPacket test_send(ACS_SYNC, 8), test_recv;

	INFO("Register the Devices");
	int powerid = gpio.attachDevice('B', 25, GPIO_OUTPUT);
	int resetid = gpio.attachDevice('A', 12, GPIO_OUTPUT);
	spiid = spi.attachDevice(2);
	intrid = gpio.attachDevice('A', 4, INT_FALLING);

	INFO("Initialize hardware");
	spi.initialize();
	gpio.initialize();
	gpio.set(powerid, HIGH);
	gpio.set(resetid, HIGH);

	INFO("Create an ACPInterface")
	ACPInterface acp(spi, gpio, spiid, intrid,"");
	acp.setTimeouts(1500000000,10,600);


	PROMPT("Ready to send to ACS...");
	REQUIRE(acp.transaction(test_send, test_recv) == true);
	cout << "Sync: " << (unsigned int)test_recv.sync << ", Op: " << (unsigned int)test_recv.opcode << ", Length: " << test_recv.message.size() << endl;

	INFO("Tearing down event handler");
	Logger::setMode(MODE_NOTHING);
}

TEST_CASE("Communicate with EPS", "[.][hardware][acp][EPS]"){
	INFO("Setting up Event Handler");
	Logger::setMode(MODE_PRINT);
	Logger::setLevel(LEVEL_DEBUG);
	int spiid, intrid, acpid;
	SPIManager spi("/dev/spidev32765", 0, 1000000);
	GPIOManager gpio("/sys/class/gpio/");
	ACPPacket test_send(EPS_SYNC, 8), test_recv;

	INFO("Register the Devices");
	int resetid = gpio.attachDevice('E', 10, GPIO_OUTPUT);
	spiid = spi.attachDevice(0);
	intrid = gpio.attachDevice('E', 6, INT_FALLING);

	INFO("Initialize hardware");
	spi.initialize();
	gpio.initialize();
	gpio.set(resetid, HIGH);

	INFO("Create an ACPInterface")
	ACPInterface acp(spi, gpio, spiid, intrid,"EPSTEST");
	acp.setTimeouts(2000000000,10,1);

	PROMPT("Ready to send to EPS...");
	REQUIRE(acp.transaction(test_send, test_recv) == true);
	cout << "Sync: " << (unsigned int)test_recv.sync << ", Op: " << (unsigned int)test_recv.opcode << ", Length: " << test_recv.message.size() << endl;

	INFO("Tearing down event handler");
	Logger::setMode(MODE_NOTHING);
}
