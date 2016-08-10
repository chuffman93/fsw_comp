/*! \file ReturnMessage.h
 *  \brief Header File for the Return Class.
 *
 *  This class defines a ReturnMessage, which is any message that is sent as a
 *  response from one server to another, as opposed to being originally sent,
 *  as in a SendMessage.
 *
 */


#ifndef _MESSAGERETURN_H
#define _MESSAGERETURN_H

#include "core/Message.h"
#include "core/StdTypes.h"
#include "core/VariableTypeData.h"

namespace AllStar
{
    namespace Core
    {
        class ReturnMessage : public Message
        {
        public:
        	/*! \brief Default constructor for ReturnMessage.
			 *
			 *  \note Sets the message pointer to null, and success to 0.
			 *
			 */
        	ReturnMessage(void );

        	/*! \brief Constructor for ReturnMessage from a message pointer and bool.
			 *
			 *  \param messageIn Message pointer of the message
			 *	\param successIn Success of the message
			 *
			 */
			ReturnMessage(Message * messageIn, bool successIn);

			/*! \brief Copy Constructor for ReturnMessage.
			 *
			 *  \param source Object to be copied.
			 *
			 */
			ReturnMessage(const ReturnMessage & source);

			/*! \brief Destructor for SendMessage. */
			~ReturnMessage(void );

			/*! \brief Returns if this message is a response or not.
			 *
			 *  \return Whether or not this message is a response.
			 *
			 */
			bool IsResponse(void ) const;

			/*! \brief Returns if this message was successful.
			 *
			 *  \return Whether or not this message was successful.
			 *
			 */
			bool GetSuccess(void) const;

			/*! \brief Sets success to the parameter's value.
			 *
			 *  \param successIn Value to set success to.
			 *
			 */
			void SetSuccess(bool successIn);

			/*! \brief Returns the type of this message.
			 *
			 *  \return Type of this message.
			 *
			 */
			MessageTypeEnum GetType(void ) const;

			/*! \brief Returns the opcode of this message.
			 *
			 *  \return Opcode of this message.
			 *
			 */
			MessageCodeType GetOpcode(void ) const;

			/*! \brief Returns the message pointer of this message.
			 *
			 *  \return Message pointer of this message.
			 *
			 */
			Message * GetMessagePtr(void ) const;

			/*! \brief Equals Operator for ReturnMessage.
			 *
			 *  \param check Message pointer of object to compare (polymorphic).
			 *
			 *  \return Whether this object is equal to the object
			 *  pointed to by check.
			 *
			 */
			virtual bool operator ==(Message * check) const;

			/*! \brief Assignment Operator for ReturnMessage.
			 *
			 *  \param source Object to be copied.
			 *
			 *  \return Reference to the calling object.
			 *
			 */
			ReturnMessage & operator =(const ReturnMessage & source);

			/*! \brief Returns a pointer to a new ReturnMessage object that is a copy of the calling object.
			 *
			 *  \param source Object to be copied.
			 *
			 *  \return A pointer to a new ReturnMessage object that is a copy of the calling object.
			 *
			 */
			Message * Duplicate(void ) const;

			/*! \brief Returns the minimum buffer size required by Flatten( ).
			 *
			 *  \return Minimum Flatten( ) buffer size.
			 *
			 */
			std::size_t GetFlattenSize(void ) const;

			/*! \brief Flattens the calling object into the given buffer.
			 *
			 *  \param buffer Buffer to copy data into.
			 *  \param size Size of buffer.
			 *
			 *  \return Number of bytes actually copied into buffer.
			 *
			 */
			std::size_t Flatten(uint8 * buffer, std::size_t size) const;
        private:
			/*! \brief Message pointer of ReturnMessage */
			Message * messagePtr;
			/*! \brief Success of the message. */
			bool success;
        };
    }
}

#endif  //_MESSAGERETURN_H
