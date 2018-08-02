/*
 * test_I2CManager.cpp
 *
 *  Created on: Dec 27, 2017
 *      Author: cyborg9
 */

#include "test/catch.hpp"
#include "hal/I2CManager.h"
#include "test/testmacros.h"
#include "interfaces/PowerMonitorInterface.h"
#include "interfaces/HotSwapInterface.h"
#include "util/Logger.h"

TEST_CASE("I2CManager Read write register test", "[.][hardware][i2c]"){
	Logger::setMode(MODE_PRINT);
	I2CManager i2c("/dev/i2c-2");
	int powermon = i2c.attachDevice(0xD0); //1.2V rail

	i2c.initialize();

	INFO("Writing to the control register");
	i2c.writeReg(powermon, 0x00, 0x05);

	INFO("Verifying that data was written correctly");
	REQUIRE((unsigned int)i2c.readReg(powermon, 0x00) == 0x05);

	PROMPT("Ready to read from the 1.2V rail on the Switch...");
	uint16_t rawval = ((uint16_t) i2c.readReg(powermon, 0x1E) << 8) | (i2c.readReg(powermon, 0x1F));
	float val = (float)(rawval>>4) * (25.0/1000.0);
	std::cout << "Read " << rawval << " (" << val << "V) from the power monitor" << std::endl;
	Logger::setMode(MODE_NOTHING);
}

TEST_CASE("I2CManager Test Power Monitors", "[.][hardware][i2c]"){
	Logger::setMode(MODE_PRINT);
	I2CManager i2c("/dev/i2c-2");

	PowerMonitorInterface pm1(i2c, i2c.attachDevice(0xCE), 0.02, "A5 1V8");
	PowerMonitorInterface pm2(i2c, i2c.attachDevice(0xD0), 0.02, "A5 1V2");
	PowerMonitorInterface pm3(i2c, i2c.attachDevice(0xD2), 0.02, "ETH 2V5");
	PowerMonitorInterface pm4(i2c, i2c.attachDevice(0xD4), 0.02, "ETH 1V2");

	pm1.configure();
	pm2.configure();
	pm3.configure();
	pm4.configure();

	pm1.getData();
	pm2.getData();
	pm3.getData();
	pm4.getData();

}

TEST_CASE("I2CManager Test Hot Swaps", "[.][hardware][i2c]"){
	Logger::setMode(MODE_PRINT);
	I2CManager i2c("/dev/i2c-2");

	HotSwapInterface hs1(i2c, i2c.attachDevice(0x80), 0.01,"COM 3V3");
	HotSwapInterface hs2(i2c, i2c.attachDevice(0x82), 0.01,"COM VBAT");
	HotSwapInterface hs3(i2c, i2c.attachDevice(0x84), 0.015,"COM 12V0");
	HotSwapInterface hs4(i2c, i2c.attachDevice(0x86), 0.01,"ACS 3V3");
	HotSwapInterface hs5(i2c, i2c.attachDevice(0x88), 0.01,"ACS VBAT");
	HotSwapInterface hs6(i2c, i2c.attachDevice(0x8A), 0.015,"ACS 12V0");
	HotSwapInterface hs7(i2c, i2c.attachDevice(0x92), 0.01,"PLD 3V3");
	HotSwapInterface hs8(i2c, i2c.attachDevice(0x94), 0.01,"PLD VBAT");
	HotSwapInterface hs9(i2c, i2c.attachDevice(0x96), 0.015,"PLD 12V0");
	HotSwapInterface hs10(i2c, i2c.attachDevice(0x98), 0.01,"GPS 3V3");
	HotSwapInterface hs11(i2c, i2c.attachDevice(0x9A), 0.01,"GPS VBAT");
	HotSwapInterface hs12(i2c, i2c.attachDevice(0x9C), 0.01,"AUXCOM 3V3");
	HotSwapInterface hs13(i2c, i2c.attachDevice(0x9E), 0.01,"AUXCOM VBAT");

	hs1.configure();
	hs2.configure();
	hs3.configure();
	hs4.configure();
	hs5.configure();
	hs6.configure();
	hs7.configure();
	hs8.configure();
	hs9.configure();
	hs10.configure();
	hs11.configure();
	hs12.configure();
	hs13.configure();

	hs1.getData();
	hs2.getData();
	hs3.getData();
	hs4.getData();
	hs5.getData();
	hs6.getData();
	hs7.getData();
	hs8.getData();
	hs9.getData();
	hs10.getData();
	hs11.getData();
	hs12.getData();
	hs13.getData();

}


