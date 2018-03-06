/*
 * ACS.h
 *
 *  Created on: Dec 22, 2017
 *      Author: adam
 */

#ifndef INCLUDE_SUBSYSTEM_ACS_H_
#define INCLUDE_SUBSYSTEM_ACS_H_

#include "core/FileManager.h"
#include "subsystem/SubsystemBase.h"
#include "interfaces/SubPowerInterface.h"
#include "interfaces/ACPInterface.h"
#include "core/FileSystem.h"
#include "core/Lock.h"
#include "util/ByteStream.h"
#include "subsystem/GPS.h"

#include <stdio.h>


enum ACSOpcode {
	OP_ACS_MIN = 32,
	OP_POINTNADIR = OP_ACS_MIN,
	OP_POINTDST = 33,
	OP_POINTSUN = 34,
	OP_POINTCOM = 35,
	OP_DET = 36,
	OP_SENDGPS = 37,
	OP_CONFIGGAINS = 38,
	OP_ACS_MAX,
};

struct SerializeGPS {
	SerializeGPS(GPSPositionTime gps):gps(gps){}
	GPSPositionTime gps;
	std::vector<uint8_t> serialize(){
		ByteStream bs;
		bs << gps.posX << gps.posY << gps.posZ << gps.velX << gps.velY << gps.velZ << gps.GPSWeek << gps.GPSSec << gps.isAccurate;
		return bs.vec();
	}
};

class ACS: public SubsystemBase{
public:
	ACS(ACPInterface& acp, SubPowerInterface& subPower);
	~ACS();

	//Will set up the Gpio lines and the acp devices
	bool initialize();
	//Handles any mode transition needs as well as any needs for tasks to be done in a mode.
	void handleMode(FSWMode transition);
	//Handles the capturing and storing of the health and status for a subsystem (Maybe find someway to implement the autocoding stuff?)
	void getHealthStatus();

	ACPPacket sendOpcode(uint8_t opcode, std::vector<uint8_t> buffer);
	bool isSuccess(ACSOpcode opSent, ACPPacket retPacket);
	bool isSuccess(SubsystemOpcode opSent, ACPPacket retPacket);

	float getTimeSinceLock();
	std::vector<float> getTargetMRP();
	std::vector<float> getActualMRP();
	bool sendGPS(GPSPositionTime gps);
	HealthFileStruct health;
private:
	//Change the current pointing target
	bool pointNadir();
	bool pointCOM();
	bool pointSunSoak();
	//Update the GPS information on ACS

	//Configure the gains on ACS
	void configureGains();
	bool resetACS();
	ACPInterface& getACPRef();
	void updateTimeSinceLock(std::vector<uint8_t> buffer);
	void updateTargetMRP(std::vector<uint8_t> buffer);
	void updateActualMRP(std::vector<uint8_t> buffer);

	bool pointingValid;// = false;
	float TimeSinceLock;
	std::vector<float> TargetMRP;
	std::vector<float> ActualMRP;
	ACPInterface& acp;
	SubPowerInterface& subPower;
	Lock lock;
	LogTags tags;

};




#endif /* INCLUDE_SUBSYSTEM_ACS_H_ */
