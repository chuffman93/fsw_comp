///*! \file ConfigMessage.cpp
// *  \brief Implementation File for the ConfigMessage Class.
// *
// *  This class defines a ConfigMessage, a MultiDataMessage meant to communicate
// *  configuration data (settings).
// *
// */
//
//#include "core/ConfigMessage.h"
//
//using namespace std;
//
//namespace Phoenix
//{
//    namespace Core
//    {
//        ConfigMessage::ConfigMessage()
//        : MultiDataMessage()
//        {
//            // Intentionally left empty
//        }
//
//        ConfigMessage::ConfigMessage(MessageCodeType opcodeIn)
//        : MultiDataMessage(opcodeIn)
//        {
//            // Intentionally left empty
//        }
//
//        ConfigMessage::ConfigMessage(MessageCodeType opcodeIn, const VariableTypeData & parameterIn)
//        :MultiDataMessage(opcodeIn, parameterIn)
//        {
//            // Intentionally left empty
//        }
//
//        ConfigMessage::ConfigMessage(MessageCodeType opcodeIn, const std::list<VariableTypeData*> & parametersIn)
//        : MultiDataMessage(opcodeIn, parametersIn)
//        {
//            // Intentionally left empty
//        }
//
//        ConfigMessage::ConfigMessage(const ConfigMessage & source)
//        : MultiDataMessage(source)
//        {
//            // Intentionally left empty
//        }
//
//        ConfigMessage::~ConfigMessage(void )
//        {
//            // Intentionally left empty
//        }
//
//        MessageTypeEnum ConfigMessage::GetType() const
//        {
//            return MESSAGE_TYPE_CONFIG;
//        }
//
//        ConfigMessage & ConfigMessage::operator=(const ConfigMessage & source)
//        {
//            // Make sure this isn't the same as source
//            if (this == &source)
//            {
//                return *this;
//            }
//
//            MultiDataMessage::operator=(source);
//
//            return *this;
//        }
//
//        Message * ConfigMessage::Duplicate(void ) const
//        {
//            return new ConfigMessage(*this);
//        }
//    }
//}
