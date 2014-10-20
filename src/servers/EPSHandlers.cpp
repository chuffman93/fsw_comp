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
#include "core/Packet.h"
#include "core/Singleton.h"
#include "core/Factory.h"
#include "core/Dispatcher.h"

//#include "boards/backplane/dbg_led.h"

using namespace Phoenix::Core;
using namespace Phoenix::Servers;

uint32 EPSPowerHandler::enumArray[] = {VAR_TYPE_ENUM_UNSIGNED_INT, VAR_TYPE_ENUM_UNSIGNED_INT, VAR_TYPE_ENUM_BOOL};

ReturnMessage * EPSHSHandler::Handle(const Packet & packet)
{
	return(EPSHealthStat());
}

ReturnMessage * EPSStateofChargeHandler::Handle(const Packet & packet)
{
	return(EPSStateOfCharge());
}

ReturnMessage * EPSPowerCycleHandler::Handle(const Packet & packet)
{
	return(EPSPowerCycle());
}

ReturnMessage * EPSDisableOCHandler::Handle(const Packet & packet)
{
	return(EPSDisableOCProt());
}

ReturnMessage * EPSEnableOCHandler::Handle(const Packet & packet)
{
	return(EPSEnableOCProt());
}

ReturnMessage * EPSErrorHandler::Handle(const Packet & packet)
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
	forward->SetSource(SERVER_LOCATION_EPS);

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

ReturnMessage * EPSPowerHandler::Handle(const Packet & packet)
{
	void * outputArray[numParams] = {NULL};
	if(ExtractParameters(packet, enumArray, numParams, outputArray))
	{
		ErrorMessage err(EPS_POWER_SUB_FAILURE);
		ReturnMessage * ret = new ReturnMessage(&err, false);
		return ret;
	}
	
	uint32 time = *(uint32 *) outputArray[0];
	uint32 subsystem_hold = *(uint32 *) outputArray[1];
	bool onOff = *(bool *) outputArray[2];
	PowerSubsystemEnum subsystem = static_cast<PowerSubsystemEnum>(subsystem_hold);
	
	return(EPSPowerSubsystems(subsystem, onOff, time));
}

