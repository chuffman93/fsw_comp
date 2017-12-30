
#include "test/catch.hpp"
#include "test/testmacros.h"
#include "test/mockhal/MockI2CManager.h"
#include "interfaces/HotSwapInterface.h"


TEST_CASE("Test Hot Swap Interface", "[interfaces][hotswap]"){
	MockI2CManager i2c;
	int id = i2c.attachDevice(0);
	HotSwapInterface hs(i2c, id, 0.02, "Test");
	//3.3V 0.5A
	VECTOROFDATA(testdata, uint8_t, 0x20, 0x18, 0x13);
	i2c.setRead(id, testdata);
	hs.configure();
	HotSwapData dat = hs.getData();
	REQUIRE(i2c.getWritten(id)[0] == 0x05);
	REQUIRE(fabs(dat.voltage-3.3) < 0.01);
	REQUIRE(fabs(dat.current-0.5) < 0.01);
}
