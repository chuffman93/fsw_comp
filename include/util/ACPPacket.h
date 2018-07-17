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
	OP_MAX = 18


};

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
