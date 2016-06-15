/*! \file ErrorMessage.cpp
 *  \brief Implementation File for the ErrorMessage Class.
 *
 *  This class defines an ErrorMessage, a MultiDataMessage meant to communicate
 *  an error.
 *
 */

#include "core/ErrorMessage.h"

namespace Phoenix
{
    namespace Core
    {
		ErrorMessage::ErrorMessage()
					 :MultiDataMessage()
		{
			// Intentionally left empty
		}

        ErrorMessage::ErrorMessage(MessageCodeType opcodeIn)
					 :MultiDataMessage(opcodeIn)
        {
        	// Intentionally left empty
        }

        ErrorMessage::ErrorMessage(MessageCodeType opcodeIn, const VariableTypeData & parameterIn)
                             :MultiDataMessage(opcodeIn, parameterIn)
		{
			// Intentionally left empty
		}

        ErrorMessage::ErrorMessage(MessageCodeType opcodeIn, const std::list<VariableTypeData*> & parametersIn)
                     :MultiDataMessage(opcodeIn, parametersIn)
        {
        	// Intentionally left empty
        }

        ErrorMessage::ErrorMessage(const ErrorMessage & source)
                     :MultiDataMessage(source)
        {
        	// Intentionally left empty
        }

        ErrorMessage::~ErrorMessage(void )
        {
        	// Intentionally left empty
        }

        MessageTypeEnum ErrorMessage::GetType() const
		{
        	return MESSAGE_TYPE_ERROR;
		}

        ErrorMessage & ErrorMessage::operator=(const ErrorMessage & source)
		{
        	// Make sure this isn't the same as source
			if (this == &source)
			{
				return *this;
			}

			MultiDataMessage::operator=(source);

			return *this;
		}

        Message * ErrorMessage::Duplicate(void ) const
		{
			return new ErrorMessage(*this);
		}
    }
}
