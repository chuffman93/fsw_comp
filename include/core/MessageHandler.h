//
//
//  Generated by StarUML(tm) C++ Add-In
//
//  @ Project : Phoenix
//  @ File Name : MessageHandler.h
//  @ Date : 2/9/2010
//  @ Author :
//
//


#ifndef _MESSAGEHANDLER_H
#define _MESSAGEHANDLER_H

#include "core/StdTypes.h"
#include "core/Packet.h"
#include "core/ReturnMessage.h"

#include <string>

#include "servers/SubsystemServer.h"

namespace Phoenix
{
    namespace Core
    {
        class MessageHandler
        {
        public:
            MessageHandler(void );
            MessageHandler(std::string nameIn);
            MessageHandler(const MessageHandler & source);
            virtual ~MessageHandler(void );
            MessageHandler & operator=(const MessageHandler & source);
            std::string GetName() const;
            void SetName(std::string newName);
            virtual ReturnMessage * Handle(const Packet & packet) = 0;
        private:
            std::string name;
        };
    }
}

#endif  //_MESSAGEHANDLER_H
