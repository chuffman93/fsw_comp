/*
 * ACPPacket.h
 *
 *  Created on: Dec 4, 2017
 *      Author: cyborg9
 */

#ifndef ACPPACKET_H_
#define ACPPACKET_H_

#include <stdint.h>
#include <vector>
#include <memory>
#include <string>

//TODO: move into ACP Interface

//! The sync codes to be used when communicating with subsystem
enum subsystem_sync_t{
	EPS = 0xA4 | 0,
	COM = 0xA4 | 1,
	ACS = 0xA4 | 2,
	PLD = 0xA4 | 3,
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
	ACPPacket(subsystem_sync_t sync, uint16_t op);
	ACPPacket(subsystem_sync_t sync, uint16_t op, std::vector<uint8_t> message);

	std::string summary();
	std::vector<uint8_t> pack();
	bool validate();

	~ACPPacket();
private:
	std::vector<uint8_t> packallbutcrc();
};



#endif /* ACPPACKET_H_ */
