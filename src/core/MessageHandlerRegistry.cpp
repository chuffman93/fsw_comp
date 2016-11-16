//
//
//  Generated by StarUML(tm) C++ Add-In
//
//  @ Project : AllStar
//  @ File Name : MessageHandlerRegistry.cpp
//  @ Date : 2/9/2010
//  @ Author : 
//
//


#include "core/MessageHandlerRegistry.h"
#include "core/StdTypes.h"
#include <cstdio>

//#include "boards/backplane/dbg_led.h"

using namespace AllStar::Servers;

namespace AllStar{
namespace Core{

MessageHandlerRegistry::MessageHandlerRegistry(void )
{

}

MessageHandlerRegistry::MessageHandlerRegistry(const MessageHandlerRegistry & source)
{

}

MessageHandlerRegistry::~MessageHandlerRegistry(void )
{

}

MessageHandlerRegistry & MessageHandlerRegistry::operator=(const MessageHandlerRegistry source)
{
	return *this;
}

bool MessageHandlerRegistry::RegisterHandler(const MessageIdentifierType & key, MessageHandler * handler)
{
	handlerMap.insert(make_pair(key, handler));
	return true;
}

ACPPacket * MessageHandlerRegistry::Invoke(const ACPPacket & packet)
{
	//Message * msg = packet.GetMessagePtr();
	std::map<MessageIdentifierType, MessageHandler*>::iterator it =
			handlerMap.find(MessageIdentifierType(packet.getDestination(), packet.getOpcode()));

	if (it == handlerMap.end() || it->second == NULL)
	{
		ACPPacket * ret = new ACPPacket(ERROR_OPCODE_UNRECOGNIZED_TYPE, false);
		return ret;
	}
	else
	{
		return it->second->Handle(packet);
	}
}

}
}
