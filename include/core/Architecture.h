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
#include "core/GroundCommunication.h"
#include "core/ScheduleManager.h"
#include "interfaces/HotSwapInterface.h"
#include "interfaces/TempInterface.h"
#include "interfaces/PowerMonitorInterface.h"
#include "hal/I2CManager.h"

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
	static void buildGPS();
	static void buildCDH();
	static void buildScheduleManager();
	static void buildTime();
	static void buildGND();
	static void buildBeaconManager();
	static void buildHotSwapInterfaces();
	static void buildTempInterfaces();
	static void buildPowerMonitorInterfaces();
	static void setInterfaceMode(InterfaceMode mode);

	static std::vector<SubsystemBase*> buildHSVector();
	static std::vector<SubsystemBase*> buildCFVector();
	static std::vector<SubsystemBase*> buildInitVector();
	static std::vector<HardwareManager*> buildHALInitVector();
	static std::map<FSWMode, std::vector<SubsystemBase*> > buildModeSequencing();
	static GPS* getGPS();
	static ACS* getACS();
	static RAD* getRAD();
	static EPS* getEPS();
	static COM* getCOM();
	static GroundCommunication* getGND();
	static ScheduleManager* getSchedulerManager();
	static BeaconManager* getBeaconManager();


private:
	static void buildI2C();
	static void buildOneWire();
	static void buildSPI();
	static void buildGPIO();
	static void buildUART();
	static ACS* acs;
	static CDH* cdh;
	static COM* com;
	static EPS* eps;
	static GPS* gps;
	static RAD* rad;
	static GroundCommunication* gnd;
	static ScheduleManager* sch;
	static BeaconManager* bcn;

	static I2CManager* i2c;
	static OneWireManager* onewire;

	//HAL Layer
	static SPIManager* spi;
	static GPIOManager* gpio;
	static UARTManager* uart;

	static std::vector<HotSwapInterface*> hotswaps;
	static std::vector<PowerMonitorInterface*> powermonitors;
	static std::vector<TempInterface*> tempmonitors;

	static InterfaceMode mode;

};




#endif /* ARCHITECTURE_H_ */
