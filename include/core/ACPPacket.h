/*! \file ACPPacket.h
 *  \brief Header File for the ACPPacket Class
 *
 *  This file contains the definition of the ACPPacket class, encapsulates a
 *  packet in the AllStar Message Protocol.
 */

#ifndef _ACPPACKET_H
#define _ACPPACKET_H

#include "core/StdTypes.h"

namespace AllStar{
namespace Core{

typedef uint8 ErrorStatusType;

enum ErrorStatusEnum{
	INVALID_PACKET = 100,
	NO_ERROR_BYTE,
	NO_RESPONSE,
	FAILED_DISPATCH
};

#define ACP_MIN_BYTES 8
#define SYNC_VALUE 0xA4

class ACPPacket
{
public:
	// --- Empty constructor -------------------------------------------------------------------------------------------
	ACPPacket(void);

	// --- Constructor from a raw buffer -------------------------------------------------------------------------------
	ACPPacket(uint8 * buffer, std::size_t size);

	// --- Constructors with for dispatching ---------------------------------------------------------------------------
	ACPPacket(LocationIDType sourceIn, LocationIDType destIn, uint8 opcodeIn);
	ACPPacket(LocationIDType sourceIn, LocationIDType destIn, uint8 opcodeIn, uint16 lengthIn, uint8 * messageIn);

	// --- Constructors for return messages ----------------------------------------------------------------------------
	ACPPacket(uint8 opcodeIn, ErrorStatusType errorStatusIn);
	ACPPacket(uint8 opcodeIn, ErrorStatusType errorStatusIn, uint16 lengthIn, uint8 * messageIn);

	// --- Constructor from existing packet ----------------------------------------------------------------------------
	ACPPacket(const ACPPacket & source);

	virtual ~ACPPacket(void);
	bool operator==(const ACPPacket & comparison) const;

	// --- Packet flattening -------------------------------------------------------------------------------------------
	virtual std::size_t GetFlattenSize(void ) const;
	virtual std::size_t Flatten(uint8 * buffer, std::size_t size) const;


	// --- Getters and Setters -----------------------------------------------------------------------------------------
	uint16 getCRC() const {
		return CRC;
	}

	void setCRC(uint16 crc) {
		CRC = crc;
	}

	uint16 getLength() const {
		return length;
	}

	void setLength(uint16 length) {
		this->length = length;
	}

	uint8* getMessageBuff() const {
		return messageBuff;
	}

	void setMessageBuff(uint8* messageBuf) {
		this->messageBuff = messageBuff;
	}

	uint8 getOpcode() const {
		return opcode;
	}

	void setOpcode(uint8 opcode) {
		this->opcode = opcode;
	}

	uint16 getPacketID() const {
		return packetID;
	}

	void setPacketID(uint16 packetId) {
		packetID = packetId;
	}

	LocationIDType getSource() const {
		return source;
	}

	void setSource(LocationIDType source) {
		this->source = source;
	}

	LocationIDType getDestination() const {
		return destination;
	}

	void setDestination(LocationIDType destination) {
		this->destination = destination;
	}

	bool isFromHardware() const {
		return fromHardware;
	}

	void setFromHardware(bool fromHardware) {
		this->fromHardware = fromHardware;
	}

	uint64 getTimestamp() const {
		return timestamp;
	}

	void setTimestamp(uint64 timestamp) {
		this->timestamp = timestamp;
	}

	ErrorStatusType getErrorStatus() const {
		return errorStatus;
	}

	void setErrorStatus(ErrorStatusType errorStatus) {
		this->errorStatus = errorStatus;
	}

	bool isSuccess() const {
		return success;
	}

	void setSuccess(bool success) {
		this->success = success;
	}

	uint8 getSync() const {
		return sync;
	}

	void setSync(uint8 sync) {
		this->sync = sync;
	}

private:
	// --- Internal Use ------------------------------------------------------------------------------------------------
	LocationIDType source;
	LocationIDType destination;
	uint64 timestamp;
	ErrorStatusType errorStatus;
	bool success;
	bool fromHardware;

	// --- Packet Definition -------------------------------------------------------------------------------------------
	uint8 sync;
	uint16	packetID;
	uint8	opcode;
	uint16	length;
	uint8 *	messageBuff;
	uint16	CRC;
};

} // End of namespace Core
} // End of namespace AllStar

#endif  //_PACKET_H
