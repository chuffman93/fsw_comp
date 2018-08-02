/*
 * test_RAD.cpp
 *
 *  Created on: Feb 12, 2018
 *      Author: fsw
 */
#include "test/catch.hpp"
#include "test/testmacros.h"
#include "core/Architecture.h"
#include "core/Watchdog.h"
#include "util/Logger.h"
#include "subsystem/RAD.h"
#include "test/swintegration/MockSubPower.h"

using namespace std;

TEST_CASE("Test Data run with RAD", "[.][hardware][fullrad]"){
	Architecture::buildTime();
	Logger::setMode(MODE_PRINT);
	Logger::setLevel(LEVEL_DEBUG);
	Architecture::setInterfaceMode(HARDWARE);
	Architecture::buildEPS();
	Architecture::buildRAD();
	Watchdog watchdog(*(Architecture::getEPS()));
	watchdog.AddThread(Thread::GetMyThreadID());
	Architecture::getRAD()->watchdog = &watchdog;
	RAD* rad = Architecture::getRAD();

	Logger::log(LEVEL_FATAL, "Initializing HAL");
	vector<HardwareManager*> halinit = Architecture::buildHALInitVector();
	for(vector<HardwareManager*>::iterator i = halinit.begin(); i != halinit.end(); i++){
		(*i)->initialize();
	}

	rad->initialize();

	rad->commandCollectionBegin();

	sleep(15);

	rad->commandCollectionEnd();

}

TEST_CASE("Ping RAD", "[.][hardware][radping]"){
	INFO("Setting up Logger");
	Logger::setMode(MODE_PRINT);
	Logger::setLevel(LEVEL_DEBUG);

	INFO("Build HAL Layer");
	int spiid, intrid, acpid;
	SPIManager spi("/dev/spidev32765", 0, 1000000);
	GPIOManager gpio("/sys/class/gpio/");

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
	ACPInterface acp(spi,gpio, spiid, intrid,"RAD");
	acp.setTimeouts(10, 10, 100);

	INFO("Create a SubPowerInterface")
	SubPowerInterface sp(gpio, powerid, resetid, -1, "RAD");
	sp.configDelay(100, 4000);
	sp.powerOn();

	system("ping 10.14.134.207");
}



