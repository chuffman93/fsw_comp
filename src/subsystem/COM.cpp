/*
 * COM.cpp
 *
 *  Created on: Dec 22, 2017
 *      Author: alex
 */
#include "subsystem/COM.h"


	COM::COM(ACPInterface& acp, SubPowerInterface& subPower)
	: acp(acp), subPower(subPower)
	{ tags += LogTag("Name", "FileManager"); }

	COM::~COM(){}

	//Will set up the Gpio lines and the acp devices
	void COM::initialize(){
		//TODO: error handling

		ACPPacket acpPacket(COM_SYNC, OP_TESTALIVE);
		ACPPacket acpReturn;
		acp.transaction(acpPacket,acpReturn);

		acpPacket.opcode = OP_TESTLED;
		acp.transaction(acpPacket,acpReturn);

		acpPacket.opcode = OP_TESTCONFIG;
		acp.transaction(acpPacket,acpReturn);
	}

	//Handles any mode transition needs as well as any needs for tasks to be done in a mode.
	void COM::handleMode(FSWMode transition){
		switch (transition){
		case Mode_Com:
			break;
		case Mode_Reset:
			break;
		case Trans_ComToBus:
			break;
		default:
			//TODO: error handling
			break;
		}
	}

	//Handles the capturing and storing of the health and status for a subsystem (Maybe find someway to implement the autocoding stuff?)
	void COM::getHealthStatus(){
		LockGuard l(lock);
		ACPPacket acpPacket(COM_SYNC, OP_HEALTHSTATUS);
		ACPPacket acpReturn;
		acp.transaction(acpPacket,acpReturn);

		std::string folderLocation = HEALTH_DIRECTORY COM_PATH;
		FileManager fm;
		std::string healthFile = fm.createFileName(HEALTH_DIRECTORY COM_PATH);
		fm.writeToFile(healthFile,acpReturn.message);

	}

	//Configure the lithium radio
	void COM::configureLithium(){
			//set baud rate command
	}

	//Send the beacon
	void COM::sendBeacon(){

	}

	//Need to figure out how the GND Communication stuff will work


