/*
 * ErrorHandlers.h
 *
 *  Created on: Mar 23, 2013
 *      Author: Brian Campuzano
 */

#ifndef ERRORHANDLERS_H_
#define ERRORHANDLERS_H_

#include "core/MessageHandler.h"

class ErrorOctopusHandler : public Phoenix::Core::MessageHandler
{
public:
	/* \brief Handle any error.
	 *
	 * \param packet FSWPacket containing the message
	 * \return ReturnMessage that serves as a response
	 */
	Phoenix::Core::ReturnMessage * Handle(const Phoenix::Core::FSWPacket & packet);
};

#endif //ERRORHANDLERS_H_
