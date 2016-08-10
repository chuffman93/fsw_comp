/*
 * ErrorHandlers.h
 *
 *  Created on: Mar 23, 2013
 *      Author: Brian Campuzano
 */

#ifndef ERRORHANDLERS_H_
#define ERRORHANDLERS_H_

#include "core/MessageHandler.h"

class ErrorOctopusHandler : public AllStar::Core::MessageHandler
{
public:
	/* \brief Handle any error.
	 *
	 * \param packet FSWPacket containing the message
	 * \return ReturnMessage that serves as a response
	 */
	AllStar::Core::FSWPacket * Handle(const AllStar::Core::FSWPacket & packet);
};

#endif //ERRORHANDLERS_H_
