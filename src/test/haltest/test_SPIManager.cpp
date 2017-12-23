/*
 * test_SPIManager.cpp
 *
 *  Created on: Dec 4, 2017
 *      Author: fsw
 */
#include "test/testmacros.h"
#include "hal/SPIManager.h"
#include <unistd.h>
#include "test/catch.hpp"
#include <iostream>

using namespace std;

TEST_CASE("SPI TX/RX", "[.][hardware][spi]"){
	string bus = "/dev/spidev32765";
	uint8_t mode = 0;
	uint32_t speed = 1000000;
	SPIManager test(bus,mode,speed);
	int ss = 3;
	int id = test.attachDevice(ss);
	test.initialize();

	PROMPT("Ready to send byte over SPI to cs " << ss);
	uint8_t byte = 0x76;
	test.sendbyte(id,byte);

	PROMPT("Ready to read byte over SPI from cs " << ss);

	unsigned int returnByte = test.receivebyte(id);
	cout << "Received " << returnByte << endl;
}
