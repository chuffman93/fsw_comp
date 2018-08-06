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

// Opcodes specific to COM, these are irrelevant and should be deleted
enum COMOpcode {
	OP_COM_MIN = 32,
	OP_SIMPLEX = OP_COM_MIN,
	OP_HALFDUPLEX = 33,
	OP_FULLDUPLEX = 34,
	OP_BEACON = 35,
	OP_CHANGEBAUD = 36, //ROBERT FILL IN THIS ENUM
	OP_TX_ARM = 71,
	OP_TX_KILL = 191,
	OP_COM_MAX = 192,
};


class COM: public SubsystemBase{
public:
	COM(ACPInterface& acp, SubPowerInterface& subPower);
	~COM();

	void initialize();
	void handleMode(FSWMode transition);
	void getHealthStatus();
	void handleConfig();
	void updateConfig();
	void setKillCom(bool toKill);
	bool getKillCom();
	ACPPacket sendOpcode(uint8_t opcode, std::vector<uint8_t> buffer);
	bool isSuccess(COMOpcode opSent, ACPPacket retPacket);
	bool isSuccess(SubsystemOpcode opSent, ACPPacket retPacket);

	HealthFileStruct health;
private:

	bool killCom; //used for during the com daemon off
	void launchDaemon();
	//Send the beacon
	void sendBeacon(); //Should be deleted
	bool resetCOM(); // Should be deleted
	void changeBaudRate(uint32_t baudRate); //Should be deleted
	ACPInterface& acp;
	SubPowerInterface& subPower;
	Lock lock;
	LogTags tags;
	ExternalProcess li;
	ExternalProcess da;
};


#endif /* INCLUDE_SUBSYSTEM_COM_H_ */
