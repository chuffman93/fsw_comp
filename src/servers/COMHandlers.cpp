/*
 * COMHandlers.cpp
 *
 *  Created on: Oct 6, 2012
 *      Author: Brian Campuzano
 *
 */

 
#include "core/CommandMessage.h"
#include "core/ModeManager.h"
#include "core/ReturnMessage.h"
#include "core/ErrorMessage.h"
#include "core/Singleton.h"
#include "core/Factory.h"
#include "core/StdTypes.h"

#include "servers/COMHandlers.h"
#include "servers/COMStdTasks.h"
#include "servers/COMServer.h"
#include "servers/DispatchStdTasks.h"

//#include "boards/backplane/dbg_led.h"

using namespace std;
using namespace Phoenix::Core;
using namespace Phoenix::Servers;

uint32 COMDataSendHandler::enumArray[] = {VAR_TYPE_ENUM_ARRAY};
uint32 COMDataReceiveHandler::enumArray[] = {VAR_TYPE_ENUM_ARRAY};

ReturnMessage * COMHSHandler::Handle(const FSWPacket & packet)
{
	return (COMHealthStatus());
}

ReturnMessage * COMBeaconHandler::Handle(const FSWPacket & packet)
{
	DEBUG_COUT("		In COMBeaconHandler, calling COMBeacon...")
	//return (COMBeacon());
}

ReturnMessage * COMDataSendHandler::Handle(const FSWPacket & packet)
{
	DEBUG_COUT("		COMDataSendHandler:: handle caled...");
	void * outputArray[numParams] = {NULL};
	if(!ExtractParameters(packet, enumArray, numParams, outputArray))
	{
		ErrorMessage err(COM_DATA_SEND_FAILURE);
		ReturnMessage * ret = new ReturnMessage(&err, false);
		return ret;
	}
	std::vector<uint8> data = *(vector<uint8> *) outputArray[0];
	
	//return (COMDataSend(data, packet.GetDestination()));
}

ReturnMessage * COMLoginHandler::Handle(const FSWPacket & packet)
{	
	DEBUG_COUT("		COMLoginHandler:: logging in...");
	//return (COMLogin(packet.GetDestination()));
}

ReturnMessage * COMLogoutHandler::Handle(const FSWPacket & packet)
{
	//return (COMLogout(packet.GetDestination(), packet.GetMessagePtr()->GetOpcode()));
}

ReturnMessage * COMResetHandler::Handle(const FSWPacket & packet)
{	
	//return (COMReset());
}

ReturnMessage * COMDataReceiveHandler::Handle(const FSWPacket & packet)
{	
	//DEBUG_COUT("		COMDataReceiveHandler:: handle called...");
	void * outputArray[numParams] = {NULL};
	if(!ExtractParameters(packet, enumArray, numParams, outputArray))
	{
		DEBUG_COUT("		COMDataReceiveHandler:: failed parameter extraction...");
		ErrorMessage err(COM_DATA_RECEIVE_FAILURE);
		ReturnMessage * ret = new ReturnMessage(&err, false);
		return ret;
	}

	std::vector<uint8> data = *(vector<uint8> *) outputArray[0];
	
	//return (COMDataReceive(data, packet.GetDestination()));
}

ReturnMessage * COMErrorHandler::Handle(const FSWPacket & packet)
{
	//grab dispatcher instance, if it fails return DISPATCHER_NO_INSTANCE
	Dispatcher * dispatcher = dynamic_cast<Dispatcher *> (Factory::GetInstance(DISPATCHER_SINGLETON));
	FSWPacket * forward = new FSWPacket(packet);

	//forward error message to Error Octopus
	forward->SetDestination(SERVER_LOCATION_ERR);
	forward->SetSource(SERVER_LOCATION_COM);

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
