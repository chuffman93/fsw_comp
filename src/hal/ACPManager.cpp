/*
 * ACPManager.cpp
 *
 *  Created on: Dec 5, 2017
 *      Author: cyborg9
 */
#include "hal/ACPManager.h"

using namespace std;

/*! Initializes the ACP Manager with the provided SPIManager and Interrupt Manager
 * \param spiman the spimanager reference
 * \param intman the interruptmanager reference
 */
ACPManager::ACPManager(SPIManager& spiman, InterruptManager& intman)
:spiman(spiman), intman(intman), numbertransactions(0)
{

}

//! Does nothing
ACPManager::~ACPManager(){}

//! Does nothing
void ACPManager::initialize(){}

/*! Creates a device to refer the the specified spi and interrupt ids
 * \param spiid the id of the spi device to use
 * \param intid the id of the int device to use
 * \return the value of the attached spi device
 */
int ACPManager::attachDevice(int spiid, int intid){
	ACPDevice dev;
	dev.spiid = spiid;
	dev.intid = intid;
	return BusManager<ACPDevice>::attachDevice(dev);
}

/*! Handles a complete ACP Transaction
 * \param id id of the acp device
 * \param packet the packet to send
 * \param ret the response from the subsystem
 * \return whether the transaction was successful or not
 */
bool ACPManager::transaction(int id, ACPPacket& packet, ACPPacket& ret){
	Monitor monitor(&this->lock);

	bool success = false;
	packet.id = numbertransactions++;
	ret.sync = packet.sync;
	//TODO: Consider how to best handle timeouts and number of times to retry
	for(int i = 0; i < 10; i++){
		if(!sendPacket(id, packet)) continue;
		if(!receivePacket(id, ret)) continue;
		else{
			success = true;
			break;
		}
	}
	return success;
}

/*! Handles sending an ACP Packet
 * \param id id of the acp device
 * \param packet the packet to send
 * \return whether the packet was sent successfully
 */
bool ACPManager::sendPacket(int id, ACPPacket& packet){
	//TODO: figure out what timeout to use in here
	ACPDevice dev = getDevice(id);
	vector<uint8_t> data = packet.pack();
	for(vector<uint8_t>::iterator i = data.begin(); i < data.end(); i++){
		spiman.sendbyte(dev.spiid, *i);
		if(!intman.wait(dev.intid, 100)){
			return false;
		}
	}
	return true;
}

/*! Handles receiving an ACP Packet
 * \param id id of the acp device
 * \param ret the packet to fill with the response
 * \return whether packet was successfully received
 */

bool ACPManager::receivePacket(int id, ACPPacket& ret){
	//TODO: figure out what timeout to use in here
	ACPDevice dev = getDevice(id);
	ret.id = 0;
	ret.opcode = 0;
	ret.crc = 0;
	uint16_t len = 0;

	//Wait for sync
	if(spiman.receivebyte(dev.spiid) != ret.sync){
		return false;
	}
	//Read header
	if(!intman.wait(dev.intid, 100)) return false;
	ret.id |= (uint16_t)spiman.receivebyte(dev.spiid) << 8;
	if(!intman.wait(dev.intid, 100)) return false;
	ret.id |= (uint16_t)spiman.receivebyte(dev.spiid);
	if(!intman.wait(dev.intid, 100)) return false;
	ret.opcode |= (uint16_t)spiman.receivebyte(dev.spiid);
	if(!intman.wait(dev.intid, 100)) return false;
	len |= (uint16_t)spiman.receivebyte(dev.spiid) << 8;
	if(!intman.wait(dev.intid, 100)) return false;
	len |= (uint16_t)spiman.receivebyte(dev.spiid);

	//Read message
	ret.message.clear();
	for(size_t i = 0; i < len; i++){
		if(!intman.wait(dev.intid, 100)) return false;
		ret.message.push_back((uint16_t)spiman.receivebyte(dev.spiid));
	}

	//Read crc
	if(!intman.wait(dev.intid, 100)) return false;
	ret.crc |= (uint16_t)spiman.receivebyte(dev.spiid) << 8;
	if(!intman.wait(dev.intid, 100)) return false;
	ret.crc |= (uint16_t)spiman.receivebyte(dev.spiid);

	//Check that the packet is valid
	return ret.validate();
}



