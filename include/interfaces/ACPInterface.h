/*
 * ACPManager.h
 *
 *  Created on: Dec 4, 2017
 *      Author: cyborg9
 */

#ifndef ACPINTERFACE_H_
#define ACPINTERFACE_H_

#include "hal/SPIManager.h"
#include "hal/GPIOManager.h"
#include "util/ACPPacket.h"
#include "util/Logger.h"

/*!
 * Manages a SPI bus and interrupt lines.
 * Handles transactions with the assosiated subsystems.
 */
class ACPInterface{
public:
	ACPInterface(SPIManager& spiman, GPIOManager& intman, int spiid, int intid, std::string instance);
	MOCK ~ACPInterface();

	MOCK bool transaction(ACPPacket& packet, ACPPacket& ret);

	MOCK void setTimeouts(size_t timeout, size_t tries, size_t delay);

private:
	//! Reference to the SPIManager to be used
	SPIManager& spiman;
	//! Reference to the InterruptManager to be used
	GPIOManager& gpioman;

	//! The spi device to use
	int spiid;
	//! The interrupt device to use
	int intid;

	//! Stores the current number of packets sent
	uint16_t numbertransactions;

	//! The amount of time to wait for an interrupt [ms]
	size_t timeout;
	//! The number of times to try sending a packet
	size_t tries;
	//! The amount of time to wait between packet transactions [ms]
	size_t delay;

	//! Tags to be used when logging
	LogTags tags;

	bool sendPacket(ACPPacket& packet);
	bool receivePacket(ACPPacket& ret);
	bool waitInterrupt();
};



#endif /* ACPINTERFACE_H_ */
