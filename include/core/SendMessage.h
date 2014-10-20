/*! \file SendMessage.h
 *  \brief Header File for the SendMessage Class.
 *
 *  This class defines a SendMessage, which is any message that is originally sent from
 *  one server to another, as opposed to being returned, as in a ReturnMessage.
 *
 */

#ifndef _SENDMESSAGE_H
#define _SENDMESSAGE_H

#include "core/Message.h"

namespace Phoenix
{
    namespace Core
    {
		/*! \brief SendMessage superclass for all other originally sent messages.
		 *
		 *  This class defines a SendMessage, which is any message that is originally sent from
		 *  one server to another, as opposed to being returned, as in a ReturnMessage.
		 *
		 */
        class SendMessage : public Message
        {
        public:
        	/*! \brief Destructor for SendMessage. */
            virtual ~SendMessage(void );

            /*! \brief Returns if this message is a response or not.
			 *
			 *  \return Whether or not this message is a response.
			 *
			 */
            bool IsResponse(void )const{return false;}

            /*! \brief Returns if this message was successful.
			 *
			 *  \return Whether or not this message was successful.
			 *
			 */
            bool GetSuccess(void )const{return false;}

            /*! \brief Returns the type of this message.
			 *
			 *  \return Type of this message.
			 *
			 */
            virtual MessageTypeEnum GetType(void )const = 0;

            /*! \brief Returns the opcode of this message.
			 *
			 *  \return Opcode of this message.
			 *
			 */
            MessageCodeType GetOpcode(void ) const;

            /*! \brief Equals Operator for SendMessage.
			 *
			 *  \param check Message pointer of object to compare (polymorphic).
			 *
			 *  \return Whether this object is equal to the object
			 *  pointed to by check.
			 *
			 */
            virtual bool operator ==(Message * check) const;

            /*! \brief Assignment Operator for SendMessage.
			 *
			 *  \param source Object to be copied.
			 *
			 *  \return Reference to the calling object.
			 *
			 */
            SendMessage & operator=(const SendMessage & source);

            /*! \brief Returns a pointer to a new SendMessage object that is a copy of the calling object.
			 *
			 *  \param source Object to be copied.
			 *
			 *  \return A pointer to a new SendMessage object that is a copy of the calling object.
			 *
			 */
			virtual Message * Duplicate(void ) const = 0;

            /*! \brief Returns the minimum buffer size required by Flatten( ).
			 *
			 *  \return Minimum Flatten( ) buffer size.
			 *
			 */
            virtual std::size_t GetFlattenSize(void ) const;

            /*! \brief Flattens the calling object into the given buffer.
			 *
			 *  \param buffer Buffer to copy data into.
			 *  \param size Size of buffer.
			 *
			 *  \return Number of bytes actually copied into buffer.
			 *
			 */
            virtual std::size_t Flatten(uint8 * buffer, std::size_t size) const;
        private:
            /*! \brief Type of the opcode. */
            MessageCodeType opcode;
        protected:
            /*! \brief Default constructor for SendMessage.
             *
             *  \note Sets the opcode to 0.
			 *
			 */
			SendMessage(void );

            /*! \brief Constructor for Message from a MessageCodeType.
			 *
			 *  \param opcodeIn Opcode of the message.
			 *
			 */
            SendMessage(MessageCodeType opcodeIn);

            /*! \brief Copy Constructor for SendMessage.
             *
             *  \param source Object to be copied.
             *
             */
			SendMessage(const SendMessage & source);
        };
    }
}

#endif  //_SENDMESSAGE_H
