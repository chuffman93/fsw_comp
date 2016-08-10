/*! \file CommandMessage.h
 *  \brief Header File for the CommandMessage Class.
 *
 *  This class defines a CommandMessage, a MultiDataMessage meant to communicate
 *  command data (VariableTypeData).
 *
 */

#ifndef _COMMANDMESSAGE_H
#define _COMMANDMESSAGE_H

#include "core/MultiDataMessage.h"

namespace AllStar
{
    namespace Core
    {
        /*! \brief CommandMessage Class
         *
         *  This class defines a CommandMessage, a MultiDataMessage meant to communicate
         *  command data (VariableTypeData).
         *
         */
        class CommandMessage : public MultiDataMessage
        {
        public:
            /*! \brief Default constructor for CommandMessage.
             *
             *  \note Sets the opcode to 0, and initializes parameters to an empty list.
             *
             */
            CommandMessage();

            /*! \brief Constructor for CommandMessage with MessageCodeType parameter.
             *
             *  \note Initializes parameters to an empty list
             *
             *  \param opcodeIn Opcode of the message.
             *
             */
            CommandMessage(MessageCodeType opcodeIn);

            CommandMessage(MessageCodeType opcodeIn, const VariableTypeData & parameterIn);

            /*! \brief Constructor for CommandMessage with MessageCodeType and list<VariableTypeData *> parameter.
             *
             *  \param opcodeIn Opcode of the message.
             *  \param parametersIn Parameters of the message.
             *
             */
            CommandMessage(MessageCodeType opcodeIn, const std::list<VariableTypeData *> & parametersIn);

            /*! \brief Copy Constructor for CommandMessage.
             *
             *  \param source Object to be copied.
             *
             */
            CommandMessage(const CommandMessage & source);

            /*! \brief Destructor for CommandMessage. */
            ~CommandMessage(void );

            /*! \brief Returns the type of this message.
             *
             *  \return Type of this message.
             *
             */
            MessageTypeEnum GetType() const;

            /*! \brief Assignment Operator for CommandMessage.
             *
             *  \param source Object to be copied.
             *
             *  \return Reference to the calling object.
             *
             */
            CommandMessage & operator=(const CommandMessage & source);

            /*! \brief Returns a pointer to a new CommandMessage object that is a copy of the calling object.
             *
             *  \param source Object to be copied.
             *
             *  \return A pointer to a new CommandMessage object that is a copy of the calling object.
             *
             */
            Message * Duplicate(void ) const;
        };
    }
}

#endif  //_COMMANDMESSAGE_H
