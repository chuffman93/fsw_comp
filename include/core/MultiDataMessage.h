/*! \file MultiDataMessage.h
 *  \brief Header File for the MultiDataMessage Class.
 *
 *  This class defines a MultiDataMessage, which is any message that contains a list
 *  of any type field (ConfigMessage and CommandMessage are derivatives).  It is
 *  implemented as a template class.
 *
 */

#ifndef _MULTIDATAMESSAGE_H
#define _MULTIDATAMESSAGE_H

#include <list>

#include "core/SendMessage.h"
#include "core/VariableTypeData.h"

namespace Phoenix
{
    namespace Core
    {
        /*! \brief Superclass for all list messages.
         *
         *  This class defines a MultiDataMessage, which is any message that contains a list
         *  of any type field (ConfigMessage and CommandMessage are derivatives).  It is
         *  implemented as a template class.
         *
         */
        class MultiDataMessage : public SendMessage
        {
        public:
            /*! \brief Destructor for MultiDataMessage. */
            virtual ~MultiDataMessage(void );

            /*! \brief Returns the type of this message.
             *
             *  \return Type of this message.
             *
             */
            virtual MessageTypeEnum GetType(void ) const = 0;

            /*! \brief Returns the parameters held by this message.
             *
             *  \return Parameters in this message
             *
             */
            const std::list<VariableTypeData*> & GetParameters(void) const;

            /*! \brief Adds to the parameters held by this message.
             *
             *  \param newParam Parameter to be added to this message.
             *
             *  \return Whether the operation was successful.
             *
             */
            bool AddParameter(const VariableTypeData & newParam);

            /*! \brief Sets the parameters held by this message.
             *
             *  \param newParams Parameters to be assigned to this message.
             *
             *  \return Whether the operation was successful.
             *
             */
            bool SetParameters(const std::list<VariableTypeData*> & newParams);

            /*! \brief Sets the parameters held by this message from a buffer.
             *
             *  \note Buffer must start from the "length" field.
             *
             *  \param buffer Buffer in which the new parameters resides.
             *  \param size Size of the buffer.
             *
             *  \return Whether the operation was successful.
             *
             */
            bool SetParameters(uint8 * buffer, std::size_t size);

            /*! \brief Clears all of the parameters in the list.
             *
             *  \return Whether the operation was successful.
             *
             */
            void ClearParameters(void );

            /*! \brief Checks if the parameters of two MultiDataMessages are the same.
             *
             *  \param check Object to compare
             *
             *  \return Whether the parameters of these two objects are equal
             *
             */
            bool ParametersEqual (const MultiDataMessage& check) const;

            /*! \brief Equals Operator for MultiDataMessage.
             *
             *  \param check Message pointer of object to compare (polymorphic).
             *
             *  \return Whether this object is equal to the object
             *  pointed to by check.
             *
             */
            bool operator ==(Message * check) const;

            /*! \brief Assignment Operator for MultiDataMessage.
             *
             *  \param source Object to be copied.
             *
             *  \return Reference to the calling object.
             *
             */
            MultiDataMessage& operator=(const MultiDataMessage& source);

            /*! \brief Returns a pointer to a new MultiDataMessage object that is a copy of the calling object.
             *
             *  \param source Object to be copied.
             *
             *  \return A pointer to a new MultiDataMessage object that is a copy of the calling object.
             *
             */
            virtual Message * Duplicate(void ) const = 0;

            /*! \brief Returns the minimum buffer size required by Flatten( ).
             *
             *  \return Minimum Flatten( ) buffer size.
             *
             */
            std::size_t GetFlattenSize(void ) const ;

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
            /*! \brief Iterator typedef to simplify looping through parameters */
            typedef std::list<VariableTypeData*>::const_iterator IteratorType;

            /*! \brief Parameters held by this message */
            std::list<VariableTypeData*> parameters;
        protected:
            /*! \brief Default constructor for MultiDataMessage.
             *
             *  \note Sets the opcode to 0, and initializes parameters to an empty list.
             *
             */
            MultiDataMessage();

            /*! \brief Constructor for MultiDataMessage with MessageCodeType parameter.
             *
             *  \note Initializes parameters to an empty list.
             *
             *  \param opcodeIn Opcode of the message.
             *
             */
            MultiDataMessage(MessageCodeType opcodeIn);

            MultiDataMessage(MessageCodeType opcodeIn, const VariableTypeData & parameterIn);

            /*! \brief Constructor for MultiDataMessage with MessageCodeType and list<VariableTypeData*> parameter.
             *
             *  \param opcodeIn Opcode of the message.
             *  \param dataIn Parameters of the message.
             *
             */
            MultiDataMessage(MessageCodeType opcodeIn, const std::list<VariableTypeData*> & parametersIn);

            /*! \brief Copy Constructor for MultiDataMessage.
             *
             *  \param source Object to be copied.
             *
             */
            MultiDataMessage(const MultiDataMessage& source);
        };
    }
}

#endif  //_MULTIDATAMESSAGE_H
