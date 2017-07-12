/*
 * DispatchStdTasks.cpp
 *
 * Created: 7/26/2013 11:23:00 AM
 *  Author: Admin
 */

#include "servers/DispatchStdTasks.h"

#include "core/Dispatcher.h"
#include "core/Singleton.h"
#include "core/Factory.h"
#include "util/Logger.h"
#include <assert.h>

using namespace std;
using namespace AllStar::Core;

namespace AllStar{
namespace Servers{

ACPPacket * DispatchPacket(ACPPacket * query) {
	Logger * logger = static_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
	logger->Debug("DispatchStdTasks: DispatchPacket() Called with ACPPacket");

	//check inputs
	LocationIDType source = query->getSource();
	LocationIDType destination = query->getDestination();
	if (source < HARDWARE_LOCATION_MIN || source >= SERVER_LOCATION_MAX || destination < HARDWARE_LOCATION_MIN || destination >= SERVER_LOCATION_MAX) {
		logger->Error("DispatcherStdTasks: DispatchPacket(): invalid src/dest!");
		ACPPacket * ret = new ACPPacket(PACKET_FORMAT_FAIL, INVALID_PACKET);
		delete query;
		return ret;
	}

	Dispatcher * dispatcher = static_cast<Dispatcher *> (Factory::GetInstance(DISPATCHER_SINGLETON));
	query->setPacketID(dispatcher->packetNumber++);

	// Dispatch packet, if it fails return DISPATCH_FAILED
	if(!dispatcher->Dispatch(*query)) {
		logger->Warning("DispatchStdTasks: Failed to dispatch packet\n");
		ACPPacket * ret = new ACPPacket(DISPATCH_FAILED, FAILED_DISPATCH);
		delete query;
		return ret;
	}

	DispatcherStatusEnum stat;
	ACPPacket * response;

	//Wait for return message, if it fails return status response from dispatcher
	logger->Debug("DispatchStdTasks: Waiting for return message\n");
	if (DISPATCHER_STATUS_OK != (stat = WaitForDispatchResponse(*query, &response))) {
		logger->Warning("DispatchStdTasks: Did not receive response\n");
		ACPPacket * ret = new ACPPacket(DISPATCHER_STATUS_ERR, NO_RESPONSE);
		delete query;
		return ret;
	}

	logger->Debug("DispatchStdTasks: Received return message\n");

	assert(response != NULL);
	return response;
}

DispatcherStatusEnum WaitForDispatchResponse(const ACPPacket & packet, ACPPacket ** retPacketin) {
	Logger * logger = static_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
	Dispatcher * dispatcher = static_cast<Dispatcher *> (Factory::GetInstance(DISPATCHER_SINGLETON));
	size_t i;
	ACPPacket * retPacket;

	logger->Debug("    Dispatcher: WaitForDispatchResponse() called");
	logger->Debug("    Dispatcher: Checking queue for matching packet");
	for (i = 0; i < DISPATCHER_MAX_RESPONSE_TRIES; ++i) {
		if (dispatcher->CheckQueueForMatchingPacket(packet, retPacket, dispatcher->CHECK_MATCHING_RESPONSE)) {
			logger->Debug("    Dispatcher: WaitForDispatchResponse(): Matching ACPPacket found.");

			*retPacketin = retPacket;
			return DISPATCHER_STATUS_OK;
		}
		usleep(DISPATCHER_WAIT_TIME);
	}

	// At this point, see if the command we sent has been received at least.
	logger->Debug("   Dispatch:  No response, see if the packet we sent has been received.");
	//debug_led_set_led(6, LED_ON);

	if (dispatcher->CheckQueueForMatchingPacket(packet, retPacket, dispatcher->CHECK_SAME)) {
		logger->Error("   Dispatch: Command not received, removed from queue");
		return DISPATCHER_STATUS_MSG_NOT_RCVD;
	}

	// The command was received, but no response has been placed in
	// the queue, so return that the operation failed.
	logger->Error("   Dispatch: Command received, but no response sent");
	return DISPATCHER_STATUS_MSG_RCVD_NO_RESP_SENT;
}

uint32 GetUInt32(uint8 * buffer) {
	uint32 result = (uint32) buffer[0] << 24 | (uint32) buffer[1] << 16 | (uint32) buffer[2] << 8 | (uint32) buffer[3];
	return result;
}

uint16 GetUInt16(uint8 * buffer) {
	uint16 result = (uint16) buffer[0] << 8 | (uint16) buffer[1];
	return result;
}

uint8 GetUInt8(uint8 * buffer) {
	uint8 result = buffer[0];
	return result;
}

int32 GetInt(uint8 * buffer) {
	int32 result = (int32) buffer[0] << 24 | (int32) buffer[1] << 16 | (int32) buffer[2] << 8 | (int32) buffer[3];
	return result;
}

bool GetBool(uint8 * buffer) {
	return((bool) (buffer[0]));
}

float GetFloat(uint8 * buffer) {
	uint32 temp = (uint32) buffer[0] << 24 | (uint32) buffer[1] << 16 | (uint32) buffer[2] << 8 | (uint32) buffer[3];
	return *((float *) &temp);
}

double GetDouble(uint8 * buffer) {
	uint64 temp = (uint64) buffer[0] << 56 | (uint64) buffer[1] << 48 | (uint64) buffer[2] << 40 | (uint64) buffer[3] << 32 |
			(uint64) buffer[4] << 24 | (uint64) buffer[5] << 16 | (uint64) buffer[6] << 8 | (uint64) buffer[7];
	return *((double *) &temp);
}

void AddUInt16(uint8 * buffer, uint16 data) {
	buffer[0] = ((data & 0xFF00) >> 8);
	buffer[1] = data & 0x00FF;
}

void AddUInt32(uint8 * buffer, uint32 data) {
	buffer[0] = (data & 0xFF000000) >> 24;
	buffer[1] = (data & 0x00FF0000) >> 16;
	buffer[2] = (data & 0x0000FF00) >> 8;
	buffer[3] = (data & 0x000000FF);
}

void AddFloat(uint8 * buffer, float data) {
	uint32 temp = *((uint32 *) &data);
	buffer[0] = (temp & 0xFF000000) >> 24;
	buffer[1] = (temp & 0x00FF0000) >> 16;
	buffer[2] = (temp & 0x0000FF00) >> 8;
	buffer[3] = (temp & 0x000000FF);
}

void AddDouble(uint8 * buffer, double data) {
	uint64 temp = *((uint64 *) &data);
	buffer[0] = (temp & 0xFF00000000000000) >> 56;
	buffer[1] = (temp & 0x00FF000000000000) >> 48;
	buffer[2] = (temp & 0x0000FF0000000000) >> 40;
	buffer[3] = (temp & 0x000000FF00000000) >> 32;
	buffer[4] = (temp & 0x00000000FF000000) >> 24;
	buffer[5] = (temp & 0x0000000000FF0000) >> 16;
	buffer[6] = (temp & 0x000000000000FF00) >> 8;
	buffer[7] = (temp & 0x00000000000000FF);
}

} // End of namespace Servers
} // End of namespace AllStar
