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


ReturnMessage * GPSHealthAndStatusHandler::Handle(const Packet & packet)
{
	return (GPSHealthStatus());
}

ReturnMessage * GPSTimeHandler::Handle(const Packet & packet)
{
	return (GPSTime());
}

ReturnMessage * GPSPositionHandler::Handle(const Packet & packet)
{
	return (GPSPostion());
}

ReturnMessage * GPSResetHandler::Handle(const Packet & packet)
{
	return (GPSReset());
}

ReturnMessage * GPSErrorHandler::Handle(const Packet & packet)
{
	//grab dispatcher instance, if it fails return DISPATCHER_NO_INSTANCE
	Dispatcher * dispatcher = dynamic_cast<Dispatcher *> (Factory::GetInstance(DISPATCHER_SINGLETON));
	if (NULL == dispatcher)
	{
			ErrorMessage err(DISPATCHER_NO_INSTANCE);
			ReturnMessage * eRet = new ReturnMessage(&err, false);
			return eRet;
	}
	Packet * forward = new Packet(packet);

	//forward error message to Error Octopus
	forward->SetDestination(SERVER_LOCATION_ERR);
	forward->SetSource(SERVER_LOCATION_GPS);

	//Dispatch packet, if it fails return DISPATCH_FAILED
	if(!dispatcher->Dispatch(*forward))
	{
			ErrorMessage err(DISPATCH_FAILED);
			ReturnMessage * eRet = new ReturnMessage(&err, false);
			delete forward;
			return eRet;
	}

	ReturnMessage retMsg;
	DispatcherStatusEnum stat;
	//Wait for return message, if it fails return status response from dispatcher
	if(DISPATCHER_STATUS_OK != (stat = dispatcher->WaitForDispatchResponse(*forward, retMsg)))
	{
			ErrorMessage err(DISPATCHER_STATUS_ERR);
			ReturnMessage * eRet = new ReturnMessage(&err, false);
			delete forward;
			return eRet;
	}

	delete forward;
	//Send server response message back to caller
	ReturnMessage * ret = new ReturnMessage(retMsg);
	//caller responsible for deleting the return message.
	return ret;
}
