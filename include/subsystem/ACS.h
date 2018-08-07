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
#include "core/Watchdog.h"
#include "util/ByteStream.h"
#include "subsystem/GPS.h"
#include <stdio.h>

#define ACS_CONFIG_SIZE 838

// Opcodes specific to ACS
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

// Serializes the gps data that is sent to ACS
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
	void initialize();
	//Handles any mode transition needs as well as any needs for tasks to be done in a mode.
	void handleMode(FSWMode transition);
	//Handles the capturing and storing of the health and status for a subsystem (Maybe find someway to implement the autocoding stuff?)
	void getHealthStatus();
	void handleConfig();
	void updateConfig();
	ACPPacket sendOpcode(uint8_t opcode, std::vector<uint8_t> buffer);
	bool isSuccess(ACSOpcode opSent, ACPPacket retPacket);
	bool isSuccess(SubsystemOpcode opSent, ACPPacket retPacket);
	bool isACSOn();


	float getTimeSinceLock();
	std::vector<float> getTargetMRP();
	std::vector<float> getActualMRP();
	float getStarMRP();
	std::vector<uint16_t> getXPixel();
	std::vector<uint16_t> getYPixel();
	std::vector<uint16_t> getCatalogID();
	uint8_t getNumStarsFound();

	bool sendGPS(GPSPositionTime gps);

	ScheduleManager * sch;

	HealthFileStruct health;
PRIVATE:
	//Change the current pointing target
	bool pointNadir();
	bool pointCOM();
	bool pointSunSoak();

	//Configure the gains on ACS
	void configureGains();
	bool resetACS();
	ACPInterface& getACPRef();
	void checkACSMode(std::vector<uint8_t> buffer);
	void updateStarMRP(std::vector<uint8_t> buffer);
	void updateTargetMRP(std::vector<uint8_t> buffer);
	void updateActualMRP(std::vector<uint8_t> buffer);
	void updateTimeSinceLock(std::vector<uint8_t> buffer);
	void updateXPixel(std::vector<uint8_t> buffer);
	void updateYPixel(std::vector<uint8_t> buffer);
	void updateCatalogID(std::vector<uint8_t> buffer);
	void updateNumStarsFound(std::vector<uint8_t> buffer);

	bool pointingValid;// this should be removed, not used
	float TimeSinceLock;
	float starMRP;
	std::vector<float> TargetMRP;
	std::vector<float> ActualMRP;
	std::vector<uint16_t> xPixel;
	std::vector<uint16_t> yPixel;
	std::vector<uint16_t> catalogID;
	uint8_t numStarsFound;
	ACPInterface& acp;
	SubPowerInterface& subPower;
	bool isOn; // Flag that signifies ACS is powered on when true
	Lock lock;
	LogTags tags;

};




#endif /* INCLUDE_SUBSYSTEM_ACS_H_ */
