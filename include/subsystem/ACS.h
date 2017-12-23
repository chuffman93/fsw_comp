/*
 * ACS.h
 *
 *  Created on: Dec 22, 2017
 *      Author: adam
 */

#ifndef INCLUDE_SUBSYSTEM_ACS_H_
#define INCLUDE_SUBSYSTEM_ACS_H_

#include "SubsystemBase.h"


class ACS: public SubsystemBase{
public:
	ACS(){}
	~ACS(){}

	//ID for subsystem
	static SubsystemID id = SUBSYSTEM_ID_ACS;
	//Will set up the Gpio lines and the acp devices
	virtual void initialize();
	//Handles any mode transition needs as well as any needs for tasks to be done in a mode.
	virtual void handleMode(mode_t transition);
	//Handles the capturing and storing of the health and status for a subsystem (Maybe find someway to implement the autocoding stuff?)
	virtual void getHealthStatus();
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

};




#endif /* INCLUDE_SUBSYSTEM_ACS_H_ */
