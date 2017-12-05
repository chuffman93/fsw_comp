/*
 * ACPManager.h
 *
 *  Created on: Dec 4, 2017
 *      Author: cyborg9
 */

#ifndef ACPMANAGER_H_
#define ACPMANAGER_H_

#include "hal/BusManager.h"
#include "hal/SPIManager.h"
#include "hal/InterruptManager.h"
#include "util/ACPPacket.h"

//! Contains the ids for the corrisponding spi and interrupt devices
struct ACPDevice{
	int spiid;
	int intid;
};

/*!
 * Manages a SPI bus and interrupt lines.
 * Handles transactions with the assosiated subsystems.
 */
class ACPManager: public BusManager<ACPDevice>{
public:
	ACPManager(SPIManager& spiman, InterruptManager& intman);
	~ACPManager();

	void initialize();

	MOCK void attachDevice(int spiid, int intid);

	MOCK bool transaction(ACPPacket& packet, ACPPacket& ret);

private:
	//! Reference to the SPIManager to be used
	SPIManager& spiman;
	//! Reference to the InterruptManager to be used
	InterruptManager& intman;
};



#endif /* ACPMANAGER_H_ */
