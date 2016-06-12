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
#include "core/FSWPacket.h"

#include "util/Logger.h"

using namespace std;
using namespace Phoenix::Core;
using namespace Phoenix::Servers;

uint32 PLDDataHandler::enumArray[] = {VAR_TYPE_ENUM_UNSIGNED_INT};
uint32 PLDResSetHandler::enumArray[] = {VAR_TYPE_ENUM_UNSIGNED_INT, VAR_TYPE_ENUM_UNSIGNED_INT};
uint32 PLDSetChunkSizeHandler::enumArray[] = {VAR_TYPE_ENUM_UNSIGNED_INT};
uint32 PLDSetGainHandler::enumArray[] = {VAR_TYPE_ENUM_UNSIGNED_INT};
uint32 PLDSetExpTimeHandler::enumArray[] = {VAR_TYPE_ENUM_UNSIGNED_INT};

FSWPacket * PLDHSHandler::Handle(const FSWPacket & packet)
{
	return (PLDHealthStatus());
}

FSWPacket * PLDPictureTakeHandler::Handle(const FSWPacket & packet)
{
	return (PLDPicture());
}

FSWPacket * PLDGetPictureHandler::Handle(const FSWPacket & packet)
{
	return (PLDGetPicture());
}


FSWPacket * PLDDataHandler::Handle(const FSWPacket & packet)
{
	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
	logger->Log("PLDHandlers: PLDDataHandler(): Unfinished method!", LOGGER_LEVEL_FATAL);

//	void * outputArray[numParams] = {NULL};
//	if(!ExtractParameters(packet, enumArray, numParams, outputArray))
//	{
//		ErrorMessage err(PLD_DATA_FAILURE);
//		ReturnMessage * ret = new ReturnMessage(&err, false);
//		return ret;
//	}
//
//	uint32 ChunkNum = *(uint32 *) outputArray[0];
//
//	return (PLDDataRequest(ChunkNum));
}

FSWPacket * PLDResSetHandler::Handle(const FSWPacket & packet)
{
	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
	logger->Log("PLDHandlers: PLDResSetHandler(): Unfinished method!", LOGGER_LEVEL_FATAL);
	
//	void * outputArray[numParams] = {NULL};
//	if(!ExtractParameters(packet, enumArray, numParams, outputArray))
//	{
//		ErrorMessage err(PLD_RES_FAILURE);
//		ReturnMessage * ret = new ReturnMessage(&err, false);
//		return ret;
//	}
//
//	uint32 ResX = *(uint32 *) outputArray[0];
//	uint32 ResY = *(uint32 *) outputArray[1];
//
//	return (PLDResSet(ResX, ResY));
}

FSWPacket * PLDSetChunkSizeHandler::Handle(const FSWPacket & packet)
{
	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
	logger->Log("PLDHandlers: PLDSetChunkSizeHandler(): Unfinished method!", LOGGER_LEVEL_FATAL);

//	void * outputArray[numParams] = {NULL};
//	if(!ExtractParameters(packet, enumArray, numParams, outputArray))
//	{
//		ErrorMessage err(PLD_CHUNK_FAILURE);
//		ReturnMessage * ret = new ReturnMessage(&err, false);
//		return ret;
//	}
//
//	uint32 ChunkSize = *(uint32 *) outputArray[0];
//
//	return (PLDConfigChunkSize(ChunkSize));
}



FSWPacket * PLDSetGainHandler::Handle(const FSWPacket & packet)
{
	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
	logger->Log("PLDHandlers: PLDSetGainHandler(): Unfinished method!", LOGGER_LEVEL_FATAL);

//	void * outputArray[numParams] = {NULL};
//	if(!ExtractParameters(packet, enumArray, numParams, outputArray))
//	{
//		ErrorMessage err(PLD_GAIN_FAILURE);
//		ReturnMessage * ret = new ReturnMessage(&err, false);
//		return ret;
//	}
//
//	uint32 gain = *(uint32 *) outputArray[0];
//
//	return (PLDSetGain(gain));
}



FSWPacket * PLDSetExpTimeHandler::Handle(const FSWPacket & packet)
{	
	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
	logger->Log("PLDHandlers: PLDSetExpTimeHandler(): Unfinished method!", LOGGER_LEVEL_FATAL);

//	void * outputArray[numParams] = {NULL};
//	if(!ExtractParameters(packet, enumArray, numParams, outputArray))
//	{
//		ErrorMessage err(PLD_EXPTIME_FAILURE);
//		ReturnMessage * ret = new ReturnMessage(&err, false);
//		return ret;
//	}
//
//	uint32 time = *(uint32 *) outputArray[0];
//
//	return (PLDSetExpTime(time));
}


FSWPacket * PLDResetHandler::Handle(const FSWPacket & packet)
{
	return (PLDReset());
}

FSWPacket * PLDErrorHandler::Handle(const FSWPacket & packet)
{
	//grab dispatcher instance, if it fails return DISPATCHER_NO_INSTANCE
	Dispatcher * dispatcher = dynamic_cast<Dispatcher *> (Factory::GetInstance(DISPATCHER_SINGLETON));
	if (NULL == dispatcher)
	{
		FSWPacket * ret = new FSWPacket(0, DISPATCHER_NO_INSTANCE, false, true, MESSAGE_TYPE_ERROR);
		return ret;
	}
	FSWPacket * forward = new FSWPacket(packet);

	//forward error message to Error Octopus
	forward->SetDestination(SERVER_LOCATION_ERR);
	forward->SetSource(SERVER_LOCATION_PLD);

	//Dispatch packet, if it fails return DISPATCH_FAILED
	if(!dispatcher->Dispatch(*forward))
	{
		delete forward;
		FSWPacket * ret = new FSWPacket(0, DISPATCH_FAILED, false, true, MESSAGE_TYPE_ERROR);
		return ret;
	}

	FSWPacket * retPacket;
	DispatcherStatusEnum stat;
	//Wait for return message, if it fails return status response from dispatcher
	if(DISPATCHER_STATUS_OK != (stat = dispatcher->WaitForDispatchResponse(*forward, retPacket)))
	{
		delete forward;
		FSWPacket * ret = new FSWPacket(0, DISPATCHER_STATUS_ERR, false, true, MESSAGE_TYPE_ERROR);
		return ret;
	}

	delete forward;
	//Send server response message back to caller
	//caller responsible for deleting the return message.
	return retPacket;
}

