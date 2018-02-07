/*
 * MockACP.cpp
 *
 *  Created on: Jan 29, 2018
 *      Author: cyborg9
 */

#include "test/swintegration/MockACP.h"

static SPIManager dummys("", 0, 0);
static GPIOManager dummyg("");

MockACP::MockACP(std::string instance)
:ACPInterface(dummys, dummyg, -1, -1, "")
{
	tags += LogTag("Name", "MockACP");
	tags += LogTag("Instance", instance);
}

MockACP::~MockACP(){}

bool MockACP::transaction(ACPPacket& packet, ACPPacket& ret){
	sentPackets.push_back(packet);
	Logger::Stream(LEVEL_INFO, tags) << "Sending ACP Packet: " << packet;
	ret.id = packet.id;
	ret.opcode = packet.opcode;
	ret.message = std::vector<uint8_t>(255, 0); //Return a zero filled buffer to ensure that there is sufficient data
	Logger::Stream(LEVEL_INFO, tags) << "Received ACP Packet: " << ret;
	return true;
}

void MockACP::setTimeouts(size_t timeout, size_t tries, size_t delay){
	Logger::Stream(LEVEL_INFO, tags) << "Setting timeout: " << timeout << " tries:" << tries << " delay:" << delay;
}





