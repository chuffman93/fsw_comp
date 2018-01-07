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

#include <stdio.h>

#include <stdio.h>

enum ACSOpcode {
	OP_POINTNADIR = 32,
	OP_POINTCOM = 33,
	OP_POINTSUN = 34,
	OP_CONFIGGPS = 35,
	OP_CONFIGGAINS = 36,
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
private:
	//Change the current pointing target
	void pointNadir();
	void pointCOM();
	void pointSunSoak();
	//Update the GPS information on ACS
	void configGPS();
	//Configure the gains on ACS
	void configureGains();

	bool pointingValid = false;
	ACPInterface& acp;
	SubPowerInterface& subPower;
	Lock lock;
	LogTags tags;

};




#endif /* INCLUDE_SUBSYSTEM_ACS_H_ */
