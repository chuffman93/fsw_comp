/*! \file DataMessage.cpp
 *  \brief Implementation File for the DataMessage Class.
 *
 *  This class defines a DataMessage, a SingleDataMessage meant to communicate
 *  a piece of data.
 *
 */

#include "core/DataMessage.h"

namespace Phoenix
{
    namespace Core
    {
		DataMessage::DataMessage()
					:MultiDataMessage()
		{
			// Intentionally left empty
		}

       	DataMessage::DataMessage(MessageCodeType opcodeIn)
                    :MultiDataMessage(opcodeIn)
        {
       		// Intentionally left empty
        }

       	DataMessage::DataMessage(MessageCodeType opcodeIn, const VariableTypeData & parameterIn)
       	                             :MultiDataMessage(opcodeIn, parameterIn)
		{
			// Intentionally left empty
		}
        
        DataMessage::DataMessage(MessageCodeType opcodeIn, const std::list<VariableTypeData *> & parametersIn)
                    :MultiDataMessage(opcodeIn, parametersIn)
        {
        	// Intentionally left empty
        }

        DataMessage::DataMessage(const DataMessage & source)
                    :MultiDataMessage(source)
        {
        	// Intentionally left empty
        }
        
        DataMessage::~DataMessage(void )
        {
        	// Intentionally left empty
        }
        
        MessageTypeEnum DataMessage::GetType() const
		{
        	return MESSAGE_TYPE_DATA;
		}

        DataMessage & DataMessage::operator=(const DataMessage & source)
		{
        	// Make sure this isn't the same as source
			if (this == &source)
			{
				return *this;
			}

			MultiDataMessage::operator=(source);

			return *this;
		}

        Message * DataMessage::Duplicate(void ) const
        {
        	return new DataMessage(*this);
        }
    }
}
