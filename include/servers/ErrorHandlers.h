/*
 * ErrorHandlers.h
 *
 *  Created on: Mar 23, 2013
 *      Author: Brian Campuzano
 */

#ifndef ERRORHANDLERS_H_
#define ERRORHANDLERS_H_

#include "core/MessageHandler.h"

class ERRServerHandler : public AllStar::Core::MessageHandler{
public:
	AllStar::Core::ACPPacket * Handle(const AllStar::Core::ACPPacket & packet);
};

#endif //ERRORHANDLERS_H_
