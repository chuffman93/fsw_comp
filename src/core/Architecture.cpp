/*
 * architecture.cpp
 *
 *  Created on: Jan 22, 2018
 *      Author: alpa3119
 */

#include "core/Architecture.h"

void Architecture::buildSubsystem(Subsystem subsystem){
	if(subsystem != CDH && subsystem != GPS){
		buildECARSubsystem(subsystem);
	}
	if(mode == HARDWARE){
		switch(subsystem){
		case ACS:

		}
	}
}

void Architecture::buildECARSubsystem(Subsystem subsystem){
	if(mode == HARDWARE){
		//-------------_BUILD ACP INTERFACE--------------------
		if(spi == nullptr){
			//Need to build the SPI Manager
			spi = new SPIManager("/dev/spidev32765", 0, 1000000);
		}
		if(gpio == nullptr){
			//Need to build the GPIO Manager
			gpio = new GPIOManager("/sys/class/gpio/");
		}

		int spiid, intid;
		switch(subsystem){
		case EPS:
			intid = gpio->attachDevice('E', 6, INT_FALLING);
			spiid = spi->attachDevice(0);
			break;
		case COM:
			intid = gpio->attachDevice('D', 6, INT_FALLING);
			spiid = spi->attachDevice(1);
			break;
		case ACS:
			intid = gpio->attachDevice('A', 4, INT_FALLING);
			spiid = spi->attachDevice(2);
			break;
		case RAD:
			intid = gpio->attachDevice('A', 25, INT_FALLING);
			spiid = spi->attachDevice(3);
			break;
		}

		ACPInterface* acp = new ACPInterface(*spi, *gpio, spiid, intid);

		//-------------_BUILD SUB POWER INTERFACE--------------------
		SubPowerInterface* sp = nullptr;
		if(subsystem == EPS){
			//TODO Figurout EPS Sub power interface
			int resetid = gpio->attachDevice('E', 10, GPIO_OUTPUT);
			sp = SubPowerInterface(*gpio, -1, resetid, -1, "EPS");
		}else{
			int powid, resetid, faultid;

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

			switch(subsystem){
			case COM:
				powid = gpio->attachDevice('B', 15, GPIO_OUTPUT);
				resetid = gpio->attachDevice('A', 11, GPIO_OUTPUT);
				faultid = gpio->attachDevice('B', 13, GPIO_INPUT);
				sp = SubPowerInterface(*gpio, powid, resetid, faultid, "COM");
				break;
			case ACS:
				powid = gpio->attachDevice('B', 25, GPIO_OUTPUT);
				resetid = gpio->attachDevice('A', 12, GPIO_OUTPUT);
				faultid = gpio->attachDevice('B', 5, GPIO_INPUT);
				sp = SubPowerInterface(*gpio, powid, resetid, faultid, "ACS");
				break;
			case RAD:
				powid = gpio->attachDevice('B', 17, GPIO_OUTPUT);
				resetid = gpio->attachDevice('E', 11, GPIO_OUTPUT);
				faultid = gpio->attachDevice('B', 14, GPIO_INPUT);
				sp = SubPowerInterface(*gpio, powid, resetid, faultid, "RAD");
				break;
			}
		}

		//------------------BUILD SUBSYSTEM---------------------
		switch(subsystem){
		case EPS:
			eps = new EPS(*acp, *sp);
			break;
		case COM:
			com = new COM(*acp, *sp);
			break;
		case ACS:
			acs = new ACS(*acp, *sp);
			break;
		case RAD:
			rad = new RAD(*acp, *sp);
			break;
		}
	}
	if(mode == SOFTWARE){

	}


}

void Architecture::setHalMode(HalMode mode){
	Architecture::mode = mode;
}
