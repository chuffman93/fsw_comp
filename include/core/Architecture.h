/*
 * architecture.h
 *
 *  Created on: Jan 22, 2018
 *      Author: alpa3119
 */

#ifndef ARCHITECTURE_H_
#define ARCHITECTURE_H_

#include "subsystem/ACS.h"
#include "subsystem/CDH.h"
#include "subsystem/COM.h"
#include "subsystem/EPS.h"
#include "subsystem/GPS.h"
#include "subsystem/RAD.h"

enum InterfaceMode {
	SOFTWARE,
	HARDWARE,
};

class Architecture{
public:
	static void buildEPS();
	static void buildCOM();
	static void buildACS();
	static void buildRAD();
	static void setInterfaceMode(InterfaceMode mode);

	static std::vector<SubsystemBase*> buildHSVector();

private:
	static void buildSPI();
	static void buildGPIO();
	static ACS* acs;
	static CDH* cdh;
	static COM* com;
	static EPS* eps;
	static GPS* gps;
	static RAD* rad;

	//HAL Layer
	static SPIManager* spi;
	static GPIOManager* gpio;
	static InterfaceMode mode;

};




#endif /* ARCHITECTURE_H_ */
