/*! \file FSWPacket.h
 *  \brief Header File for the FSWPacket Class
 *
 *  This file contains the definition of the FSWPacket class, encapsulates a
 *  packet in the Phoenix Message Protocol.
 */

#ifndef _FSWPACKET_H
#define _FSWPACKET_H

#include "core/StdTypes.h"
#include "core/Message.h"

namespace Phoenix
{
    namespace Core
    {
        /*! \brief Class that Encapsulates a Phoenix FSWPacket
         *
         *  This class encapsulates a Phoenix message packet used to send
         *  messages between servers.
         */
        class FSWPacket
        {
        public:

        	// --- Empty constructor -------------------------------------------------------------------------------------------
            FSWPacket(void);

            // --- Constructor from a raw buffer -------------------------------------------------------------------------------
            FSWPacket(uint8 * buffer, std::size_t size);

            // --- Constructors with for dispatching ---------------------------------------------------------------------------
            FSWPacket(LocationIDType sourceIn, LocationIDType destIn, uint8 opcodeIn, bool success, bool response, MessageTypeEnum type);
            FSWPacket(LocationIDType sourceIn, LocationIDType destIn, uint8 opcodeIn, bool success, bool response, MessageTypeEnum type, uint16 lengthIn, uint8 * messageIn);

            // --- Constructors for return messages ----------------------------------------------------------------------------
            FSWPacket(uint8 opcodeIn, bool successIn, bool responseIn, MessageTypeEnum typeIn);
            FSWPacket(uint8 opcodeIn, bool successIn, bool responseIn, MessageTypeEnum typeIn, uint16 lengthIn, uint8 * messageIn);

            // --- Constructor from existing packet ----------------------------------------------------------------------------
            FSWPacket(const FSWPacket & source);

            virtual ~FSWPacket(void);
            bool operator==(const FSWPacket & comparison) const;

            // --- Grab information from a packet ------------------------------------------------------------------------------
            LocationIDType GetSource(void) const;
            LocationIDType GetDestination(void) const;
            uint16 GetNumber(void) const;
            uint32 GetTimestamp(void) const;
            uint8 * GetMessageBufPtr(void) const;
            uint8 GetStatus(void) const;
			bool IsResponse(void) const;
			bool IsSuccess(void) const;
			MessageTypeEnum GetType(void) const;
			uint8 GetOpcode(void) const;
            uint16 GetMessageLength(void) const;
            std::size_t GetPacketCounter(void) const;

            // --- Set packet information --------------------------------------------------------------------------------------
            void SetSource(LocationIDType newSource);
            void SetDestination(LocationIDType newDestination);
            void SetNumber(uint16 newNumber);
            void SetTimestamp(uint32 newTimestamp);
            void SetMessageBuf(uint8 * newPtr);
            void SetResponse(bool responseIn);
            void SetSuccess(bool successIn);
            void SetOpcode(uint8 opcodeIn);

            uint32 MakeTimestamp();

            // --- Packet flattening -------------------------------------------------------------------------------------------
            virtual std::size_t GetFlattenSize(void ) const;
            virtual std::size_t Flatten(uint8 * buffer, std::size_t size) const;

        private:

            LocationIDType source;
            LocationIDType destination;
            uint16 number;
            uint32 timestamp;

            uint8 status;
            bool response;
            bool success;
            MessageTypeEnum type;
            uint16 length;
            uint8 opcode;
            uint8 * messageBuf;
            static std::size_t packetCounter;

        };
    }
}

#endif  //_PACKET_H
