/*
 * CDHHandlers.h
 *
 *  Created on: Mar 8, 2016
 *      Author: Alex St. Clair
 */

#ifndef CDHHANDLERS_H_
#define CDHHANDLERS_H_

#include "core/MessageHandler.h"
#include "core/ACPPacket.h"

class CDHCPUUsageHandler : public AllStar::Core::MessageHandler{
public:
	AllStar::Core::ACPPacket * Handle(const AllStar::Core::ACPPacket & packet);
};

class CDHMemUsageHandler : public AllStar::Core::MessageHandler{
public:
	AllStar::Core::ACPPacket * Handle(const AllStar::Core::ACPPacket & packet);
};

class CDHTempStartHandler : public AllStar::Core::MessageHandler{
public:
	AllStar::Core::ACPPacket * Handle(const AllStar::Core::ACPPacket & packet);
};

class CDHTempReadHandler : public AllStar::Core::MessageHandler{
public:
	AllStar::Core::ACPPacket * Handle(const AllStar::Core::ACPPacket & packet);
};

class CDHHotSwapsHandler : public AllStar::Core::MessageHandler{
public:
	AllStar::Core::ACPPacket * Handle(const AllStar::Core::ACPPacket & packet);
};

class CDHPowerMonitorsHandler : public AllStar::Core::MessageHandler{
public:
	AllStar::Core::ACPPacket * Handle(const AllStar::Core::ACPPacket & packet);
};

class CDHStartPMHandler : public AllStar::Core::MessageHandler{
public:
	AllStar::Core::ACPPacket * Handle(const AllStar::Core::ACPPacket & packet);
};

class CDHCleanFSHandler : public AllStar::Core::MessageHandler{
public:
	AllStar::Core::ACPPacket * Handle(const AllStar::Core::ACPPacket & packet);
};

#endif /* CDHHANDLERS_H_ */
