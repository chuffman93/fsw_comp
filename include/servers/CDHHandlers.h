/*
 * CDHHandlers.h
 *
 *  Created on: Mar 8, 2016
 *      Author: Alex St. Clair
 */

#ifndef CDHHANDLERS_H_
#define CDHHANDLERS_H_

#include "core/MessageHandler.h"
#include "core/ReturnMessage.h"
#include "core/FSWPacket.h"
#include "core/VariableTypeEnum.h"

/* Message handlers for the CDH server.
 * Any time a message is sent to CDH it should
 * execute one of these handlers.
 */


class CDHCPUUsageHandler : public Phoenix::Core::MessageHandler
{
public:
        /*! \brief Handle a request for CDH CPU usage.
         *
         *  \param packet FSWPacket containing the message.
         *  \return ReturnMessage that serves as a response.
         */
        Phoenix::Core::ReturnMessage * Handle(const Phoenix::Core::FSWPacket & packet);
};

class CDHMemUsageHandler : public Phoenix::Core::MessageHandler
{
public:
        /*! \brief Handle a request for CDH memory usage.
         *
         *  \param packet FSWPacket containing the message.
         *  \return ReturnMessage that serves as a response.
         */
        Phoenix::Core::ReturnMessage * Handle(const Phoenix::Core::FSWPacket & packet);
};

class CDHStorageHandler : public Phoenix::Core::MessageHandler
{
public:
        /*! \brief Handle a request for CDH processor usage.
         *
         *  \param packet FSWPacket containing the message.
         *  \return ReturnMessage that serves as a response.
         */
        Phoenix::Core::ReturnMessage * Handle(const Phoenix::Core::FSWPacket & packet);
};

class CDHTempStartHandler : public Phoenix::Core::MessageHandler
{
public:
        /*! \brief Handle a request for temperature bus starting.
         *
         *  \param packet FSWPacket containing the message.
         *  \return ReturnMessage that serves as a response.
         */
        Phoenix::Core::ReturnMessage * Handle(const Phoenix::Core::FSWPacket & packet);
};

class CDHTempReadHandler : public Phoenix::Core::MessageHandler
{
public:
        /*! \brief Handle a request for temperature bus readings.
         *
         *  \param packet FSWPacket containing the message.
         *  \return ReturnMessage that serves as a response.
         */
        Phoenix::Core::ReturnMessage * Handle(const Phoenix::Core::FSWPacket & packet);
};

class CDHHotSwapsHandler : public Phoenix::Core::MessageHandler
{
public:
        /*! \brief Handle a request for hot swap readings.
         *
         *  \param packet FSWPacket containing the message.
         *  \return ReturnMessage that serves as a response.
         */
        Phoenix::Core::ReturnMessage * Handle(const Phoenix::Core::FSWPacket & packet);
};

#endif /* CDHHANDLERS_H_ */
