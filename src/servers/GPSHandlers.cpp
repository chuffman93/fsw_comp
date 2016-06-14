/*
 * GPSHandlers.cpp
 *
 *  Created: 7/3/12
 *  Written by: Conrad Hougen
 */
#include "servers/GPSHandlers.h"
#include "servers/GPSStdTasks.h"

#include "core/ReturnMessage.h"
#include "core/Singleton.h"
#include "core/Factory.h"
#include "core/StdTypes.h"
#include "core/DataMessage.h"
#include "core/Uint32Datatype.h"
#include "core/Dispatcher.h"

//#include "boards/backplane/dbg_led.h"

using namespace std;
//using namespace Phoenix::HAL;
using namespace Phoenix::Servers;
using namespace Phoenix::Core;


FSWPacket * GPSHealthAndStatusHandler::Handle(const FSWPacket & packet)
{
	return (GPSHealthStatus());
}

FSWPacket * GPSTimeHandler::Handle(const FSWPacket & packet)
{
	return (GPSTime());
}

FSWPacket * GPSPositionHandler::Handle(const FSWPacket & packet)
{
	return (GPSPostion());
}

FSWPacket * GPSResetHandler::Handle(const FSWPacket & packet)
{
	return (GPSReset());
}

FSWPacket * GPSErrorHandler::Handle(const FSWPacket & packet)
{
	//grab dispatcher instance, if it fails return DISPATCHER_NO_INSTANCE
	Dispatcher * dispatcher = dynamic_cast<Dispatcher *> (Factory::GetInstance(DISPATCHER_SINGLETON));
	if (NULL == dispatcher)
	{
		FSWPacket * ret = new FSWPacket(0, DISPATCHER_NO_INSTANCE, false, false, MESSAGE_TYPE_ERROR);
		return ret;
	}
	FSWPacket * forward = new FSWPacket(packet);

	//forward error message to Error Octopus
	forward->SetDestination(SERVER_LOCATION_ERR);
	forward->SetSource(SERVER_LOCATION_GPS);

	//Dispatch packet, if it fails return DISPATCH_FAILED
	if(!dispatcher->Dispatch(*forward))
	{
		delete forward;
		FSWPacket * ret = new FSWPacket(0, DISPATCH_FAILED, false, false, MESSAGE_TYPE_ERROR);
		return ret;
	}

	FSWPacket * retPacket;
	DispatcherStatusEnum stat;
	//Wait for return message, if it fails return status response from dispatcher
	if(DISPATCHER_STATUS_OK != (stat = dispatcher->WaitForDispatchResponse(*forward, &retPacket)))
	{
		delete forward;
		FSWPacket * ret = new FSWPacket(0, DISPATCHER_STATUS_ERR, false, false, MESSAGE_TYPE_ERROR);
		return ret;
	}

	delete forward;
	//Send server response message back to caller
	//caller responsible for deleting the return message.
	return retPacket;
}
