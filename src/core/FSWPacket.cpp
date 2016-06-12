//
//
//  Generated by StarUML(tm) C++ Add-In
//
//  @ Project : Phoenix
//  @ File Name : FSWPacket.cpp
//  @ Date : 2/9/2010
//  @ Author :
//
//

#include "core/FSWPacket.h"
#include "util/Logger.h"
#include "util/crc.h"
#include <stdio.h>

//#include "boards/backplane/dbg_led.h"

using namespace std;

namespace Phoenix
{
    namespace Core
    {
        std::size_t FSWPacket::packetCounter = 0;

        FSWPacket::FSWPacket(void )
        {
            source = LOCATION_ID_INVALID;
            destination = LOCATION_ID_INVALID;
            number = 0;
            timestamp = 0;
            status = 0;
            response = 0;
            success = 0;
            type = (MessageTypeEnum) 0;
            opcode = 0;
            length = 0;
            messagePtr = NULL;
            messageBuf = NULL;
        }

        FSWPacket::FSWPacket(LocationIDType sourceIn, LocationIDType destIn, uint16 numberIn, uint32 timestampIn, Message * messageIn)
        {
        	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
        	logger->Log("FSW Packet: outdated constructor called!", LOGGER_LEVEL_ERROR);
            source = sourceIn;
            destination = destIn;
            timestamp = timestampIn;
            opcode = messageIn->GetOpcode();
            // TODO: add the following
            status = 0;
            response = 0;
			success = 0;
			type = (MessageTypeEnum) 0;
			length = 0;
            messageBuf = NULL; //TODO: remove this
            if (messageIn != NULL)
            {
                messagePtr = messageIn->Duplicate();
            }
            else
            {
                messagePtr = NULL;
                number = numberIn;
                return;
            }
			
            if (NULL == messagePtr)
            {
                source = LOCATION_ID_INVALID;
                destination = LOCATION_ID_INVALID;
                number = 0;
                return;
            }
			
            if(messagePtr->IsResponse())
            {
                number = numberIn;
            }
            else
            {
                number = packetCounter++;
            }
        }

        FSWPacket::FSWPacket(LocationIDType sourceIn, LocationIDType destIn, uint16 numberIn, uint32 timestampIn, uint8 opcodeIn, uint8 * messageIn)
		{
			Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
			logger->Log("FSW Packet: outdated constructor called!", LOGGER_LEVEL_ERROR);
			source = sourceIn;
			destination = destIn;
			timestamp = timestampIn;
			opcode = opcodeIn;
			status = 0;
			response = 0;
			success = 0;
			type = (MessageTypeEnum) 0;
			length = 0;
			messagePtr = NULL; //TODO: remove this
			if (messageIn != NULL)
			{
				messageBuf = messageIn;
			}
			else
			{
				messageBuf = NULL;
				number = numberIn;
				return;
			}

			if (NULL == messageBuf)
			{
				source = LOCATION_ID_INVALID;
				destination = LOCATION_ID_INVALID;
				number = 0;
				return;
			}

			if(messagePtr->IsResponse())
			{
				number = numberIn;
			}
			else
			{
				number = packetCounter++;
			}
		}

