/*
 * ACSHandlers.h
 *
 *  Created on: Apr 29, 2011
 *      Author: Michael
 *
 *  Updated: Alex St. Clair
 */

#ifndef ACSHANDLERS_H_
#define ACSHANDLERS_H_

#include "core/MessageHandler.h"
#include "core/ACPPacket.h"

class ACSMeasurementHandler : public AllStar::Core::MessageHandler{
public:
        AllStar::Core::ACPPacket * Handle(const AllStar::Core::ACPPacket & packet);
};

class ACSGPSHandler : public AllStar::Core::MessageHandler{
public:
        AllStar::Core::ACPPacket * Handle(const AllStar::Core::ACPPacket & packet);
private:
		const static uint32 numParams = 8;
		static uint32 enumArray[numParams];
};

#endif /* ACSHANDLERS_H_ */
