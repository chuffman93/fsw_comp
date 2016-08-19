/*
 * COMHandlers.h
 *
 *  Created on: Oct 6, 2012
 *      Author: Brian Campuzano
 *
 */

#ifndef COMHANDLERS_H_
#define COMHANDLERS_H_

#include "core/MessageHandler.h"

class COMHSHandler : public AllStar::Core::MessageHandler{
public:
	AllStar::Core::ACPPacket * Handle(const AllStar::Core::ACPPacket & packet);
};

#endif  //COMHANDLERS_H_
