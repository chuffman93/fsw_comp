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
#include "core/MessageHandlerRegistry.h"
#include "core/Arbitrator.h"

#define DIS_DEBUG			0

#if DIS_DEBUG && defined(WIN32)
#include <iostream>
#define DEBUG_COUT(m) cout << m << endl;
#else
#define DEBUG_COUT(m)
#endif // DEBUG

namespace AllStar{
namespace Servers{

enum DispatcherStatusEnum {
	/*! \brief Response Receive Successful */
	DISPATCHER_STATUS_OK = 0,
	/*! \brief Dispatched ACPPacket Not Received by Destination*/
	DISPATCHER_STATUS_MSG_NOT_RCVD,
	/*! \brief Dispatched ACPPacket Received But No Response Received */
	DISPATCHER_STATUS_MSG_RCVD_NO_RESP_SENT,
	/*! \brief Maximum Return Value */
	DISPATCHER_STATUS_MAX
};

/*! \brief Typedef for a ACPPacket Comparison Function */
//typedef bool (Core::Dispatcher::* PacketCheckFunctionType)(const Core::ACPPacket & packetIn, const Core::ACPPacket & packetOut) const;

struct DispatcherHandlerType
{
	DispatcherHandlerType(Core::MessageHandlerRegistry * regIn, Core::Arbitrator * arbyIn)
	: registry(regIn), arby(arbyIn) { }

	Core::MessageHandlerRegistry * registry;
	Core::Arbitrator * arby;
};

// Send packet to the dispatcher
AllStar::Core::ACPPacket * DispatchPacket(AllStar::Core::ACPPacket * packet);
bool DispatchNoResponse(AllStar::Core::ACPPacket * packet);

// Invoke message handlers for any messages sent to a server
//bool Listen(LocationIDType serverID);

// Process a response packet
//void PacketProcess(LocationIDType id, AllStar::Core::ACPPacket * retPacket);

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
