/*
 * CDH.h
 *
 *  Created on: Dec 22, 2017
 *      Author: adam
 */

#ifndef INCLUDE_SUBSYSTEM_CDH_H_
#define INCLUDE_SUBSYSTEM_CDH_H_

#include "SubsystemBase.h"
//#include <vector>


class CDH: public SubsystemBase{
public:
	CDH(){}
	~CDH(){}

	//ID for subsystem
	static SubsystemID id  = SUBSYSTEM_ID_CDH;
	//Will set up the Gpio lines and the acp devices
	virtual void initialize();
	//Handles any mode transition needs as well as any needs for tasks to be done in a mode.
	virtual void handleMode(mode_t transition);
	//Handles the capturing and storing of the health and status for a subsystem (Maybe find someway to implement the autocoding stuff?)
	virtual void getHealthStatus();
private:
	//Various methods for data collection
	void collectHotswap();
	void collectPowermon();
	void collectTherm();

	//std::vector<> Hotswaps;
	//std::vector<>  powermons
	//std::vector<>  thermalsensors



};




#endif /* INCLUDE_SUBSYSTEM_CDH_H_ */
