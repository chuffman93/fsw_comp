/*
 * ACPPacket.cpp
 *
 *  Created on: Dec 5, 2017
 *      Author: cyborg9
 */

#include "util/ACPPacket.h"
#include "util/crc.h"


//TODO: move unpacking into here

//! Doesn't do anything
ACPPacket::ACPPacket()
:sync(0), id(0), opcode(0), crc(0)
{

}

//! Create a packet from the given subsystem and opcode
ACPPacket::ACPPacket(subsystem_sync_t sync, uint16_t op)
:sync(sync), id(0), opcode(op), crc(0)
{

}

//! Create a packet from the given subsystem opcode and message
ACPPacket::ACPPacket(subsystem_sync_t sync, uint16_t op, std::vector<uint8_t> message)
:sync(sync), id(0), opcode(op), message(message), crc(0)
{

}

//! Destructor does nothing
ACPPacket::~ACPPacket(){

}

//! Packs the ACPPacket into a vector
std::vector<uint8_t> ACPPacket::pack(){
	std::vector<uint8_t> data = packallbutcrc();
	crc = crcSlow(&data[0], data.size());
	data.push_back(crc >> 8);
	data.push_back(crc);
	return data;
}

//! Pakcks everything but the crc into a vector
std::vector<uint8_t> ACPPacket::packallbutcrc(){
	std::vector<uint8_t> data;
	data.push_back(sync);
	data.push_back(id >> 8);
	data.push_back(id);
	data.push_back(opcode);
	data.push_back(message.size() >> 8);
	data.push_back(message.size());
	for(std::vector<uint8_t>::iterator i = message.begin(); i < message.end(); i++){
		data.push_back(*i);
	}
	return data;
}

//! Checks that the packet is valid
bool ACPPacket::validate(){
	std::vector<uint8_t> data = packallbutcrc();
	uint16_t checksum = crcSlow(&data[0], data.size());
	return checksum == crc;
}

