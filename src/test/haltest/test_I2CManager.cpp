/*
 * test_I2CManager.cpp
 *
 *  Created on: Dec 27, 2017
 *      Author: cyborg9
 */

#include "test/catch.hpp"
#include "hal/I2CManager.h"
#include "test/testmacros.h"

TEST_CASE("I2CManager Read write register test", "[.][hardware][i2c]"){
	I2CManager i2c("/dev/i2c-2");
	int powermon = i2c.attachDevice(0xCE); //1.2V rail

	i2c.initialize();

	INFO("Writing to the control register");
	i2c.writeReg(powermon, 0x00, 0x05);

	INFO("Verifying that data was written correctly");
	REQUIRE(i2c.readReg(powermon, 0x00) == 0x05);

	PROMPT("Ready to read from the 1.2V rail on the A5...");
	uint16_t rawval = ((uint16_t) i2c.readReg(powermon, 0x1E) << 8) | (i2c.readReg(powermon, 0x1F));
	float val = (float)rawval * (25.0/1000.0);
	std::cout << "Read " << rawval << "V from the power monitor";
}



