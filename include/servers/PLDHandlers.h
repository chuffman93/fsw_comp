/*
 * PLDHandlers.h
 *
 *  Created on: Mar 26, 2013
 *      Author: Brian Campuzano
 */

#ifndef PLDHANDLERS_H_
#define PLDHANDLERS_H_

#include "core/MessageHandler.h"

class PLDHSHandler : public AllStar::Core::MessageHandler
{
public:
        /* \brief Handle a health and status request.
         *
         *  \param packet: ACPPacket containing the command.
         *  \return: ReturnMessage from PLD hardware.
         */
		AllStar::Core::ACPPacket * Handle(const AllStar::Core::ACPPacket & packet);
};

#endif /* PLDHANDLERS_H_ */
