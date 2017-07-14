/*! \file ACPPacket.cpp
 *  \brief Header File for the ACPPacket Class
 *
 *  This file contains the implementation of the ACPPacket class, encapsulates a
 *  packet in the AllStar Communication Protocol (ACP).
 */

#include "core/ACPPacket.h"
#include "servers/GPSServer.h"
#include "servers/FMGServer.h"
#include "util/Logger.h"
#include "util/crc.h"
#include "POSIX.h"
#include <stdio.h>
#include <math.h>

using namespace std;

namespace AllStar{
namespace Core{

// --- Constructors/Operators --------------------------------------------------------------------------------------
ACPPacket::ACPPacket(void )
	: source(LOCATION_ID_INVALID), destination(LOCATION_ID_INVALID), fromHardware(false), opcode(0),
	  packetID(0), CRC(0), messageBuff(NULL), length(0), success(true), errorStatus(0), sync(0){
	timestamp = getTimeInMillis();
}

ACPPacket::~ACPPacket(void ){
	free(messageBuff);
}

bool ACPPacket::operator==(const ACPPacket & check) const{
	if(length != check.getLength()){
		return false;
	}

	return ((destination == check.destination) 	&&
			(packetID == check.packetID));
}

// --- Constructors for dispatching --------------------------------------------------------------------------------
ACPPacket::ACPPacket(LocationIDType sourceIn, LocationIDType destIn, uint8 opcodeIn)
	: source(sourceIn), destination(destIn), fromHardware(false), opcode(opcodeIn), packetID(0),
	  CRC(0), messageBuff(NULL), length(0), errorStatus(0), success(true), sync(0){
	timestamp = getTimeInMillis();
}

ACPPacket::ACPPacket(LocationIDType sourceIn, LocationIDType destIn, uint8 opcodeIn, uint16 lengthIn, uint8 * messageIn)
	: source(sourceIn), destination(destIn), fromHardware(false), opcode(opcodeIn), packetID(0),
	  CRC(0), messageBuff(messageIn), length(lengthIn), errorStatus(0), success(true), sync(0){
	timestamp = getTimeInMillis();
}

// --- Constructors for return messages ----------------------------------------------------------------------------
ACPPacket::ACPPacket(uint8 opcodeIn, ErrorStatusType errorStatusIn)
	: source(LOCATION_ID_INVALID), destination(LOCATION_ID_INVALID), fromHardware(false), opcode(opcodeIn),
	  packetID(0), CRC(0), messageBuff(NULL), length(0), errorStatus(errorStatusIn), success(errorStatusIn == 0), sync(0){
	timestamp = getTimeInMillis();
}

ACPPacket::ACPPacket(uint8 opcodeIn, ErrorStatusType errorStatusIn, uint16 lengthIn, uint8 * messageIn)
	: source(LOCATION_ID_INVALID), destination(LOCATION_ID_INVALID), fromHardware(false), opcode(opcodeIn),
	  packetID(0), CRC(0), messageBuff(messageIn), length(lengthIn), errorStatus(errorStatusIn), success(errorStatusIn == 0), sync(0){
	timestamp = getTimeInMillis();
}

ACPPacket::ACPPacket(const ACPPacket & packetSource){
	source			= packetSource.getSource();
	destination		= packetSource.getDestination();
	fromHardware	= packetSource.isFromHardware();
	opcode			= packetSource.getOpcode();
	length			= packetSource.getLength();
	messageBuff		= packetSource.getMessageBuff();
	packetID		= packetSource.getPacketID();
	CRC				= packetSource.getCRC();
	errorStatus		= packetSource.getErrorStatus();
	success			= packetSource.isSuccess();
	sync 			= packetSource.getSync();
	timestamp = getTimeInMillis();
}

// --- Construct from a buffer --------------------------------------------------------------------------------------
ACPPacket::ACPPacket(uint8 * buffer, std::size_t size_in){
	Logger * logger = static_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
	logger->Debug("Creating ACPPacket from buffer of size %d", (int) size_in);

	std::size_t size = size_in;

	source = LOCATION_ID_INVALID;
	destination = LOCATION_ID_INVALID;
	fromHardware = true;

	// Check that the buffer is sized correctly.
	if (NULL == buffer || size < ACP_MIN_BYTES){
		messageBuff = NULL;
		errorStatus = INVALID_PACKET;
		success = false;
		logger->Warning("ACPPacket: Null or incomplete packet");
		return;
	}

//	printf("RX Packet: ");
//	for(uint16_t i = 0; i < size_in; i++)
//	{
//		printf("0x%02x ",buffer[i]);
//	}
//	printf("\n");

	// Get the source
	sync = buffer[0];
	logger->Debug("ACPPacket sync: %u", (uint32) sync);
	buffer += 1;
	size -= 1;

	// Check that the top six bits match the sync code
	if((sync & 0xFC) != SYNC_VALUE){
		logger->Error("ACPPacket: wrong sync! Packet invalid.");
		messageBuff = NULL;
		errorStatus = INVALID_PACKET;
		success = false;
		return;
	}

	// Assign destination based on the lower two sync bits
	switch(sync & 0b11){
	case 0b00:
		destination = SERVER_LOCATION_EPS;
		break;
	case 0b01:
		destination = SERVER_LOCATION_COM;
		break;
	case 0b10:
		destination = SERVER_LOCATION_ACS;
		break;
	case 0b11:
		destination = SERVER_LOCATION_PLD;
		break;
	}

	packetID = (uint16)(((uint16)(buffer[0]) << 8) | buffer[1]);
	logger->Debug("ACPPacket ID: %u", (uint32) packetID);
	buffer += 2;
	size -= 2;

	opcode = buffer[0];
	logger->Debug("ACPPacket opcode: %u", (uint32) opcode);
	buffer += 1;
	size -= 1;

	length = ((uint16) buffer[0] << 8) | (uint16)buffer[1];
	logger->Debug("ACPPacket message length: %u", (uint32) length - 1);
	buffer += 2;
	size -= 2;

	// Make sure that the length we're expecting is correct
	size -= 2; // take CRC into account
	if(size != length){
		logger->Error("ACPPacket: length difference error! Packet invalid.");
		messageBuff = NULL;
		errorStatus = NO_ERROR_BYTE;
		success = false;
		return;
	}

	// Set success and errorStatus
	if(length == 0){ // errors byte not sent
		errorStatus = NO_ERROR_BYTE;
		success = false;
		return;
	}else{
		errorStatus = buffer[0];
		logger->Debug("ACPPacket: error status: %u", (uint32) errorStatus);
		success = (errorStatus == 0 || errorStatus == 0xAB); // 0xAB = unimplemented (ignore for development, TODO: RBF)
		buffer += 1;
		size -=1;
	}

	// Allocate and fill in message buffer
	messageBuff = (uint8*) malloc(length - 1);
	memcpy(messageBuff, buffer, length - 1);
	buffer += length - 1;

	// Check that the allocation worked correctly.
	if (NULL == messageBuff){
		logger->Error("ACPPacket: NULL message pointer after memcpy");
		return;
	}

	// Get the length
	CRC = ((uint16) buffer[0] << 8) | (uint16) buffer[1];
	logger->Debug("ACPPacket CRC: %#04x", (uint32) CRC);

	length--; // remove error byte from length
	timestamp = getTimeInMillis();
}

// --- Flatten into buffer --------------------------------------------------------------------------------------
std::size_t ACPPacket::GetFlattenSize(void ) const{
	return (length + ACP_MIN_BYTES);
}

std::size_t ACPPacket::Flatten(uint8 * buffer, std::size_t size) const{
	size_t numCopied = 0;
	uint8 * bufferStart = buffer;
	crc_t crc;

	// Check the buffer and buffer size.
	if (NULL == buffer || size < GetFlattenSize( )){
		return 0;
	}

	// Copy the sync, packetID, opcode, and length
	*(buffer++) = sync & 0xFF;
	numCopied += 1;

	*(buffer++) = (packetID >> 8) & 0xFF;
	*(buffer++) = packetID & 0xFF;
	numCopied += 2;

	*(buffer++) = opcode & 0xFF;
	numCopied += 1;

	*(buffer++) = (length >> 8) & 0xFF;
	*(buffer++) = length & 0xFF;
	numCopied += 2;

	memcpy(buffer, messageBuff, length);
	buffer += length;
	numCopied += length;

	// FIXME: check CRC!
	crc = crcSlow(bufferStart, numCopied);
	for (size_t i = 0; i < sizeof(crc); ++i)
	{
		*(buffer++) = (crc >> (8*(sizeof(crc)-i-1))) & 0xff;
	}
	numCopied += sizeof(crc);

	return numCopied;
}

} // End of namespace Core
} // End of namespace AllStar
