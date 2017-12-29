/*
 * test_PowerMonitorInterface.cpp
 *
 *  Created on: Dec 29, 2017
 *      Author: cyborg9
 */


#include "test/catch.hpp"
#include "test/testmacros.h"
#include "test/mockhal/MockI2CManager.h"
#include "interfaces/PowerMonitorInterface.h"


TEST_CASE("Test Power Monitor Interface", "[interfaces][powermonitor]"){
	MockI2CManager i2c;
	int id = i2c.attachDevice(0);
	PowerMonitorInterface pm(i2c, id, 0.02, "Test");
	i2c.writeReg(id, 0x1F, 0x84); //3.3V
	i2c.writeReg(id, 0x14, 0x01); //0.5A
	i2c.writeReg(id, 0x15, 0x90);

	pm.configure();
	PowerMonitorData dat = pm.getData();
	REQUIRE(i2c.readReg(id, 0x00) == 0x05);
	REQUIRE(fabs(dat.voltage - 3.3) < 0.01);
	REQUIRE(fabs(dat.current - 0.5) < 0.01);
}



