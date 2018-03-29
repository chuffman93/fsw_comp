/*
 * ACPPacket.h
 *
 *  Created on: Dec 4, 2017
 *      Author: cyborg9
 */

#ifndef ACPPACKET_H_
#define ACPPACKET_H_

#include "util/crc.h"

#include <stdint.h>
#include <vector>
#include <memory>
#include <string>
#include <iostream>

//TODO: move into ACP Interface

//! The sync codes to be used when communicating with subsystem
enum subsystem_sync_t{
	EPS_SYNC = 0xA4 | 0,
	COM_SYNC = 0xA4 | 1,
	ACS_SYNC = 0xA4 | 2,
	RAD_SYNC = 0xA4 | 3,
};

enum SubsystemOpcode{
	OP_MIN = 0,
	OP_TESTLED = OP_MIN,
	OP_TESTCONFIG = 1,
	OP_TESTDATA = 2,
	OP_TESTALIVE = 8,
	OP_HEALTHSTATUS = 16,
	OP_SUBSYSTEMRESET = 17,
	OP_MAX = 18,


};

typedef enum ErrorOpcode {
	ERR_MIN = 0,

	// ACS Errors
	ERR_ACS_MIN = ERR_MIN, //0
	ERR_ACS_NOTALIVE = ERR_ACS_MIN, //0
	ERR_ACS_SELFCHECK, //1
	ERR_ACS_MAX, //2

	// CDH Errors
	ERR_CDH_MIN = ERR_ACS_MAX, //2
	ERR_CDH_MAX, //3

	// CMD Errors
	ERR_CMD_MIN = ERR_CDH_MAX, //3
	ERR_CMD_MAX,//4

	// COM Errors
	ERR_COM_MIN = ERR_CMD_MAX, //4
	ERR_COM_NOTALIVE = ERR_COM_MIN, //4
	ERR_COM_SELFCHECK, //5
	ERR_COM_MAX,

	// EPS Errors
	ERR_EPS_MIN = ERR_COM_MAX,
	ERR_EPS_NOTALIVE = ERR_EPS_MIN,
	ERR_EPS_SELFCHECK,
	ERR_EPS_MAX,

	// FMG Errors
	ERR_FMG_MIN = ERR_EPS_MAX,
	ERR_FMG_MAX,

	// GPS Errors
	ERR_GPS_MIN = ERR_FMG_MAX,
	ERR_GPS_MAX,

	// PLD Errors
	ERR_PLD_MIN = ERR_GPS_MAX,
	ERR_PLD_NOTALIVE = ERR_PLD_MIN,
	ERR_PLD_SELFCHECK,
	ERR_PLD_MAX,

	// SCH Errors
	ERR_SCH_MIN = ERR_PLD_MAX,
	ERR_SCH_MAX,

	// General errors (from the core)
	ERR_GEN_MIN = ERR_SCH_MAX,
	ERR_GEN_MAX,

	ERR_MAX
} ErrorOpcodeType;

/*!
 * A struct to encapsulate the atomic unit of communication with the subsystems.
 */
struct ACPPacket{
	//! Sync code, subsystem dependant
	uint8_t  sync;
	//! Unique id for the transaction
	uint16_t id;
	//! Opcode for the packet
	uint8_t  opcode;
	//! Variable length message
	std::vector<uint8_t> message;
	//! Checksum
	uint16_t crc;

	ACPPacket();
	ACPPacket(subsystem_sync_t sync, uint8_t op);
	ACPPacket(subsystem_sync_t sync, uint8_t op, std::vector<uint8_t> message);

	std::string summary() const;
	std::vector<uint8_t> pack();
	bool validate();

	friend std::ostream &operator<<( std::ostream &output, const ACPPacket &p );

	~ACPPacket();
private:
	std::vector<uint8_t> packallbutcrc();
};



#endif /* ACPPACKET_H_ */
