//
//
//  Generated by StarUML(tm) C++ Add-In
//
//  @ Project : AllStar
//  @ File Name : Server.cpp
//  @ Date : 2/9/2010
//  @ Author :
//
//

#include "servers/Server.h"

namespace AllStar
{
    namespace Servers
    {
        Server::Server(std::string nameIn, LocationIDType idIn)
               : name(nameIn), id(idIn)
        {

        }

        Server::~Server(void )
        {

        }

        Server & Server::operator=(const Server & source)
        {
        	if (this == &source)
        	{
        		return *this;
        	}

        	name = source.name;
        	id = source.id;

        	return *this;
        }

        std::string Server::GetName(void ) const
        {
        	return name;
        }

        LocationIDType Server::GetID(void ) const
        {
        	return id;
        }
    }
}
