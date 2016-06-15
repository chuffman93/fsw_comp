/*
 * EPSHandlers.cpp
 *
 *  Created on: Apr 29, 2011
 *      Author: Michael
 *  Updated on: 10/26/2012
 *  Author: Gabrielle Massone
 */

#include "servers/EPSHandlers.h"
#include "servers/EPSStdTasks.h"
#include "servers/DispatchStdTasks.h"

#include "core/StdTypes.h"
#include "core/ReturnMessage.h"
#include "core/FSWPacket.h"
#include "core/Singleton.h"
#include "core/Factory.h"
#include "core/Dispatcher.h"

//#include "boards/backplane/dbg_led.h"

using namespace Phoenix::Core;
using namespace Phoenix::Servers;

uint32 EPSPowerHandler::enumArray[] = {VAR_TYPE_ENUM_UNSIGNED_INT, VAR_TYPE_ENUM_BOOL, VAR_TYPE_ENUM_UNSIGNED_INT};

FSWPacket * EPSHSHandler::Handle(const FSWPacket & packet)
{
	return(EPSHealthStat());
}

FSWPacket * EPSStateofChargeHandler::Handle(const FSWPacket & packet)
{
	return(EPSStateOfCharge());
}

FSWPacket * EPSPowerCycleHandler::Handle(const FSWPacket & packet)
{
	return(EPSPowerCycle());
}

FSWPacket * EPSDisableOCHandler::Handle(const FSWPacket & packet)
{
	return(EPSDisableOCProt());
}

FSWPacket * EPSEnableOCHandler::Handle(const FSWPacket & packet)
{
	return(EPSEnableOCProt());
}

FSWPacket * EPSErrorHandler::Handle(const FSWPacket & packet)
{
	//grab dispatcher instance, if it fails return DISPATCHER_NO_INSTANCE
	Dispatcher * dispatcher = dynamic_cast<Dispatcher *> (Factory::GetInstance(DISPATCHER_SINGLETON));
	FSWPacket * forward = new FSWPacket(packet);

	//forward error message to Error Octopus
	forward->SetDestination(SERVER_LOCATION_ERR);
	forward->SetSource(SERVER_LOCATION_EPS);

	//Dispatch packet, if it fails return DISPATCH_FAILED
	if(!dispatcher->Dispatch(*forward))
	{
		FSWPacket * ret = new FSWPacket(0, DISPATCH_FAILED, false, false, MESSAGE_TYPE_ERROR);
		return ret;
	}

	FSWPacket * retMsg;
	DispatcherStatusEnum stat;
	//Wait for return message, if it fails return status response from dispatcher
	if(DISPATCHER_STATUS_OK != (stat = dispatcher->WaitForDispatchResponse(*forward, &retMsg)))
	{
			FSWPacket * ret = new FSWPacket(0, DISPATCHER_STATUS_ERR, false, false, MESSAGE_TYPE_ERROR);
			return ret;
	}

	delete forward;
	return retMsg;
}

FSWPacket * EPSPowerHandler::Handle(const FSWPacket & packet)
{
//	void * outputArray[numParams] = {NULL};
//	if(!ExtractParameters(packet, enumArray, numParams, outputArray))
//	{
//		FSWPacket * ret = new FSWPacket(0, EPS_POWER_SUB_FAILURE, false, false, MESSAGE_TYPE_ERROR);
//		return ret;
//	}
//
//	uint32 time = *(uint32 *) outputArray[2];
//	uint32 subsystem_hold = *(uint32 *) outputArray[0];
//	bool onOff = *(bool *) outputArray[1];
//	PowerSubsystemEnum subsystem = static_cast<PowerSubsystemEnum>(subsystem_hold);
//
//	return(EPSPowerSubsystems(subsystem, onOff, time));
}

