/*
 * CMDHandlers.h
 *
 *  Created on: Nov 19, 2015
 *      Author: Alex St. Clair
 */

#include "core/MessageHandler.h"
#include "core/ACPPacket.h"

class CMDSwitchProtocolHandler : public AllStar::Core::MessageHandler{
public:
	AllStar::Core::ACPPacket * Handle(const AllStar::Core::ACPPacket & packet);
private:
	const static uint32 numParams = 2;
	static uint32 enumArray[numParams];
};
