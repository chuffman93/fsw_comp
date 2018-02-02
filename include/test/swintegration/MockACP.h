/*
 * MockACPInterface.h
 *
 *  Created on: Jan 29, 2018
 *      Author: cyborg9
 */

#ifndef MOCKACPINTERFACE_H_
#define MOCKACPINTERFACE_H_

#include "interfaces/ACPInterface.h"

class MockACP: public ACPInterface{
public:
	MockACP(std::string instance);
	~MockACP();

	bool transaction(ACPPacket& packet, ACPPacket& ret);

	void setTimeouts(size_t timeout, size_t tries, size_t delay);
private:
	std::vector<ACPPacket> sentPackets;
	LogTags tags;
};




#endif /* MOCKACPINTERFACE_H_ */