        FSWPacket::FSWPacket(uint8 * buffer, std::size_t size_in)
        {
        	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
        	logger->Log("Creating FSWPacket from buffer of size %d\n", (int) size_in, LOGGER_LEVEL_DEBUG);

        	messagePtr = NULL; // TODO: remove messagePtr from class

        	std::size_t size = size_in;

        	// Check that the buffer is sized correctly.
            if (NULL == buffer || size < 14)
            {
                source = LOCATION_ID_INVALID;
                destination = LOCATION_ID_INVALID;
                messageBuf = NULL;
                logger->Log("FSWPacket: Null or incomplete packet", LOGGER_LEVEL_DEBUG);
                return;
            }

            // -----------------------------------------------------------------------------------

            // Get the source
            source = (LocationIDType)(((uint16)(buffer[0]) << 8) | buffer[1]);
            logger->Log("FSWPacket source: %u", (uint32) source, LOGGER_LEVEL_DEBUG);
            buffer += 2;
            size -= 2;

            // Get the destination
            destination = (LocationIDType)(((uint16)(buffer[0]) << 8) | buffer[1]);
            logger->Log("FSWPacket destination: %u", (uint32) destination, LOGGER_LEVEL_DEBUG);
            buffer += 2;
            size -= 2;

            // Get the number
            number = (uint16)(((uint16)(buffer[0]) << 8) | buffer[1]);
            logger->Log("FSWPacket number: %u", (uint32) number, LOGGER_LEVEL_DEBUG);
            buffer += 2;
            size -= 2;

            // Get the timestamp
            timestamp = (uint32)(((uint32)(buffer[0]) << 24) | ((uint32)(buffer[1]) << 16) | ((uint32)(buffer[0]) << 8) | ((uint32)(buffer[3])));
            logger->Log("FSWPacket timestamp: %u", timestamp, LOGGER_LEVEL_DEBUG);
            buffer += 4;
            size -= 4;

            // Get the status, then response, success, and type
            status = buffer[0];
            response = (status >> 7) & 0x01;
			success = (status >> 6) & 0x01;
			type = (MessageTypeEnum)(status & 0x3F);
			logger->Log("FSWPacket response: %d", response, LOGGER_LEVEL_DEBUG);
			logger->Log("FSWPacket success: %d", success, LOGGER_LEVEL_DEBUG);
			logger->Log("FSWPacket type: %u", (uint32) type, LOGGER_LEVEL_DEBUG);
            buffer++;
            size--;

            // Get the opcode
            opcode = buffer[0];
            logger->Log("FSWPacket opcode: %u", (uint32) opcode, LOGGER_LEVEL_DEBUG);
            buffer++;
            size--;

            // Get the length
            length = ((uint16) buffer[0] << 8) | (uint16)buffer[1];
            logger->Log("FSWPacket message length: %u", (uint32) length, LOGGER_LEVEL_DEBUG);
            buffer += 2;
            size -= 2;

            // -----------------------------------------------------------------------------------

            // Make sure that the length we're expecting is correct
            size -= 2; // take CRC into account
            if(size != length){
            	logger->Log("FSWPacket: length difference error! Packet invalid.", LOGGER_LEVEL_ERROR);
            	source = LOCATION_ID_INVALID;
				destination = LOCATION_ID_INVALID;
				messageBuf = NULL;
				return;
            }

            // Allocate and fill in message buffer
            messageBuf = (uint8*) malloc(length);
            memcpy(messageBuf, buffer, length);

            // Check that the allocation worked correctly.
            if (NULL == messageBuf)
            {
            	logger->Log("FSWPacket: NULL message pointer after memcpy", LOGGER_LEVEL_ERROR);
                source = LOCATION_ID_INVALID;
                destination = LOCATION_ID_INVALID;
                return;
            }
        }

        FSWPacket::FSWPacket(LocationIDType sourceIn, LocationIDType destIn, uint32 timestampIn, uint8 opcodeIn, bool successIn, bool responseIn, MessageTypeEnum typeIn){
        	destination = sourceIn;
			source = destIn;
			number = 1;
			timestamp = destIn;
			type = typeIn;
			success = successIn;
			response = responseIn;
			status = 0; // TODO: this isn't accurate, but check that it doesn't need to be
			opcode = opcodeIn;
			length = 0;
			messagePtr = NULL;
			messageBuf = NULL;
        }

        FSWPacket::FSWPacket(uint32 timestampIn, uint8 opcodeIn, bool successIn, bool responseIn, MessageTypeEnum typeIn){
			destination = LOCATION_ID_INVALID;
			source = LOCATION_ID_INVALID;
			number = 1;
			timestamp = timestampIn;
			type = typeIn;
			success = successIn;
			response = responseIn;
			status = 0; // TODO: this isn't accurate, but check that it doesn't need to be
			opcode = opcodeIn;
			length = 0;
			messagePtr = NULL;
			messageBuf = NULL;
		}

        FSWPacket::FSWPacket(const FSWPacket & packetSource)
        {
//            if (NULL == packetSource.GetMessagePtr())
//            {
//                source = LOCATION_ID_INVALID;
//                destination = LOCATION_ID_INVALID;
//                number = 0;
//                timestamp = 0;
//                messagePtr = NULL;
//                status = 0;
//                opcode = packetSource.GetMessagePtr()->GetOpcode();
//                return;
//            }
//            messagePtr = packetSource.GetMessagePtr()->Duplicate();
//
//            if (NULL == messagePtr)
//            {
//                source = LOCATION_ID_INVALID;
//                destination = LOCATION_ID_INVALID;
//                number = 0;
//                timestamp = 0;
//                opcode = 0;
//                status = 0;
//                return;
//            }
			
            destination = packetSource.destination;
            source = packetSource.source;
            number = packetSource.number;
            timestamp = packetSource.timestamp;
            status = packetSource.GetStatus();
            response = packetSource.IsResponse();
            success = packetSource.IsSuccess();
            type = packetSource.GetType();
            opcode = packetSource.GetOpcode();
            length = packetSource.GetMessageLength();
            messagePtr = packetSource.GetMessagePtr();
            messageBuf = packetSource.GetMessageBufPtr();

        }

        FSWPacket::~FSWPacket(void )
        {
            delete messagePtr;
            free(messageBuf);
            //messagePtr = NULL;
        }

        FSWPacket & FSWPacket::operator=(const FSWPacket & source)
        {
            if (this == &source)
            {
                return *this;
            }

            SetSource(source.GetSource());
            SetDestination(source.GetDestination());
            SetNumber(source.GetNumber());
            SetTimestamp(source.GetTimestamp());
            if (NULL == source.GetMessagePtr())
            {
                messagePtr = NULL;
            }
            else
            {
                messagePtr = source.GetMessagePtr();
            }

            return *this;
        }


