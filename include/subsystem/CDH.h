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
#include <sys/sysinfo.h>

struct CDHState {
	unsigned long cpu1;
	unsigned long cpu5;
	unsigned long cpu15;
	unsigned short memory;
};

class CDH: public SubsystemBase{
public:
	CDH(std::vector<HotSwapInterface*> * hotswaps, std::vector<PowerMonitorInterface*> * powermonitors, std::vector<TempInterface*> * thermalsensors, OneWireManager* onewire);
	~CDH();

	void initialize();
	void handleMode(FSWMode transition);
	void getHealthStatus();
	ACPPacket sendOpcode(uint8_t opcode, std::vector<uint8_t> buffer);
	void handleConfig();
	void updateConfig();
	CDHState getCDHState();
	HealthFileStruct health;

	private:
	std::vector<HotSwapData> collectHotswap();
	std::vector<PowerMonitorData> collectPowermon();
	std::vector<float> collectTherm();

	std::vector<HotSwapInterface*> * HotSwaps;
	std::vector<PowerMonitorInterface*> * PowerMonitors;
	std::vector<TempInterface*> * ThermalSensors;
	OneWireManager * OneWireMan;

	CDHState state; //for the bacon

	Lock lock;
	LogTags tags;


};




#endif /* INCLUDE_SUBSYSTEM_CDH_H_ */
