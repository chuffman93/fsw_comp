/*
 * CDH.h
 *
 *  Created on: Dec 22, 2017
 *      Author: adam
 */

#ifndef INCLUDE_SUBSYSTEM_CDH_H_
#define INCLUDE_SUBSYSTEM_CDH_H_

#include "SubsystemBase.h"
#include "interfaces/HotSwapInterface.h"
#include "interfaces/TempInterface.h"
#include "interfaces/PowerMonitorInterface.h"
//#include <vector>


class CDH: public SubsystemBase{
public:
	CDH(std::vector<HotSwapInterface*> * hotswaps, std::vector<PowerMonitorInterface*> * powermonitors, std::vector<TempInterface*> * thermalsensors, OneWireManager* onewire);
	~CDH();

	//Will set up the Gpio lines and the acp devices
	bool initialize();
	//Handles any mode transition needs as well as any needs for tasks to be done in a mode.
	void handleMode(FSWMode transition);
	//Handles the capturing and storing of the health and status for a subsystem (Maybe find someway to implement the autocoding stuff?)
	void getHealthStatus();
	ACPPacket sendOpcode(uint8_t opcode, std::vector<uint8_t> buffer);


	HealthFileStruct health;

	private:
	//Various methods for data collection
	std::vector<HotSwapData> collectHotswap();
	std::vector<PowerMonitorData> collectPowermon();
	std::vector<float> collectTherm();

	std::vector<HotSwapInterface*> * HotSwaps;
	std::vector<PowerMonitorInterface*> * PowerMonitors;
	std::vector<TempInterface*> * ThermalSensors;
	OneWireManager * OneWireMan;



	Lock lock;
	LogTags tags;


};




#endif /* INCLUDE_SUBSYSTEM_CDH_H_ */
