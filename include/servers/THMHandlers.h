/*
 * THMHandlers.h
 *
 * Created: 10/1/2013 3:36:24 PM
 *  Author: EPS
 */ 


#ifndef THMHANDLERS_H_
#define THMHANDLERS_H_

#include "core/MessageHandler.h"


class THMDataHandler : public Phoenix::Core::MessageHandler
{
public:
		/* \breif Handle a data request.
		 *
		 *	\param packet FSWPacket containing the command.
		 *	\return ReturnMessage from THM hardware.
		 */	
		Phoenix::Core::ReturnMessage * Handle(const Phoenix::Core::FSWPacket & packet);
private:
			const static uint32 numParams = 1;
			static uint32 enumArray[numParams];
};




#endif /* THMHANDLERS_H_ */
