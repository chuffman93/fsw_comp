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
	}

}

TEST_CASE("Testing for sleep mode","[.][epsSL]"){
	Architecture::buildTime();
	Logger::setMode(MODE_PRINT);
	Logger::setLevel(LEVEL_DEBUG);
	Architecture::setInterfaceMode(SOFTWARE);
	Architecture::buildGPS();
	Architecture::setInterfaceMode(HARDWARE);
	Architecture::buildEPS();

	EPS *eps = Architecture::getEPS();
	GPS* gps = Architecture::getGPS();

	Logger::log(LEVEL_FATAL, "Initializing HAL");
	vector<HardwareManager*> halinit = Architecture::buildHALInitVector();
	for(vector<HardwareManager*>::iterator i = halinit.begin(); i != halinit.end(); i++){
		(*i)->initialize();
	}

	gps->initialize();
	eps->initialize();
	gps->fetchNewGPS();
	gps->getBestXYZI();


}
