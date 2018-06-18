#include "test/catch.hpp"
#include "test/testmacros.h"
#include "core/Architecture.h"

TEST_CASE("Testing for power budget","[.][epsBG]"){
	Architecture::buildTime();
	Logger::setMode(MODE_PRINT);
	Logger::setLevel(LEVEL_INFO);
	Architecture::setInterfaceMode(HARDWARE);
	Architecture::buildEPS();
	EPS *eps = Architecture::getEPS();

	Logger::log(LEVEL_FATAL, "Initializing HAL");
	vector<HardwareManager*> halinit = Architecture::buildHALInitVector();
	for(vector<HardwareManager*>::iterator i = halinit.begin(); i != halinit.end(); i++){
		(*i)->initialize();
	}

	eps->initialize();

	for(int y = 0; y <= 60; y++){
		eps->getHealthStatus();
		sleep(1);
	}

}
