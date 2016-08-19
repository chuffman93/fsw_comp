/*
 * EPSHandlers.h
 *
 *  Created on: Apr 29, 2011
 *      Author: Michael
 *  Updated On: 10/26/2012
 *  	Author: Gabrielle Massone
 *  Updated:
 *  	Authod: Alex St. Clair
 */

#ifndef EPSHANDLERS_H_
#define EPSHANDLERS_H_

#include "core/MessageHandler.h"

class EPSHSHandler : public AllStar::Core::MessageHandler{
public:
	AllStar::Core::ACPPacket * Handle(const AllStar::Core::ACPPacket & packet);
};

class EPSPowerCycleHandler : public AllStar::Core::MessageHandler{
public:
	AllStar::Core::ACPPacket * Handle(const AllStar::Core::ACPPacket & packet);
};

#endif /* EPSHANDLERS_H_ */

