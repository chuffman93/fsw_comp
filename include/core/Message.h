/*! \file Message.h
 *  \brief Header File for the Message Class.
 *
 *  This class encapsulates the message field of a Phoenix message protocol
 *  packet, and defines a superclass for the specific derivatives of Message to
 *  inherit from.
 *
 */

#ifndef _MESSAGE_H
#define _MESSAGE_H

#include "core/StdTypes.h"

namespace Phoenix
{
    namespace Core
    {
		/*! \brief Superclass for all other messages.
		 *
		 *  This class encapsulates the message field of a Phoenix message protocol
		 *  packet, and defines a superclass for the specific derivatives of Message
		 *  to inherit from.
		 *
		 */
        class Message
        {
        public:
        	/*! \brief Returns a pointer to a new Message object created from a buffer.
			 *
			 *  \return A pointer to the Message object.
			 *
			 *  \param buffer Buffer to create the Message object from.
			 *  \param size Size of the buffer.
			 *
			 *  \note This function will not return an actual Message object, as it
			 *  cannot be instantiated, but a pointer to an object of one of its
			 *  derived classes (e.g. DataMessage).  Be careful to delete the returned
			 *  pointer as it is allocated on the heap.
			 *
			 */
        	static Message * CreateMessage(uint8 * buffer, std::size_t size);

        	/*! \brief Destructor for Message. */
            virtual ~Message(void ) { }

            /*! \brief Returns if this message is a response or not.
			 *
			 *  \return Whether or not this message is a response.
			 *
			 */
            virtual bool IsResponse(void ) const = 0;

            /*! \brief Returns if this message was successful.
			 *
			 *  \return Whether or not this message was successful.
			 *
			 */
            virtual bool GetSuccess(void ) const = 0;

            /*! \brief Returns the type of this message.
			 *
			 *  \return Type of this message.
			 *
			 */
            virtual MessageTypeEnum GetType(void ) const = 0;

            /*! \brief Returns the opcode of this message.
			 *
			 *  \return Opcode of this message.
			 *
			 */
            virtual MessageCodeType GetOpcode(void ) const = 0;

            /*! \brief Equals Operator for Message.
			 *
			 *  \param check Message pointer of object to compare (polymorphic).
			 *
			 *  \return Whether this object is equal to the object
			 *  pointed to by check.
			 *
			 */
			virtual bool operator ==(Message * check) const = 0;

			/*! \brief Returns a pointer to a new Message object that is a copy of the calling object.
			 *
			 *  \return A pointer to a new Message object that is a copy of the calling object.
			 *
			 */
			virtual Message * Duplicate(void ) const = 0;

            /*! \brief Returns the minimum buffer size required by Flatten( ).
			 *
			 *  \return Minimum Flatten( ) buffer size.
			 *
			 */
            virtual std::size_t GetFlattenSize(void ) const = 0;

            /*! \brief Flattens the calling object into the given buffer.
			 *
			 *  \param buffer Buffer to copy data into.
			 *  \param size Size of buffer.
			 *
			 *  \return Number of bytes actually copied into buffer.
			 *
			 */
            virtual std::size_t Flatten(uint8 * buffer, std::size_t size) const = 0;
        };
    }
}

#endif  //_MESSAGE_H
