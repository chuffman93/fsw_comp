/*! \file CommandMessage.cpp
 *  \brief Implementation File for the CommandMessage Class.
 *
 *  This class defines a CommandMessage, a MultiDataMessage meant to communicate
 *  command data (VariableTypeData).
 *
 */

#include "core/CommandMessage.h"

using namespace std;

namespace Phoenix
{
    namespace Core
    {
        CommandMessage::CommandMessage()
        : MultiDataMessage()
        {
            // Intentionally left empty
        }

        CommandMessage::CommandMessage(MessageCodeType opcodeIn)
        : MultiDataMessage(opcodeIn)
        {
            // Intentionally left empty
        }

        CommandMessage::CommandMessage(MessageCodeType opcodeIn, const VariableTypeData & parameterIn)
        :MultiDataMessage(opcodeIn, parameterIn)
        {
            // Intentionally left empty
        }

        CommandMessage::CommandMessage(MessageCodeType opcodeIn, const std::list<VariableTypeData *> & parametersIn)
        : MultiDataMessage(opcodeIn, parametersIn)
        {
            // Intentionally left empty
        }

        CommandMessage::CommandMessage(const CommandMessage & source)
        : MultiDataMessage(source)
        {
            // Intentionally left empty
        }

        CommandMessage::~CommandMessage(void )
        {
            //Intentionally left empty
        }

        MessageTypeEnum CommandMessage::GetType() const
        {
            return MESSAGE_TYPE_COMMAND;
        }

        CommandMessage & CommandMessage::operator=(const CommandMessage & source)
        {
            // Make sure this isn't the same as source
            if (this == &source)
            {
                return *this;
            }

            MultiDataMessage::operator=(source);

            return *this;
        }

        Message * CommandMessage::Duplicate(void ) const
        {
            return new CommandMessage(*this);
        }
    }
}
