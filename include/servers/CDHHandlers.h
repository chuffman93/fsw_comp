/*
 * CDHHandlers.h
 *
 *  Created on: Mar 8, 2016
 *      Author: alex
 */

#ifndef CDHHANDLERS_H_
#define CDHHANDLERS_H_

#include "core/MessageHandler.h"
#include "core/ReturnMessage.h"
#include "core/FSWPacket.h"
#include "core/VariableTypeEnum.h"

/* Message handlers for the CDH server.
 * Anytime a message is sent to CDH it should
 * execute one of these handlers.
 */
class CDHUsageHandler : public Phoenix::Core::MessageHandler
{
public:
        /*! \brief Handle a request for CDH processor usage.
         *
         *  \param packet FSWPacket containing the message.
         *  \return ReturnMessage that serves as a response.
         */
        Phoenix::Core::ReturnMessage * Handle(const Phoenix::Core::FSWPacket & packet);
};



#endif /* CDHHANDLERS_H_ */
