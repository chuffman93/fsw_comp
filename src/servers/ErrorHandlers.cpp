/*
 * ErrorHandlers.cpp
 *
 *  Created on: Mar 23, 2013
 *      Author: Brian Campuzano
 */

#include "servers/ErrorHandlers.h"
#include "servers/ErrorQueue.h"
#include "core/Dispatcher.h"
#include "core/StdTypes.h"
#include "core/ModeManager.h"
#include "core/ACPPacket.h"

using namespace std;
using namespace AllStar::Servers;
using namespace AllStar::Core;

ACPPacket * ERRServerHandler::Handle(const ACPPacket & packet){
	ACPPacket * quePacket = new ACPPacket(packet);
	//Enqueue the packet
	ErrorQueue * que = dynamic_cast<ErrorQueue *>(Factory::GetInstance(ERROR_QUEUE_SINGLETON));
	
	if(!(que->EnqueueError(quePacket))){
		//Enqueue failed... errorception.
		ACPPacket * ret = new ACPPacket(ERROR_QUEUE_FAILED, false);
		return ret;
	}

	ACPPacket * ret = new ACPPacket(packet.getOpcode(), true);
	return ret;
}
