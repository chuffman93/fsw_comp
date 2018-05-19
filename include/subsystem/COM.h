/*
 * COM.h
 *
 *  Created on: Dec 22, 2017
 *      Author: adam
 */

#ifndef INCLUDE_SUBSYSTEM_COM_H_
#define INCLUDE_SUBSYSTEM_COM_H_

#include "subsystem/SubsystemBase.h"
#include "interfaces/SubPowerInterface.h"
#include "interfaces/ACPInterface.h"
#include "core/FileManager.h"
#include "core/FileSystem.h"
#include "core/Lock.h"

enum COMOpcode {
	OP_COM_MIN = 32,
	OP_SIMPLEX = OP_COM_MIN,
	OP_HALFDUPLEX = 33,
	OP_FULLDUPLEX = 34,
	OP_BEACON = 35,
	OP_CHANGEBAUD = 36, //ROBERT FILL IN THIS ENUM
	OP_TX_OFF = 191,
	OP_COM_MAX = 192,
};


class COM: public SubsystemBase{
public:
	COM(ACPInterface& acp, SubPowerInterface& subPower);
	~COM();

	//Will set up the Gpio lines and the acp devices
	void initialize();
	//Handles any mode transition needs as well as any needs for tasks to be done in a mode.
	void handleMode(FSWMode transition);
	//Handles the capturing and storing of the health and status for a subsystem (Maybe find someway to implement the autocoding stuff?)
	void getHealthStatus();
	void handleConfig();
	void updateConfig();
	ACPPacket sendOpcode(uint8_t opcode, std::vector<uint8_t> buffer);
	bool isSuccess(COMOpcode opSent, ACPPacket retPacket);
	bool isSuccess(SubsystemOpcode opSent, ACPPacket retPacket);

	HealthFileStruct health;
private:
	//Configure the lithium radio
	void launchDaemon();
	//Send the beacon
	void sendBeacon();
	//Need to figure out how the GND Communication stuff will work
	bool resetCOM();
	void changeBaudRate(uint32_t baudRate);
	ACPInterface& acp;
	SubPowerInterface& subPower;
	Lock lock;
	LogTags tags;
	ExternalProcess li;
	ExternalProcess da;
};


#endif /* INCLUDE_SUBSYSTEM_COM_H_ */
