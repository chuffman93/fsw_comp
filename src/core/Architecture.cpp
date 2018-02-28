/*
 * architecture.cpp
 *
 *  Created on: Jan 22, 2018
 *      Author: alpa3119
 */

#include "core/Architecture.h"
#include "test/swintegration/MockACP.h"
#include "test/swintegration/MockSubPower.h"
#include "test/swintegration/MockNMEA.h"

ACS* Architecture::acs = NULL;
CDH* Architecture::cdh = NULL;
COM* Architecture::com = NULL;
EPS* Architecture::eps = NULL;
GPS* Architecture::gps = NULL;
RAD* Architecture::rad = NULL;

GroundCommunication* Architecture::gnd = NULL;
ScheduleManager* Architecture::sch = NULL;

//HAL Layer
SPIManager* Architecture::spi = NULL;
GPIOManager* Architecture::gpio = NULL;
UARTManager* Architecture::uart = NULL;

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

void Architecture::buildUART(){
	//Build UART if not already built
	if(uart == NULL){
		uart = new UARTManager("/dev/ttyS1");
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
		sp->configDelay(100,2000);
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
	if(mode == HARDWARE){
		buildUART();
		buildGPIO();
		NMEAInterface* nmea = new NMEAInterface(*uart);
		//TODO: Actually figure this shit out
		int powid = gpio->attachDevice('B', 27, GPIO_OUTPUT);
		SubPowerInterface* sp = new SubPowerInterface(*gpio, powid, -1, -1, "GPS");
		gps = new GPS(*nmea, *sp);
	}else{
		gps = new GPS(*(new MockNMEA()), *(new MockSubPower("GPS")));
	}
}

void Architecture::buildCDH(){

}

void Architecture::buildTime(){
	initializeTime();
}

void Architecture::buildGND(){
	//Yay for code reuse
	//TODO: Rewrite Ground Comms so this isn't shit
	gnd = new GroundCommunication(buildHSVector());
}

void Architecture::buildScheduleManager(){
	sch = new ScheduleManager();
}

ScheduleManager* Architecture::getSchedulerManager(){
	assert(sch != NULL);
	return sch;
}

std::map<FSWMode, std::vector<SubsystemBase*> > Architecture::buildModeSequencing(){
	std::map<FSWMode, std::vector<SubsystemBase*> > sequences;

	std::vector<SubsystemBase*> nominal;
	if(cdh != NULL) nominal.push_back(cdh);
	if(gps != NULL) nominal.push_back(gps);
	if(com != NULL) nominal.push_back(com);
	if(acs != NULL) nominal.push_back(acs);
	if(rad != NULL) nominal.push_back(rad);
	if(eps != NULL) nominal.push_back(eps);


	// What is the purpose of this? -> PLD doesn't actually exit in this order for some odd reason
	std::vector<SubsystemBase*> pld2bus;
	if(cdh != NULL) nominal.push_back(cdh);
	if(gps != NULL) nominal.push_back(gps);
	if(rad != NULL) nominal.push_back(rad);
	if(com != NULL) nominal.push_back(com);
	if(acs != NULL) nominal.push_back(acs);
	if(eps != NULL) nominal.push_back(eps);

	sequences.insert(std::pair<FSWMode, std::vector<SubsystemBase*> >
	(Mode_Bus, nominal));
	sequences.insert(std::pair<FSWMode, std::vector<SubsystemBase*> >
	(Mode_Payload, nominal));
	sequences.insert(std::pair<FSWMode, std::vector<SubsystemBase*> >
	(Mode_Com, nominal));
	sequences.insert(std::pair<FSWMode, std::vector<SubsystemBase*> >
	(Mode_Reset, nominal));
	sequences.insert(std::pair<FSWMode, std::vector<SubsystemBase*> >
	(Trans_BusToPayload, nominal));
	sequences.insert(std::pair<FSWMode, std::vector<SubsystemBase*> >
	(Trans_PayloadToBus, nominal));
	sequences.insert(std::pair<FSWMode, std::vector<SubsystemBase*> >
	(Trans_BusToCom, nominal));
	sequences.insert(std::pair<FSWMode, std::vector<SubsystemBase*> >
	(Trans_ComToBus, nominal));

	return sequences;
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
	if(eps != NULL) ret.push_back(eps);
	if(com != NULL) ret.push_back(com);
	if(acs != NULL) ret.push_back(acs);
	if(rad != NULL) ret.push_back(rad);
	if(cdh != NULL) ret.push_back(cdh);
	if(gps != NULL) ret.push_back(gps);

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

std::vector<HardwareManager*> Architecture::buildHALInitVector(){
	std::vector<HardwareManager*> ret;
	if(spi != NULL) ret.push_back(spi);
	if(gpio != NULL) ret.push_back(gpio);
	if(uart != NULL) ret.push_back(uart);
	return ret;
}

GPS* Architecture::getGPS(){
	assert(gps != NULL);
	return gps;
}

ACS* Architecture::getACS(){
	assert(acs != NULL);
	return acs;
}

GroundCommunication* Architecture::getGND(){
	assert(gnd != NULL);
	return gnd;
}

void Architecture::setInterfaceMode(InterfaceMode mode){
	Architecture::mode = mode;
}