        LocationIDType FSWPacket::GetDestination(void) const
        {
            return destination;
        }

        LocationIDType FSWPacket::GetSource(void) const
        {
            return source;
        }

        uint16 FSWPacket::GetNumber(void) const
        {
            return number;
        }

        uint32 FSWPacket::GetTimestamp(void) const
        {
            return timestamp;
        }

        Message * FSWPacket::GetMessagePtr(void) const
        {
            return messagePtr;
        }

        uint8 * FSWPacket::GetMessageBufPtr(void) const
        {
        	return messageBuf;
        }

        uint8 FSWPacket::GetStatus(void) const
        {
        	return status;
        }

		bool FSWPacket::IsResponse(void) const
		{
			return response;
		}

		bool FSWPacket::IsSuccess(void) const
		{
			return success;
		}

		MessageTypeEnum FSWPacket::GetType(void) const
		{
			return type;
		}

		uint8 FSWPacket::GetOpcode(void) const
		{
			return opcode;
		}

        uint16 FSWPacket::GetMessageLength(void) const
        {
        	return length;
        }

        std::size_t FSWPacket::GetPacketCounter(void) const
        {
            return packetCounter;
        }

        std::size_t FSWPacket::GetFlattenSize(void ) const
        {
            return (length + 16);

		}

        bool FSWPacket::operator==(const FSWPacket & check) const
		{
            if (messagePtr == NULL)
            {
                return false;
            }

            return ((source == check.source) &&
                    (destination == check.destination) &&
                    (number == check.number) &&
                    (timestamp == check.timestamp) &&
                    (*messagePtr == check.messagePtr));
		}

        void FSWPacket::SetSource(LocationIDType newSource)
        {
            source = newSource;
        }

        void FSWPacket::SetDestination(LocationIDType newDestination)
        {
            destination = newDestination;
        }

        void FSWPacket::SetNumber(uint16 newNumber)
        {
            number = newNumber;
        }

        void FSWPacket::SetTimestamp(uint32 newTimestamp)
        {
            timestamp = newTimestamp;
        }

        void FSWPacket::SetMessage(Message * newMessage)
        {
            delete messagePtr;
            messagePtr = newMessage->Duplicate();
        }

        void FSWPacket::SetMessageBuf(uint8 * newPtr){
        	delete messageBuf;
        	messageBuf = newPtr;
        }

        void FSWPacket::SetResponse(bool responseIn){
        	response = responseIn;
        }

		void FSWPacket::SetSuccess(bool successIn){
			success = successIn;
		}

        std::size_t FSWPacket::Flatten(uint8 * buffer, std::size_t size) const
        {
            size_t numCopied = 0;
            uint8 * bufferStart = buffer;
            crc_t crc;

            // Check the buffer and buffer size.
            if (NULL == buffer || size < GetFlattenSize( ))
            {
                return 0;
            }

            // Copy the source, destination, number, messagePtr
            *(buffer++) = (source >> 8) & 0xFF;
            *(buffer++) = source & 0xFF;
            numCopied += 2;
            size -= 2;

            *(buffer++) = (destination >> 8) & 0xFF;
            *(buffer++) = destination & 0xFF;
            numCopied += 2;
            size -= 2;

            *(buffer++) = (number >> 8) & 0xFF;
            *(buffer++) = number & 0xFF;
            numCopied += 2;
            size -= 2;

            *(buffer++) = (timestamp >> 24) & 0xFF;
            *(buffer++) = (timestamp >> 16) & 0xFF;
            *(buffer++) = (timestamp >> 8)  & 0xFF;
            *(buffer++) = timestamp & 0xFF;
            numCopied += 4;
            size -= 4;

            *(buffer++) = status & 0xFF;
            *(buffer++) = opcode & 0xFF;
            numCopied += 2;
            size -= 2;

//            if(messagePtr){
//            	length = messagePtr->GetFlattenSize();
//            	*(buffer++) = (length >> 8) & 0xFF;
//            	*(buffer++) = length & 0xFF;
//            	numCopied += sizeof(uint16);
//            	messageCopied += messagePtr->Flatten(buffer, size);
//				numCopied += messageCopied;
//				buffer += messageCopied;
//            }else{
//            	*(buffer++) = 0x00;
//            	*(buffer++) = 0x00;
//            	numCopied += sizeof(uint16);
//            }

            *(buffer++) = (length >> 8) & 0xFF;
            *(buffer++) = length & 0xFF;
            numCopied += 2;
            size -= 2;

            memcpy(buffer, messageBuf, length);
            buffer += length;
            numCopied += length;
            size -= length;

            // TODO: check CRC!
            crc = crcSlow(bufferStart, numCopied);
			for (size_t i = 0; i < sizeof(crc); ++i)
			{
				*(buffer++) = (crc >> (8*(sizeof(crc)-i-1))) & 0xff;
			}
			numCopied += sizeof(crc);

			return numCopied;
		}
    }
}
