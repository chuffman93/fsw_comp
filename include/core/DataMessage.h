/*! \file DataMessage.h
 *  \brief Data File for the ErrorMessage Class.
 *
 *  This class defines a DataMessage, a SingleDataMessage meant to communicate
 *  a piece of data.
 *
 */


#ifndef _DATAMESSAGE_H
#define _DATAMESSAGE_H

#include "core/MultiDataMessage.h"

namespace AllStar
{
    namespace Core
    {
		/*! \brief DataMessage Class
		 *
		 *  This class defines a DataMessage, a SingleDataMessage meant to communicate
		 *  a piece of data.
		 *
		 */
        class DataMessage : public MultiDataMessage
        {
        public:
        	/*! \brief Default constructor for DataMessage.
			 *
			 *  \note Sets the opcode to 0, and uses default VariableTypeData constructor.
			 *
			 */
        	DataMessage();

        	/*! \brief Constructor for DataMessage with MessageCodeType parameter.
			 *
			 *  \note Uses default VariableTypeData constructor.
			 *
			 *  \param opcodeIn Opcode of the message.
			 *
			 */
            DataMessage(MessageCodeType opcodeIn);

            DataMessage(MessageCodeType opcodeIn, const VariableTypeData & parameterIn);

            /*! \brief Constructor for DataMessage with MessageCodeType and VariableTypeData parameter.
			 *
			 *  \param opcodeIn Opcode of the message.
			 *  \param dataIn Data of the message.
			 *
			 */
            DataMessage(MessageCodeType opcodeIn, const std::list<VariableTypeData *> & parametersIn);

            /*! \brief Copy Constructor for DataMessage.
			 *
			 *  \param source Object to be copied.
			 *
			 */
            DataMessage(const DataMessage & source);

            /*! \brief Destructor for DataMessage. */
            virtual ~DataMessage(void );

            /*! \brief Returns the type of this message.
			 *
			 *  \return Type of this message.
			 *
			 */
            virtual MessageTypeEnum GetType() const;

            /*! \brief Assignment Operator for DataMessage.
			 *
			 *  \param source Object to be copied.
			 *
			 *  \return Reference to the calling object.
			 *
			 */
            DataMessage & operator=(const DataMessage & source);

            /*! \brief Returns a pointer to a new DataMessage object that is a copy of the calling object.
			 *
			 *  \param source Object to be copied.
			 *
			 *  \return A pointer to a new DataMessage object that is a copy of the calling object.
			 *
			 */
			virtual Message * Duplicate(void ) const;
        };
    }
}

#endif  //_DATAMESSAGE_H
