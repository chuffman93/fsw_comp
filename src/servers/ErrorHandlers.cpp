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
#include "core/FSWPacket.h"

//#include "boards/backplane/dbg_led.h"

using namespace std;
using namespace Phoenix::Servers;
using namespace Phoenix::Core;

FSWPacket * ErrorOctopusHandler::Handle(const FSWPacket & packet)
{
	FSWPacket * quePacket = new FSWPacket(packet);
	//Enqueue the packet
	ErrorQueue * que = dynamic_cast<ErrorQueue *>(Factory::GetInstance(ERROR_QUEUE_SINGLETON));
	
	if(!(que->EnqueueError(quePacket)))
	{
		//Enqueue failed... errorception.
		FSWPacket * ret = new FSWPacket(ERROR_QUEUE_FAILED, false, true, MESSAGE_TYPE_ERROR);
		return ret;
	}
	//ReturnMessage * ret = new ReturnMessage(packet.GetMessagePtr(), true);
	//return ret;
	FSWPacket * ret = new FSWPacket(packet.GetOpcode(), true, true, MESSAGE_TYPE_DATA);
}
