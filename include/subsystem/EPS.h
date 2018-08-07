/* EPS.h
 *
 *  Created on: Dec 22, 2017
 *      Author: adam
 */

#ifndef INCLUDE_SUBSYSTEM_EPS_H_
#define INCLUDE_SUBSYSTEM_EPS_H_

#include "subsystem/SubsystemBase.h"
#include "interfaces/SubPowerInterface.h"
#include "interfaces/ACPInterface.h"
#include "core/FileSystem.h"
#include "core/Lock.h"

#include <stdio.h>

// Opcodes specific to EPS
enum EPSOpcode{
	OP_EPS_MIN = 32,
	OP_BATTERYCONFIG = OP_EPS_MIN,
	OP_SLEEP = 33,
	OP_EPS_MAX = 34
};

class EPS: public SubsystemBase{
public:
	EPS(ACPInterface& acp, SubPowerInterface& subPower);
	~EPS();
	SubPowerInterface& subPower;
	//Will set up the Gpio lines and the acp devices
	void initialize();
	//Handles any mode transition needs as well as any needs for tasks to be done in a mode.
	void handleMode(FSWMode transition);
	//Handles the capturing and storing of the health and status for a subsystem (Maybe find someway to implement the autocoding stuff?)
	void getHealthStatus();
	void handleConfig();
	void updateConfig();
	//Get battery capacity
	uint16_t getBatteryStateOfCharge();

	ACPPacket sendOpcode(uint8_t opcode, std::vector<uint8_t> buffer);
	bool isSuccess(EPSOpcode opSent, ACPPacket retPacket);
	bool isSuccess(SubsystemOpcode opSent, ACPPacket retPacket);
	//Power cycle the entire satellite
	bool commandReset();
	HealthFileStruct health;
	//void getSleepTime();
	void sendSleepTime(uint32_t time);


private:
	//Constructor references
	ACPInterface& acp;
	uint16_t batteryCharge;
	Lock lock;
	LogTags tags;
};

#endif /* INCLUDE_SUBSYSTEM_EPS_H_ */
