/*! \file Packet.h
 *  \brief Header File for the Packet Class
 *
 *  This file contains the definition of the Packet class, encapsulates a
 *  packet in the Phoenix Message Protocol.
 */

#ifndef _PACKET_H
#define _PACKET_H

#include "core/StdTypes.h"
#include "core/Message.h"

namespace Phoenix
{
    namespace Core
    {
        /*! \brief Class that Encapsulates a Phoenix Packet
         *
         *  This class encapsulates a Phoenix message packet used to send
         *  messages between servers.
         */
        class Packet
        {
        public:
            /*! \brief Constructor for the Packet Class
             *
             *  Creates an empty packet.
             */
            Packet(void);

            /*! \brief Constructor for the Packet Class
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
            Packet(LocationIDType sourceIn, LocationIDType destIn, uint16 numberIn, uint32 timestampIn, Message * messageIn);

            /*! \brief Constructor for the Packet Class
             *
             *  Creates a packet object from the given buffer as specified in
             *  the xxx Specification.
             */
            Packet(uint8 * buffer, std::size_t size);

            /*! \brief Copy constructor for the Packet class */
            Packet(const Packet & source);

            /*! \brief Destructor for the Packet class */
            virtual ~Packet(void);

            /*! \brief Assignment Operator for the Packet Class */
            Packet & operator=(const Packet & source);

            /*! \brief Equals Operator for the Packet Class
             *
             *  \param comparison Packet with which to compare the calling
             *  object.
             *
             *  \return true if the packets are equal and false otherwise.
             */
            bool operator==(const Packet & comparison) const;

            /*! \brief Return the Source Location of the Packet
             *
             *  \return The source location of the packet.
             */
            LocationIDType GetSource(void) const;

            /*! \brief Return the Destination Location of the Packet
             *
             *  \return The destination location of the packet.
             */
            LocationIDType GetDestination(void) const;

            /*! \brief Return the Number of the Packet
             *
             *  \return The number of the packet.
             */
            uint16 GetNumber(void) const;

            /*! \brief Return the Time Stamp of the Packet
             *
             *  \return The time stamp of the packet.
             */
            uint32 GetTimestamp(void) const;

            /*! \brief Return a Pointer to the Message of the Packet
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

            /*! \brief Set the Source Location of the Packet
             *
             *  \param newSource New source location for the packet.
             */
            void SetSource(LocationIDType newSource);

            /*! \brief Set the Destination Location of the Packet
             *
             *  \param newDestination New destination location for the packet.
             */
            void SetDestination(LocationIDType newDestination);

            /*! \brief Set the Number of the Packet
             *
             *  \param newNumber New number for the packet.
             */
            void SetNumber(uint16 newNumber);

            /*! \brief Set the time stamp of the Packet
             *
             *  \param newTimestamp New time stamp for the packet.
             */
            void SetTimestamp(uint32 newTimestamp);

            /*! \brief Set the Message of the Packet
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
            /*! \brief Source of the Packet */
            LocationIDType source;

            /*! \brief Destination of the Packet */
            LocationIDType destination;

            /*! \brief Number of the Packet */
            uint16 number;

            /*! \brief Timestamp of the Packet */
            uint32 timestamp;

            /*! \brief Message of the Packet */
            Message * messagePtr;

            /*! \brief Number Counter for the Packet */
            static std::size_t packetCounter;
        };
    }
}

#endif  //_PACKET_H
