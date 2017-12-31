/*
 * EPS.cpp
 *
 *  Created on: Dec 22, 2017
 *      Author: adam
 */


#include "subsystem/EPS.h"

	EPS::EPS(ACPInterface& acp, SubPowerInterface& subPower)
	: acp(acp), subPower(subPower) {}

	EPS::~EPS(){}

	//Will set up the Gpio lines and the acp devices
	void EPS::initialize(){
		ACPPacket acpPacket(EPS_SYNC, OP_TESTALIVE);
		ACPPacket acpReturn;
		acp.transaction(acpPacket,acpReturn);

		acpPacket.opcode = OP_TESTLED;
		acp.transaction(acpPacket,acpReturn);

		acpPacket.opcode = OP_TESTCONFIG;
		acp.transaction(acpPacket,acpReturn);
	}

	//Handles any mode transition needs as well as any needs for tasks to be done in a mode.
	void EPS::handleMode(FSWMode transition){}

	//Handles the capturing and storing of the health and status for a subsystem (Maybe find someway to implement the autocoding stuff?)
	void EPS::getHealthStatus(){}

	//Power cycle the entire satellite
	void EPS::commandReset(){}



