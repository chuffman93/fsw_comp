/*
 * THMHandlers.c
 *
 * Created: 10/1/2013 3:52:15 PM
 *  Author: EPS
 */ 

#include "servers/THMStdTasks.h"
#include "servers/THMHandlers.h"
#include "servers/THMServer.h"
#include "servers/DispatchStdTasks.h"

#include "core/CommandMessage.h"
#include "core/ReturnMessage.h"

using namespace std;
using namespace Phoenix::Core;
using namespace Phoenix::Servers;

uint32 THMDataHandler::enumArray[] = {VAR_TYPE_ENUM_UNSIGNED_INT};

ReturnMessage * THMDataHandler::Handle(const FSWPacket & packet)
{	
	void * outputArray[numParams] = {NULL};
	if(ExtractParameters(packet, enumArray, numParams, outputArray))
	{
		ErrorMessage err(THM_HS_FAILURE);
		ReturnMessage * ret = new ReturnMessage(&err, false);
		return ret;
	}
	
	uint32 busNum = * (uint32 *) outputArray[0];
	
	return (DataCollect(busNum));
}
