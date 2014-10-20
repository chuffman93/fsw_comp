/*
 * SCHHandlers.cpp
 *
 * Created: 11/7/2013 11:39:04 AM
 *  Author: cypa8074
 */ 

#include <string>

#include "servers/SCHStdTasks.h"
#include "servers/SCHHandlers.h"
#include "servers/SCHServer.h"
#include "servers/DispatchStdTasks.h"

#include "core/CommandMessage.h"
#include "core/ReturnMessage.h"
#include "core/ErrorMessage.h"
#include "core/Singleton.h"
#include "core/Factory.h"
#include "core/Dispatcher.h"
#include "core/StdTypes.h"

using namespace std;
using namespace Phoenix::Core;
using namespace Phoenix::Servers;

uint32 SCHScheduleHandler::enumArray[] = {VAR_TYPE_ENUM_STRING};
uint32 SCHPldScheduleHandler::enumArray[] = {VAR_TYPE_ENUM_STRING};
uint32 SCHDefaultRangeHandler::enumArray[] = {VAR_TYPE_ENUM_FLOAT};

ReturnMessage * SCHScheduleHandler::Handle(const Packet & packet)
{
	void * outputArray[numParams] = {NULL};
	if(!ExtractParameters(packet, enumArray, numParams, outputArray))
	{
		ErrorMessage err(SCH_BUILD_SCHEDULE_FAILURE);
		ReturnMessage * ret = new ReturnMessage(&err, false);
		return ret;	
	}
	
	string * fileName = (string *) outputArray[0];
	const char * file = fileName->c_str();
	return(BuildSchedule(file));
}

ReturnMessage * SCHPldScheduleHandler::Handle(const Packet & packet)
{
	void * outputArray[numParams] = {NULL};
	if(!ExtractParameters(packet, enumArray, numParams, outputArray))
	{
		ErrorMessage err(SCH_BUILD_PLD_SCHEDULE_FAILURE);
		ReturnMessage * ret = new ReturnMessage(&err, false);
		return ret;
	}
		
	string * fileName = (string *) outputArray[0];
	const char * file = fileName->c_str();
	return(BuildPLDSchedule(file));
}

ReturnMessage * SCHRunScheduleHandler::Handle(const Packet & packet)
{
	return(RunSchedule());
}

ReturnMessage * SCHDefaultRangeHandler::Handle(const Packet & packet)
{
	void * outputArray[numParams] = {NULL};
	if(!ExtractParameters(packet, enumArray, numParams, outputArray))
	{
		ErrorMessage err(SCH_BUILD_PLD_SCHEDULE_FAILURE);
		ReturnMessage * ret = new ReturnMessage(&err, false);
		return ret;
	}
	
	float newRange = * (float *) outputArray[0];
	
	return(SCHDefaultRange(newRange));
}