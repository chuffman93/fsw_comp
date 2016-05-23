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
            /*! \brief Constructor for the FSWPacket Class
             *
             *  Creates an empty packet.
             */
            FSWPacket(void);

            /*! \brief Constructor for the FSWPacket Class
             *
             *  Creates a packet object with the given source, destination,
             *  number, and message.
             *
             *  \param sourceIn Source of the packet.
             *  \param destIn Destination of the packet.
             *  \param numberIn Number of the packet.  This value is only used
             *  if messageIn is a ReturnMessage.
             *  \param timestampIn Timestamp of the packet.
             *  \param messageIn Pointer to the message of the packet.  This
             *  parameter is duplicated internally and is not changed by
             *  this constructor.
             */
            FSWPacket(LocationIDType sourceIn, LocationIDType destIn, uint16 numberIn, uint32 timestampIn, Message * messageIn);

            /*! \brief Constructor for the FSWPacket Class
             *
             *  Creates a packet object from the given buffer as specified in
             *  the xxx Specification.
             */
            FSWPacket(uint8 * buffer, std::size_t size);

            /*! \brief Copy constructor for the FSWPacket class */
            FSWPacket(const FSWPacket & source);

            /*! \brief Destructor for the FSWPacket class */
            virtual ~FSWPacket(void);

            /*! \brief Assignment Operator for the FSWPacket Class */
            FSWPacket & operator=(const FSWPacket & source);

            /*! \brief Equals Operator for the FSWPacket Class
             *
             *  \param comparison FSWPacket with which to compare the calling
             *  object.
             *
             *  \return true if the packets are equal and false otherwise.
             */
            bool operator==(const FSWPacket & comparison) const;

            /*! \brief Return the Source Location of the FSWPacket
             *
             *  \return The source location of the packet.
             */
            LocationIDType GetSource(void) const;

            /*! \brief Return the Destination Location of the FSWPacket
             *
             *  \return The destination location of the packet.
             */
            LocationIDType GetDestination(void) const;

            /*! \brief Return the Number of the FSWPacket
             *
             *  \return The number of the packet.
             */
            uint16 GetNumber(void) const;

            /*! \brief Return the Time Stamp of the FSWPacket
             *
             *  \return The time stamp of the packet.
             */
            uint32 GetTimestamp(void) const;

            /*! \brief Return a Pointer to the Message of the FSWPacket
             *
             *  \return A pointer to the message in the packet.
             */
            Message * GetMessagePtr(void) const;

            /*! \brief Returns the current value of packetCounter. */
            std::size_t GetPacketCounter(void) const;

            /*! \brief Return the Minimum Buffer Size Required by Flatten( ).
             *
             *  \return Minimum Flatten( ) buffer size.
             */
            virtual std::size_t GetFlattenSize(void ) const;

            /*! \brief Set the Source Location of the FSWPacket
             *
             *  \param newSource New source location for the packet.
             */
            void SetSource(LocationIDType newSource);

            /*! \brief Set the Destination Location of the FSWPacket
             *
             *  \param newDestination New destination location for the packet.
             */
            void SetDestination(LocationIDType newDestination);

            /*! \brief Set the Number of the FSWPacket
             *
             *  \param newNumber New number for the packet.
             */
            void SetNumber(uint16 newNumber);

            /*! \brief Set the time stamp of the FSWPacket
             *
             *  \param newTimestamp New time stamp for the packet.
             */
            void SetTimestamp(uint32 newTimestamp);

            /*! \brief Set the Message of the FSWPacket
             *
             *  \param newMessage New message for the packet.
             */
            void SetMessage(Message * newMessage);

            /*! \brief Flattens the calling object into the given buffer.
             *
             *  \param buffer Buffer to copy data into.
             *  \param size Size of buffer.
             *
             *  \return Number of bytes actually copied into buffer.
             */
            virtual std::size_t Flatten(uint8 * buffer, std::size_t size) const;
			
        private:
            /*! \brief Source of the FSWPacket */
            LocationIDType source;

            /*! \brief Destination of the FSWPacket */
            LocationIDType destination;

            /*! \brief Number of the FSWPacket */
            uint16 number;

            /*! \brief Timestamp of the FSWPacket */
            uint32 timestamp;

            /* brief status of the FSWPacket */
            uint8 status;

            /* brief opcode of the FSWPacket */
            uint8 opcode;

            /*! \brief Message of the FSWPacket */
            Message * messagePtr;

            /*! \brief Number Counter for the FSWPacket */
            static std::size_t packetCounter;
        };
    }
}

#endif  //_PACKET_H
