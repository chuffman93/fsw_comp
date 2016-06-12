/*
 * CMDHandlers.h
 *
 *  Created on: Nov 19, 2015
 *      Author: Alex St. Clair
 */

#include "core/MessageHandler.h"
#include "core/ReturnMessage.h"
#include "core/FSWPacket.h"
#include "core/VariableTypeEnum.h"

class CMDSwitchProtocolHandler : public Phoenix::Core::MessageHandler
{
public:
        /*! \brief Handle a request for a protocol switch
         *
         *  \param packet FSWPacket containing the message.
         *  \return ReturnMessage that serves as a response.
         */
		Phoenix::Core::FSWPacket * Handle(const Phoenix::Core::FSWPacket & packet);
private:
		const static uint32 numParams = 2;
		static uint32 enumArray[numParams];
};
