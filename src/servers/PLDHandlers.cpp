/*
 * PLDHandlers.cpp
 *
 *  Created on: Mar 26, 2013
 *      Author: Brian Campuzano
 */

#include "servers/PLDServer.h"
#include "servers/PLDHandlers.h"
#include "servers/PLDStdTasks.h"
#include "servers/DispatchStdTasks.h"

#include "core/CommandMessage.h"
#include "core/ReturnMessage.h"
#include "core/ErrorMessage.h"
#include "core/Singleton.h"
#include "core/Factory.h"
#include "core/StdTypes.h"

using namespace std;
using namespace Phoenix::Core;
using namespace Phoenix::Servers;

uint32 PLDDataHandler::enumArray[] = {VAR_TYPE_ENUM_UNSIGNED_INT};
uint32 PLDResSetHandler::enumArray[] = {VAR_TYPE_ENUM_UNSIGNED_INT, VAR_TYPE_ENUM_UNSIGNED_INT};
uint32 PLDSetChunkSizeHandler::enumArray[] = {VAR_TYPE_ENUM_UNSIGNED_INT};
uint32 PLDSetGainHandler::enumArray[] = {VAR_TYPE_ENUM_UNSIGNED_INT};
uint32 PLDSetExpTimeHandler::enumArray[] = {VAR_TYPE_ENUM_UNSIGNED_INT};

ReturnMessage * PLDHSHandler::Handle(const Packet & packet)
{
	return (PLDHealthStatus());
}

ReturnMessage * PLDPictureTakeHandler::Handle(const Packet & packet)
{
	return (PLDPicture());
}

ReturnMessage * PLDGetPictureHandler::Handle(const Packet & packet)
{
	return (PLDGetPicture());
}


ReturnMessage * PLDDataHandler::Handle(const Packet & packet)
{
	void * outputArray[numParams] = {NULL};
	if(!ExtractParameters(packet, enumArray, numParams, outputArray))
	{
		ErrorMessage err(PLD_DATA_FAILURE);
		ReturnMessage * ret = new ReturnMessage(&err, false);
		return ret;
	}
	
	uint32 ChunkNum = *(uint32 *) outputArray[0];
	
	return (PLDDataRequest(ChunkNum));
}

ReturnMessage * PLDResSetHandler::Handle(const Packet & packet)
{
	void * outputArray[numParams] = {NULL};
	if(!ExtractParameters(packet, enumArray, numParams, outputArray))
	{
		ErrorMessage err(PLD_RES_FAILURE);
		ReturnMessage * ret = new ReturnMessage(&err, false);
		return ret;
	}
	
	uint32 ResX = *(uint32 *) outputArray[0];
	uint32 ResY = *(uint32 *) outputArray[1];

	return (PLDResSet(ResX, ResY));
}

ReturnMessage * PLDSetChunkSizeHandler::Handle(const Packet & packet)
{
	void * outputArray[numParams] = {NULL};
	if(!ExtractParameters(packet, enumArray, numParams, outputArray))
	{
		ErrorMessage err(PLD_CHUNK_FAILURE);
		ReturnMessage * ret = new ReturnMessage(&err, false);
		return ret;
	}
	
	uint32 ChunkSize = *(uint32 *) outputArray[0];
	
	return (PLDConfigChunkSize(ChunkSize));
}



ReturnMessage * PLDSetGainHandler::Handle(const Packet & packet)
{
	void * outputArray[numParams] = {NULL};
	if(!ExtractParameters(packet, enumArray, numParams, outputArray))
	{
		ErrorMessage err(PLD_GAIN_FAILURE);
		ReturnMessage * ret = new ReturnMessage(&err, false);
		return ret;
	}
	
	uint32 gain = *(uint32 *) outputArray[0];
	
	return (PLDSetGain(gain));
}



ReturnMessage * PLDSetExpTimeHandler::Handle(const Packet & packet)
{	
	void * outputArray[numParams] = {NULL};
	if(!ExtractParameters(packet, enumArray, numParams, outputArray))
	{
		ErrorMessage err(PLD_EXPTIME_FAILURE);
		ReturnMessage * ret = new ReturnMessage(&err, false);
		return ret;
	}
	
	uint32 time = *(uint32 *) outputArray[0];
	
	return (PLDSetExpTime(time));
}


ReturnMessage * PLDResetHandler::Handle(const Packet & packet)
{
	return (PLDReset());
}

ReturnMessage * PLDErrorHandler::Handle(const Packet & packet)
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
	forward->SetSource(SERVER_LOCATION_PLD);

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

