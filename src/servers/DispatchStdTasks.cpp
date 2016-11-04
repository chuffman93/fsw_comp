/*
 * DispatchStdTasks.cpp
 *
 * Created: 7/26/2013 11:23:00 AM
 *  Author: Admin
 */

#include "servers/DispatchStdTasks.h"

#include "core/Dispatcher.h"
#include "core/Arbitrator.h"
#include "core/Singleton.h"
#include "core/Factory.h"

#include "util/FileHandler.h"
#include "util/Logger.h"
#include <assert.h>

using namespace std;
using namespace AllStar::Core;

namespace AllStar{
namespace Servers{

ACPPacket * DispatchPacket(ACPPacket * query)
{
	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
	logger->Log("DispatchStdTasks: DispatchPacket() Called with ACPPacket", LOGGER_LEVEL_DEBUG);

	//check inputs
	LocationIDType source = query->getSource();
	LocationIDType destination = query->getDestination();

	if(source < HARDWARE_LOCATION_MIN || source >= SERVER_LOCATION_MAX
		|| destination < HARDWARE_LOCATION_MIN || destination >= SERVER_LOCATION_MAX)
	{
		logger->Log("DispatcherStdTasks: DispatchPacket(): invalid src/dest!", LOGGER_LEVEL_ERROR);
		ACPPacket * ret = new ACPPacket(PACKET_FORMAT_FAIL, INVALID_PACKET);
		delete query;
		return ret;
	}

	//grab dispatcher instance
	Dispatcher * dispatcher = dynamic_cast<Dispatcher *> (Factory::GetInstance(DISPATCHER_SINGLETON));

	//grab and increment the packet number
	query->setPacketID(dispatcher->packetNumber++);

	//Dispatch packet, if it fails return DISPATCH_FAILED
	if(!dispatcher->Dispatch(*query))
	{
		logger->Log("DispatchStdTasks: Failed to dispatch packet\n", LOGGER_LEVEL_WARN);
		ACPPacket * ret = new ACPPacket(DISPATCH_FAILED, FAILED_DISPATCH);
		delete query;
		return ret;
	}

	DispatcherStatusEnum stat;
	ACPPacket * response;
	//Wait for return message, if it fails return status response from dispatcher
	logger->Log("DispatchStdTasks: Waiting for return message\n", LOGGER_LEVEL_DEBUG);
	if(DISPATCHER_STATUS_OK != (stat = WaitForDispatchResponse(*query, &response)))
	{
		logger->Log("DispatchStdTasks: Did not receive response\n", LOGGER_LEVEL_WARN);
		ACPPacket * ret = new ACPPacket(DISPATCHER_STATUS_ERR, NO_RESPONSE);
		delete query;
		return ret;
	}

	logger->Log("DispatchStdTasks: Received return message\n", LOGGER_LEVEL_DEBUG);

	assert(response != NULL);
	return response;
}

bool DispatchNoResponse(ACPPacket * query){
	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
	logger->Log("DispatchStdTasks: DispatchNoResponse() Called with ACPPacket", LOGGER_LEVEL_DEBUG);

	//check inputs
	LocationIDType source = query->getSource();
	LocationIDType destination = query->getDestination();

	if(source < HARDWARE_LOCATION_MIN || source >= SERVER_LOCATION_MAX
		|| destination < HARDWARE_LOCATION_MIN || destination >= SERVER_LOCATION_MAX)
	{
		logger->Log("DispatcherStdTasks: DispatchPacket(): invalid src/dest!", LOGGER_LEVEL_ERROR);
		return false;
	}

	//grab dispatcher instance
	Dispatcher * dispatcher = dynamic_cast<Dispatcher *> (Factory::GetInstance(DISPATCHER_SINGLETON));

	//grab and increment the packet number
	query->setPacketID(dispatcher->packetNumber++);

	//Dispatch packet, if it fails return DISPATCH_FAILED
	if(!dispatcher->Dispatch(*query)){
		logger->Log("DispatchStdTasks: Failed to dispatch packet\n", LOGGER_LEVEL_WARN);
		return false;
	}
	return true;
}

bool Listen(LocationIDType serverID){
	Dispatcher * dispatcher = dynamic_cast<Dispatcher *> (Factory::GetInstance(DISPATCHER_SINGLETON));
	MessageHandlerRegistry * registry;
	ACPPacket * packet;
	ACPPacket tmpPacket;

	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
	logger->Log("DispatchStdTasks: Listen() called with serverID: %u", serverID, LOGGER_LEVEL_SUPER_DEBUG);

	tmpPacket.setSource(serverID); // temp packet to check for response

	if (!dispatcher->CheckQueueForMatchingPacket(tmpPacket, packet, dispatcher->CHECK_DEST_SOURCE)){
		logger->Log("   DispatchStdTasks: Listen(): No packets have been sent to this server.", LOGGER_LEVEL_SUPER_DEBUG);
		return false;
	}

	logger->Log("   DispatchStdTasks: Listen() w/ id %u: Found a packet, looking for a handler.", serverID, LOGGER_LEVEL_DEBUG);

	if(NULL == (registry = dispatcher->FindHandler(serverID, packet))){
		return false;
	}

	logger->Log("   DispatchStdTasks: Listen(): Permissions are correct, invoke the handler, and obtain the resulting message.", LOGGER_LEVEL_DEBUG);

	ACPPacket * retPacket = registry->Invoke(*packet);

	// Create a return packet from the handler response and the original query
	ACPPacket * ret = new ACPPacket(packet->getDestination(), packet->getSource(), retPacket->getOpcode(),
			retPacket->getLength(), retPacket->getMessageBuff());
	ret->setPacketID(packet->getPacketID());

	// FIXME: make sure this doesn't cause a memory leak!
	delete retPacket;
	//delete packet;

	// Send the response back to the server that dispatched the original message.
	int numPackets = mq_size(dispatcher->queueHandleRX, dispatcher->queueAttrRX);
	bool sendSuccess = false;
	if(numPackets < DISPATCHER_QUEUE_LENGTH){
		sendSuccess = mq_timed_send(dispatcher->queueNameRX, &ret, MAX_BLOCK_TIME, 0);
	}else{
		logger->Log("DispatchStdTasks: RX Queue full!", LOGGER_LEVEL_FATAL);
		sendSuccess = false;
	}

	return sendSuccess;
}

void PacketProcess(LocationIDType id, AllStar::Core::ACPPacket * retPacket)
{
	FileHandler * fileHandler = dynamic_cast<FileHandler *> (Factory::GetInstance(FILE_HANDLER_SINGLETON));
	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
	logger->Log("DispatchStdTasks: PacketProcess() called", LOGGER_LEVEL_DEBUG);

	// Get packet info
	MessageCodeType retOpcode = retPacket->getOpcode();
	bool success = (retOpcode > 199) ^ (retOpcode < 100);

	// Check if error message
	if(!success)
	{
		logger->Log("DispatchStdTasks: Got error return message", LOGGER_LEVEL_INFO);
		if((retOpcode >= DISPATCHER_ERR_START) && (retOpcode <= DISPATCHER_ERR_END))
		{
			//dispatcher error! Log it
			if(!fileHandler->Log(SYSTEM_CDH, retPacket))
			{
				logger->Log("DispatchStdTasks: Failed to log error!", LOGGER_LEVEL_ERROR);
			}
			delete retPacket;
			return;
		}
		else
		{ //Non dispatcher error message, send it to the octopus!
			Dispatcher * dispatcher = dynamic_cast<Dispatcher *> (Factory::GetInstance(DISPATCHER_SINGLETON));

			//Dispatch packet, if it fails return DISPATCH_FAILED
			if(!dispatcher->Dispatch(*retPacket)) {
				// TODO: handle this
				logger->Log("DispatchStdTasks: Failed to dispatch error to octopus", LOGGER_LEVEL_ERROR);
				delete retPacket;
				return;
			}

			DispatcherStatusEnum stat;
			ACPPacket * responsePacket;
			//Wait for return message, if it fails return status response from dispatcher
			if(DISPATCHER_STATUS_OK != (stat = WaitForDispatchResponse(*retPacket, &responsePacket)))
			{
				logger->Log("DispatchStdTasks: no response from error octopus (NOTE: check ERRServer Handler return)", LOGGER_LEVEL_ERROR);
				ACPPacket * ret = new ACPPacket(DISPATCH_FAILED, false);
				PacketProcess(id, ret);
				delete retPacket;
				return;
			}

			logger->Log("DispatchStdTasks: error octopus now has error", LOGGER_LEVEL_DEBUG);
			delete retPacket;
			return;
		}
	}

	FileHandlerIDEnum handlerID;
	switch(id)
	{
		case SERVER_LOCATION_COM :
			handlerID = SUBSYSTEM_COM;
			break;
		case SERVER_LOCATION_EPS :
			handlerID = SUBSYSTEM_EPS;
			break;
		case SERVER_LOCATION_ACS :
			handlerID = SUBSYSTEM_ACS;
			break;
		case SERVER_LOCATION_PLD :
			handlerID = SUBSYSTEM_PLD;
			break;
		case SERVER_LOCATION_GPS :
			handlerID = SUBSYSTEM_GPS;
			break;
		case SERVER_LOCATION_SCH :
			handlerID = SUBSYSTEM_SCH;
			break;
		case SERVER_LOCATION_CMD :
			handlerID = SUBSYSTEM_CMD;
			break;
		case SERVER_LOCATION_CDH :
			handlerID = SYSTEM_CDH;
			break;
		default:
			logger->Log("DispatchStdTasks: Unknown Server!", LOGGER_LEVEL_ERROR);
			delete retPacket;
			return;
	}

	// Successful return message
	logger->Log("DispatchStdTasks: Got successful return message!", LOGGER_LEVEL_INFO);
	if(!fileHandler->Log(handlerID, retPacket))
	{
		// TODO: write to error log
		logger->Log("DispatchStdTasks: Failed to log message", LOGGER_LEVEL_WARN);
	}
	delete retPacket;
}

DispatcherStatusEnum WaitForDispatchResponse(const ACPPacket & packet, ACPPacket ** retPacketin)
{
	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
	Dispatcher * dispatcher = dynamic_cast<Dispatcher *> (Factory::GetInstance(DISPATCHER_SINGLETON));
	size_t i;
	ACPPacket * retPacket;
	logger->Log("    Dispatcher: WaitForDispatchResponse() called", LOGGER_LEVEL_DEBUG);
	logger->Log("    Dispatcher: Checking queue for matching packet", LOGGER_LEVEL_DEBUG);
	for (i = 0; i < DISPATCHER_MAX_RESPONSE_TRIES; ++i)
	{
		if (dispatcher->CheckQueueForMatchingPacket(packet, retPacket, dispatcher->CHECK_MATCHING_RESPONSE))
		{
			logger->Log("    Dispatcher: WaitForDispatchResponse(): Matching ACPPacket found.", LOGGER_LEVEL_DEBUG);

			*retPacketin = retPacket;
			return DISPATCHER_STATUS_OK;
		}
		usleep(DISPATCHER_WAIT_TIME);
	}

	// At this point, see if the command we sent has been received at least.
	logger->Log("   Dispatch:  No response, see if the packet we sent has been received.", LOGGER_LEVEL_DEBUG);
	//debug_led_set_led(6, LED_ON);

	if (dispatcher->CheckQueueForMatchingPacket(packet, retPacket, dispatcher->CHECK_SAME))
	{
		logger->Log("   Dispatch: Command not received, removed from queue", LOGGER_LEVEL_ERROR);
		return DISPATCHER_STATUS_MSG_NOT_RCVD;
	}

	// The command was received, but no response has been placed in
	// the queue, so return that the operation failed.
	logger->Log("   Dispatch: Command received, but no response sent", LOGGER_LEVEL_ERROR);
	return DISPATCHER_STATUS_MSG_RCVD_NO_RESP_SENT;
}

uint32 GetUInt(uint8 * buffer){
	uint32 result = (uint32) buffer[0] << 24 | (uint32) buffer[1] << 16 | (uint32) buffer[2] << 8 | (uint32) buffer[3];
	return result;
}

uint16 GetUInt16(uint8 * buffer){
	uint16 result = (uint16) buffer[0] << 8 | (uint16) buffer[1];
	return result;
}

uint8 GetUInt8(uint8 * buffer){
	uint8 result = buffer[0];
	return result;
}

int32 GetInt(uint8 * buffer){
	int32 result = (int32) buffer[0] << 24 | (int32) buffer[1] << 16 | (int32) buffer[2] << 8 | (int32) buffer[3];
	return result;
}

bool GetBool(uint8 * buffer){
	return((bool) (buffer[0]));
}

float GetFloat(uint8 * buffer){
	float result;
	uint32 temp = (uint32) buffer[0] << 24 | (uint32) buffer[1] << 16 | (uint32) buffer[2] << 8 | (uint32) buffer[3];
	memcpy(&result, &temp, 4);
	return result;
}

double GetDouble(uint8 * buffer){
	double result;
	uint64 temp = (uint64) buffer[0] << 56 | (uint64) buffer[1] << 48 | (uint64) buffer[2] << 40 | (uint64) buffer[3] << 32 |
			(uint64) buffer[4] << 24 | (uint64) buffer[5] << 16 | (uint64) buffer[6] << 8 | (uint64) buffer[7];
	memcpy(&result, &temp, 8);
	return result;
}

void AddUInt16(uint8 * buffer, uint16 data){
	buffer[0] = ((data & 0xFF00) >> 8);
	buffer[1] = data & 0x00FF;
}

void AddUInt32(uint8 * buffer, uint32 data){
	buffer[0] = (data & 0xFF000000) >> 24;
	buffer[1] = (data & 0x00FF0000) >> 16;
	buffer[2] = (data & 0x0000FF00) >> 8;
	buffer[3] = (data & 0x000000FF);
}

void AddFloat(uint8 * buffer, float data){
	uint32 temp;
	memcpy(&temp, &data, sizeof(float));
	buffer[0] = (temp & 0xFF000000) >> 24;
	buffer[1] = (temp & 0x00FF0000) >> 16;
	buffer[2] = (temp & 0x0000FF00) >> 8;
	buffer[3] = (temp & 0x000000FF);
}

void AddDouble(uint8 * buffer, double data){
	uint64 temp;
	memcpy(&temp, &data, sizeof(double));
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
