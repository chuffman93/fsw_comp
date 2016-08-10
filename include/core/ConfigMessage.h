/*! \file ConfigMessage.h
 *  \brief Header File for the ConfigMessage Class.
 *
 *  This class defines a ConfigMessage, a MultiDataMessage meant to communicate
 *  configuration data (settings).
 *
 */


#ifndef _CONFIGMESSAGE_H
#define _CONFIGMESSAGE_H

#include "core/MultiDataMessage.h"

namespace AllStar
{
    namespace Core
    {
        /*! \brief ConfigMessage Class
         *
         *  This class defines a ConfigMessage, a MultiDataMessage meant to communicate
         *  configuration data (settings).
         *
         */
        class ConfigMessage : public MultiDataMessage
        {
        public:
            /*! \brief Default constructor for ConfigMessage.
             *
             *  \note Sets the opcode to 0, and initializes parameters to an empty list.
             *
             */
            ConfigMessage();

            /*! \brief Constructor for ConfigMessage with MessageCodeType parameter.
             *
             *  \note Initializes parameters to an empty list
             *
             *  \param opcodeIn Opcode of the message.
             *
             */
            ConfigMessage(MessageCodeType opcodeIn);

            ConfigMessage(MessageCodeType opcodeIn, const VariableTypeData & parameterIn);

            /*! \brief Constructor for ConfigMessage with MessageCodeType and list<Setting *> parameter.
             *
             *  \param opcodeIn Opcode of the message.
             *  \param settingsIn Parameters of the message.
             *
             *
             */
            ConfigMessage(MessageCodeType opcodeIn, const std::list<VariableTypeData*> & parametersIn);

            /*! \brief Copy Constructor for ConfigMessage.
             *
             *  \param source Object to be copied.
             *
             */
            ConfigMessage(const ConfigMessage & source);

            /*! \brief Destructor for ConfigMessage. */
            virtual ~ConfigMessage(void );

            /*! \brief Returns the type of this message.
             *
             *  \return Type of this message.
             *
             */
            virtual MessageTypeEnum GetType() const;

            /*! \brief Assignment Operator for ConfigMessage.
             *
             *  \param source Object to be copied.
             *
             *  \return Reference to the calling object.
             *
             */
            ConfigMessage & operator=(const ConfigMessage & source);

            /*! \brief Returns a pointer to a new ConfigMessage object that is a copy of the calling object.
             *
             *  \param source Object to be copied.
             *
             *  \return A pointer to a new ConfigMessage object that is a copy of the calling object.
             *
             */
            virtual Message * Duplicate(void ) const;
        };
    }
}

#endif  //_CONFIGMESSAGE_H
