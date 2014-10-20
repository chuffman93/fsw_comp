/*
 * ErrorHandlers.cpp
 *
 *  Created on: Mar 23, 2013
 *      Author: Brian Campuzano
 */

#include "servers/ErrorHandlers.h"
#include "servers/ErrorQueue.h"

#include "core/CommandMessage.h"
#include "core/ReturnMessage.h"
#include "core/ErrorMessage.h"
#include "core/Dispatcher.h"
#include "core/StdTypes.h"
#include "core/ModeManager.h"

//#include "boards/backplane/dbg_led.h"

using namespace std;
using namespace Phoenix::Servers;
using namespace Phoenix::Core;

ReturnMessage * ErrorOctopusHandler::Handle(const Packet & packet)
{
	Packet * quePacket = new Packet(packet);
	//Enqueue the packet
	ErrorQueue * que = dynamic_cast<ErrorQueue *>(Factory::GetInstance(ERROR_QUEUE_SINGLETON));
	
	if(!(que->EnqueueError(quePacket)))
	{
		//Enqueue failed... errorception.
		ErrorMessage err(ERROR_QUEUE_FAILED);
		ReturnMessage * eRet = new ReturnMessage(&err, false);
		return eRet;
	}
	ReturnMessage * ret = new ReturnMessage(packet.GetMessagePtr(), true);
	return ret;
}
