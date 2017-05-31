/*
 * DispatchStdTasks.h
 *
 * Created: 7/26/2013 11:22:36 AM
 *  Author: Admin
 */ 


#ifndef DISPATCHSTDTASKS_H_
#define DISPATCHSTDTASKS_H_

#include <list>
#include <map>

#include "core/StdTypes.h"
#include "core/ACPPacket.h"
#include "core/Dispatcher.h"

namespace AllStar{
namespace Servers{

enum DispatcherStatusEnum {
	DISPATCHER_STATUS_OK = 0,
	DISPATCHER_STATUS_MSG_NOT_RCVD,
	DISPATCHER_STATUS_MSG_RCVD_NO_RESP_SENT,
	DISPATCHER_STATUS_MAX
};

// Send packet to the dispatcher
AllStar::Core::ACPPacket * DispatchPacket(AllStar::Core::ACPPacket * packet);

DispatcherStatusEnum WaitForDispatchResponse(const Core::ACPPacket & packet, Core::ACPPacket ** retPacketin);

// Functions for extracting parameters into buffers
uint32 	GetUInt32(uint8 * buffer);
uint16 	GetUInt16(uint8 * buffer);
uint8	GetUInt8(uint8 * buffer);
int32 	GetInt(uint8 * buffer);
bool 	GetBool(uint8 * buffer);
float 	GetFloat(uint8 * buffer);
double 	GetDouble(uint8 * buffer);

// Functions for adding parameters to buffers
void AddUInt16(uint8 * buffer, uint16 data);
void AddUInt32(uint8 * buffer, uint32 data);
void AddFloat(uint8 * buffer, float data);
void AddDouble(uint8 * buffer, double data);

}
}


#endif /* DISPATCHSTDTASKS_H_ */
