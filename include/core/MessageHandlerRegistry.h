//
//
//  Generated by StarUML(tm) C++ Add-In
//
//  @ Project : AllStar
//  @ File Name : MessageHandlerRegistry.h
//  @ Date : 2/9/2010
//  @ Author : 
//
//


#ifndef _MESSAGEHANDLERREGISTRY_H
#define _MESSAGEHANDLERREGISTRY_H

#include <map>

#include "core/StdTypes.h"
#include "core/MessageHandler.h"
#include "servers/SubsystemServer.h"

namespace AllStar
{
    namespace Core
    {
        class MessageHandlerRegistry
        {
        public:
            MessageHandlerRegistry(void);
            MessageHandlerRegistry(const MessageHandlerRegistry & source);
            virtual ~MessageHandlerRegistry(void);
            MessageHandlerRegistry & operator=(const MessageHandlerRegistry source);
            bool RegisterHandler(const MessageIdentifierType & key, MessageHandler * handler);
            ACPPacket * Invoke(const ACPPacket & packet);
        private:
            std::map<MessageIdentifierType, MessageHandler *> handlerMap; // Local Mapping
        };
    }
}

#endif  //_MESSAGEHANDLERREGISTRY_H
