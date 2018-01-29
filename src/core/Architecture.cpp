/*
 * architecture.cpp
 *
 *  Created on: Jan 22, 2018
 *      Author: alpa3119
 */

#include "core/Architecture.h"
#include "test/swintegration/MockACP.h"
#include "test/swintegration/MockSubPower.h"

ACS* Architecture::acs = NULL;
CDH* Architecture::cdh = NULL;
COM* Architecture::com = NULL;
EPS* Architecture::eps = NULL;
GPS* Architecture::gps = NULL;
RAD* Architecture::rad = NULL;

//HAL Layer
SPIManager* Architecture::spi = NULL;
GPIOManager* Architecture::gpio = NULL;

InterfaceMode Architecture::mode = SOFTWARE;


void Architecture::buildSPI(){
	//Build the SPI Manager if not built
	if(spi == NULL){
		//Need to build the SPI Manager
		spi = new SPIManager("/dev/spidev32765", 0, 1000000);
	}
}

void Architecture::buildGPIO(){
	//Build GPIO if not already built
	if(gpio == NULL){
		//Need to build the GPIO Manager
		gpio = new GPIOManager("/sys/class/gpio/");
	}
}

void Architecture::buildEPS(){
	if(mode == HARDWARE){
		buildSPI();
		buildGPIO();
		int intid = gpio->attachDevice('E', 6, INT_FALLING);
		int spiid = spi->attachDevice(0);
		ACPInterface* acp = new ACPInterface(*spi, *gpio, spiid, intid, "EPS");

		//TODO Figurout EPS Sub power interface
		int resetid = gpio->attachDevice('E', 10, GPIO_OUTPUT);
		SubPowerInterface* sp = new SubPowerInterface(*gpio, -1, resetid, -1, "EPS");
		eps = new EPS(*acp, *sp);
	}else{
		eps = new EPS(*(new MockACP("EPS")), *(new MockSubPower("EPS")));
	}
}

void Architecture::buildCOM(){
	if(mode == HARDWARE){
		buildSPI();
		buildGPIO();
		int intid = gpio->attachDevice('D', 6, INT_FALLING);
		int spiid = spi->attachDevice(1);
		ACPInterface* acp = new ACPInterface(*spi, *gpio, spiid, intid, "COM");

		int powid = gpio->attachDevice('B', 15, GPIO_OUTPUT);
		int resetid = gpio->attachDevice('A', 11, GPIO_OUTPUT);
		int faultid = gpio->attachDevice('B', 13, GPIO_INPUT);
		SubPowerInterface* sp = new SubPowerInterface(*gpio, powid, resetid, faultid, "COM");
		com = new COM(*acp, *sp);
	}else{
		com = new COM(*(new MockACP("COM")), *(new MockSubPower("COM")));
	}
}

void Architecture::buildACS(){
	if(mode == HARDWARE){
		buildSPI();
		buildGPIO();
		int intid = gpio->attachDevice('A', 4, INT_FALLING);
		int spiid = spi->attachDevice(2);
		ACPInterface* acp = new ACPInterface(*spi, *gpio, spiid, intid, "ACS");

		int powid = gpio->attachDevice('B', 25, GPIO_OUTPUT);
		int resetid = gpio->attachDevice('A', 12, GPIO_OUTPUT);
		int faultid = gpio->attachDevice('B', 5, GPIO_INPUT);
		SubPowerInterface* sp = new SubPowerInterface(*gpio, powid, resetid, faultid, "ACS");
		acs = new ACS(*acp, *sp);
	}else{
		acs = new ACS(*(new MockACP("ACS")), *(new MockSubPower("ACS")));
	}
}

void Architecture::buildRAD(){
	if(mode == HARDWARE){
		buildSPI();
		buildGPIO();
		int intid = gpio->attachDevice('A', 25, INT_FALLING);
		int spiid = spi->attachDevice(3);
		ACPInterface* acp = new ACPInterface(*spi, *gpio, spiid, intid, "RAD");

		int powid = gpio->attachDevice('B', 17, GPIO_OUTPUT);
		int resetid = gpio->attachDevice('E', 11, GPIO_OUTPUT);
		int faultid = gpio->attachDevice('B', 14, GPIO_INPUT);
		SubPowerInterface* sp = new SubPowerInterface(*gpio, powid, resetid, faultid, "RAD");
		rad = new RAD(*acp, *sp);
	}else{
		rad = new RAD(*(new MockACP("RAD")), *(new MockSubPower("RAD")));
	}
}

void Architecture::buildGPS(){

}

void Architecture;:buildCDH(){

}


//TODO: Fault lines need to be fixed
/*			//COM Monitors
COM_3V3_FAULT					 =IOtoInt('B', 13),
COM_VBAT_FAULT					 =IOtoInt('B', 7),
COM_12V0_FAULT					 =IOtoInt('D', 19),
//ACS Monitors
ACS_3V3_FAULT					 =IOtoInt('B', 5),
ACS_VBAT_FAULT					 =IOtoInt('B', 2),
ACS_12V0_FAULT					 =IOtoInt('B', 0),
//PROP Monitors
PROP_3V3_FAULT					 =IOtoInt('B', 8),
PROP_VBAT_FAULT					 =IOtoInt('B', 16),
PROP_12V0_FAULT					 =IOtoInt('B', 6),
//PLD Monitors
PLD_3V3_FAULT					 =IOtoInt('B', 14),
PLD_VBAT_FAULT					 =IOtoInt('D', 25),
PLD_12V0_FAULT					 =IOtoInt('B', 12),
*/


std::vector<SubsystemBase*> Architecture::buildHSVector(){
	std::vector<SubsystemBase*> ret;
	if(acs != NULL) ret.push_back(acs);
	if(cdh != NULL) ret.push_back(cdh);
	if(com != NULL) ret.push_back(com);
	if(eps != NULL) ret.push_back(eps);
	if(gps != NULL) ret.push_back(gps);
	if(rad != NULL) ret.push_back(rad);

	return ret;
}

std::vector<SubsystemBase*> Architecture::buildInitVector(){
	std::vector<SubsystemBase*> ret;
	if(acs != NULL) ret.push_back(acs);
	if(cdh != NULL) ret.push_back(cdh);
	if(com != NULL) ret.push_back(com);
	if(eps != NULL) ret.push_back(eps);
	if(gps != NULL) ret.push_back(gps);
	if(rad != NULL) ret.push_back(rad);

	return ret;
}

void Architecture::setInterfaceMode(InterfaceMode mode){
	Architecture::mode = mode;
}
