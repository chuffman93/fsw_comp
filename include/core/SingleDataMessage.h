/*! \file SingleDataMessage.h
 *  \brief Header File for the SingleDataMessage Class.
 *
 *  This class defines a SingleDataMessage, which is any message that contains a single
 *  VariableTypeData field (DataMessage and ErrorMessage are derivatives).
 *
 */

#ifndef _SINGLEDATAMESSAGE_H
#define _SINGLEDATAMESSAGE_H

#include "core/SendMessage.h"
#include "core/VariableTypeData.h"

namespace AllStar
{
    namespace Core
    {
		/*! \brief Superclass for all single data messages.
		 *
		 *  This class defines a SingleDataMessage, which is any message that contains a single
		 *  VariableTypeData field (DataMessage and ErrorMessage are derivatives).
		 *
		 */
        class SingleDataMessage : public SendMessage
        {
        public:
        	/*! \brief Destructor for SingleDataMessage. */
            virtual ~SingleDataMessage(void );

            /*! \brief Returns the type of this message.
			 *
			 *  \return Type of this message.
			 *
			 */
			virtual MessageTypeEnum GetType(void ) const = 0;

			/*! \brief Returns the data held by this message.
			 *
			 *  \return Data in this message
			 *
			 */
			VariableTypeData & GetData(void );

			/*! \brief Sets the data held by this message.
			 *
			 *  \param newData Data to be assigned to this message.
			 *
			 *  \return Whether the operation was successful.
			 *
			 */
			bool SetData(const VariableTypeData & newData);

			/*! \brief Sets the data held by this message from a buffer.
			 *
			 *  \note Buffer must start from the "length" field.
			 *
			 *  \param buffer Buffer in which the new data resides.
			 *  \param size Size of the buffer.
			 *
			 *  \return Whether the operation was successful.
			 *
			 */
			bool SetData(uint8 * buffer, std::size_t size);

			/*! \brief Equals Operator for SingleDataMessage.
			 *
			 *  \param check Message pointer of object to compare (polymorphic).
			 *
			 *  \return Whether this object is equal to the object
			 *  pointed to by check.
			 *
			 */
			bool operator ==(Message * check) const;

			/*! \brief Assignment Operator for SingleDataMessage.
			 *
			 *  \param source Object to be copied.
			 *
			 *  \return Reference to the calling object.
			 *
			 */
            SingleDataMessage & operator=(const SingleDataMessage & source);

            /*! \brief Returns a pointer to a new SingleDataMessage object that is a copy of the calling object.
             *
             *  \param source Object to be copied.
			 *
			 *  \return A pointer to a new SingleDataMessage object that is a copy of the calling object.
			 *
			 */
            virtual Message * Duplicate(void ) const = 0;

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
            /*! \brief Data held by this message */
            VariableTypeData data;
        protected:
            /*! \brief Default constructor for SingleDataMessage.
			 *
			 *  \note Sets the opcode to 0, and uses default VariableTypeData constructor.
			 *
			 */
            SingleDataMessage();

            /*! \brief Constructor for SingleDataMessage with MessageCodeType parameter.
			 *
			 *  \note Uses default VariableTypeData constructor.
			 *
			 *  \param opcodeIn Opcode of the message.
			 *
			 */
			SingleDataMessage(MessageCodeType opcodeIn);

			/*! \brief Constructor for SingleDataMessage with MessageCodeType and VariableTypeData parameter.
			 *
			 *  \param opcodeIn Opcode of the message.
			 *  \param dataIn Data of the message.
			 *
			 */
            SingleDataMessage(MessageCodeType opcodeIn, const VariableTypeData & dataIn);

            /*! \brief Copy Constructor for SingleDataMessage.
             *
             *  \param source Object to be copied.
             *
             */
			SingleDataMessage(const SingleDataMessage & source);
        };
    }
}

#endif  //_MULTIDATAMESSAGE_H
