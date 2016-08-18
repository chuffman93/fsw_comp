//
//
//  Generated by StarUML(tm) C++ Add-In
//
//  @ Project : AllStar
//  @ File Name : Server.h
//  @ Date : 2/9/2010
//  @ Author :
//
//


#ifndef _SERVER_H
#define _SERVER_H

#include <string>

#include "core/ModeListener.h"
#include "core/StdTypes.h"

namespace AllStar
{
    namespace Servers
    {
        class Server : public AllStar::Core::ModeListener
        {
        public:
            Server(std::string nameIn, LocationIDType idIn);
            virtual ~Server(void );
            Server & operator=(const Server & source);
			virtual bool operator ==(const Server & check) const = 0;
            std::string GetName(void ) const;
            LocationIDType GetID(void ) const;
            virtual void Update(SystemModeEnum mode) = 0;
            std::string name;
            LocationIDType id;
            int startupWait;
        };
    }
}

#endif  //_SERVER_H
