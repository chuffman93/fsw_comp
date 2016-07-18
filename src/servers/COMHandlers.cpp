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

FSWPacket * COMHSHandler::Handle(const FSWPacket & packet)
{
	return (COMHealthStatus());
}

FSWPacket * COMBeaconHandler::Handle(const FSWPacket & packet)
{
	DEBUG_COUT("		In COMBeaconHandler, calling COMBeacon...")
	//return (COMBeacon());
}

FSWPacket * COMDataSendHandler::Handle(const FSWPacket & packet)
{
//	DEBUG_COUT("		COMDataSendHandler:: handle caled...");
//	void * outputArray[numParams] = {NULL};
//	if(!ExtractParameters(packet, enumArray, numParams, outputArray))
//	{
//		FSWPacket * ret = new FSWPacket(0, COM_DATA_SEND_FAILURE, false, true, MESSAGE_TYPE_ERROR);
//		return ret;
//	}
//	std::vector<uint8> data = *(vector<uint8> *) outputArray[0];
//
//	//return (COMDataSend(data, packet.GetDestination()));
}

FSWPacket * COMLoginHandler::Handle(const FSWPacket & packet)
{	
	DEBUG_COUT("		COMLoginHandler:: logging in...");
	//return (COMLogin(packet.GetDestination()));
}

FSWPacket * COMLogoutHandler::Handle(const FSWPacket & packet)
{
	//return (COMLogout(packet.GetDestination(), packet.GetMessagePtr()->GetOpcode()));
}

FSWPacket * COMResetHandler::Handle(const FSWPacket & packet)
{	
	//return (COMReset());
}

FSWPacket * COMDataReceiveHandler::Handle(const FSWPacket & packet)
{	
//	//DEBUG_COUT("		COMDataReceiveHandler:: handle called...");
//	void * outputArray[numParams] = {NULL};
//	if(!ExtractParameters(packet, enumArray, numParams, outputArray))
//	{
//		DEBUG_COUT("		COMDataReceiveHandler:: failed parameter extraction...");
//		FSWPacket * ret = new FSWPacket(0, COM_DATA_RECEIVE_FAILURE, false, false, MESSAGE_TYPE_ERROR);
//		return ret;
//	}
//
//	std::vector<uint8> data = *(vector<uint8> *) outputArray[0];
//
	//return (COMDataReceive(data, packet.GetDestination()));
}

FSWPacket * COMErrorHandler::Handle(const FSWPacket & packet)
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
		FSWPacket * ret = new FSWPacket(DISPATCH_FAILED, false, true, MESSAGE_TYPE_ERROR);
		delete forward;
		return ret;
	}

	FSWPacket * retMsg;
	DispatcherStatusEnum stat;
	//Wait for return message, if it fails return status response from dispatcher
	if(DISPATCHER_STATUS_OK != (stat = WaitForDispatchResponse(*forward, &retMsg)))
	{
		FSWPacket * ret = new FSWPacket(DISPATCHER_STATUS_ERR, false, true, MESSAGE_TYPE_ERROR);
		delete forward;
		return ret;
	}

	delete forward;
	return retMsg;
}
