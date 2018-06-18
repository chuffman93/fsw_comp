/*
 * ACPManager.cpp
 *
 *  Created on: Dec 5, 2017
 *      Author: cyborg9
 */
#include "interfaces/ACPInterface.h"
#include "util/Logger.h"
#include <sstream>
#include <iostream>
#include <iomanip>
#include <unistd.h>
using namespace std;

/*! Initializes the ACP Interface with the provided SPIManager and Interrupt Manager. Sets timeouts to a reasonable default value
 * \param spiman the spimanager reference
 * \param intman the interruptmanager reference
 * \param spiid the id of the spi device to use
 * \param intid the id of the interrupt device to use
 */
ACPInterface::ACPInterface(SPIManager& spiman, GPIOManager& gpioman, int spiid, int intid, std::string instance)
:spiman(spiman), gpioman(gpioman), spiid(spiid), intid(intid), numbertransactions(0), timeout(1500), tries(10), delay(1000)
{
	tags += LogTag("Name", "ACPInterface");
	tags += LogTag("Instance", instance);
}

//! Does nothing
ACPInterface::~ACPInterface(){}

/*! Configures the timeout parameters for the ACP interface
 *  \param timeout the time to wait for an interrupt [ms]
 *  \param tries the number of times to try sending a packet
 *  \param delay the amount of time to wait between tries [ms]
 */
void ACPInterface::setTimeouts(size_t timeout, size_t tries, size_t delay){
	this->timeout = timeout;
	this->tries = tries;
	this->delay = delay;
}


/*! Handles a complete ACP Transaction
 * \param id id of the acp device
 * \param packet the packet to send
 * \param ret the response from the subsystem
 * \return whether the transaction was successful or not
 */
bool ACPInterface::transaction(ACPPacket& packet, ACPPacket& ret){
	bool success = false;
	packet.id = numbertransactions++;
	ret.sync = packet.sync;
	//TODO: Consider how to best handle timeouts and number of times to retry
	for(size_t i = 0; i < tries; i++){
		if(sendPacket(packet)){
			if(receivePacket(ret)){
				Logger::Stream(LEVEL_INFO, tags) << "Received ACP Packet: " << ret;
				success = true;
				break;
			}
		}
		usleep(delay*1000);
	}
	return success;
}

/*! Handles sending an ACP Packet
 * \param id id of the acp device
 * \param packet the packet to send
 * \return whether the packet was sent successfully
 */
bool ACPInterface::sendPacket(ACPPacket& packet){
	Logger::Stream(LEVEL_INFO, tags) << "Sending ACP Packet: " << packet;
	vector<uint8_t> data = packet.pack();
	for(vector<uint8_t>::iterator i = data.begin(); i < data.end(); i++){
		spiman.sendbyte(spiid, *i);
		if(!waitInterrupt()) return false;
	}
	return true;
}

/*! Handles receiving an ACP Packet
 * \param id id of the acp device
 * \param ret the packet to fill with the response
 * \return whether packet was successfully received
 */

bool ACPInterface::receivePacket(ACPPacket& ret){
	ret.id = 0;
	ret.opcode = 0;
	ret.crc = 0;
	uint16_t len = 0;

	uint8_t debug = 0;
	//Wait for sync
	if(!waitInterrupt()) return false;
	bool goodsync = false;
	for(int i = 0; i < 10; i++){
		if((debug = spiman.receivebyte(spiid)) != ret.sync){
			Logger::Stream(LEVEL_WARN, tags) << "Received bad sync: "<< hex <<  setfill('0') << setw(2) << debug;
		}else{
			goodsync = true;
			break;
		}
	}
	if(!goodsync){
		return false;
	}
	//Read header
	if(!waitInterrupt()) return false;
	ret.id |= (uint16_t)spiman.receivebyte(spiid) << 8;
	if(!waitInterrupt()) return false;
	ret.id |= (uint16_t)spiman.receivebyte(spiid);
	if(!waitInterrupt()) return false;
	ret.opcode |= (uint16_t)spiman.receivebyte(spiid);
	if(!waitInterrupt()) return false;
	len |= (uint16_t)spiman.receivebyte(spiid) << 8;
	if(!waitInterrupt()) return false;
	len |= (uint16_t)spiman.receivebyte(spiid);

	//Read message
	ret.message.clear();
	for(size_t i = 0; i < len; i++){
		if(!waitInterrupt()) return false;
		ret.message.push_back((uint16_t)spiman.receivebyte(spiid));
	}

	//Read crc
	if(!waitInterrupt()) return false;
	ret.crc |= (uint16_t)spiman.receivebyte(spiid) << 8;
	if(!waitInterrupt()) return false;
	ret.crc |= (uint16_t)spiman.receivebyte(spiid);


	//Check that the packet is valid
	return ret.validate();
}

/*!
 * Waits to receive an interrupt, handles event raising if failed.
 * \return whether the interrupt timed out or not
 */
bool ACPInterface::waitInterrupt(){
	if(!gpioman.wait(intid, timeout)){
		return false;
	}
	return true;
}



