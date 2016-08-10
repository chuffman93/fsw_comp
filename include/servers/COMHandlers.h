/*
 * COMHandlers.h
 *
 *  Created on: Oct 6, 2012
 *      Author: Brian Campuzano
 *
 */

#ifndef COMHANDLERS_H_
#define COMHANDLERS_H_

#include "core/MessageHandler.h"

/* Message handlers for the COM server.
 * Any time a message is sent(from Hardware or another Server)
 * to COM it should execute one of these handlers.
 */


class COMHSHandler : public AllStar::Core::MessageHandler
{
public:
        /* \brief Handle a health and status request.
         *
         *  \param packet FSWPacket containing the command.
         *  \return ReturnMessage from COM hardware.
         */
		AllStar::Core::FSWPacket * Handle(const AllStar::Core::FSWPacket & packet);
};

class COMBeaconHandler : public AllStar::Core::MessageHandler
{
public:
        /* \brief Handle a beacon command.
         *
         *  \param packet FSWPacket containing the command.
         *  \return ReturnMessage from COM hardware.
         */
		AllStar::Core::FSWPacket * Handle(const AllStar::Core::FSWPacket & packet);
};


class COMDataSendHandler : public AllStar::Core::MessageHandler
{
public:
        /* \brief Handle a packet send request.
         *
         *  \param packet FSWPacket containing the command.
         *  \return ReturnMessage from COM hardware.
         */
		AllStar::Core::FSWPacket * Handle(const AllStar::Core::FSWPacket & packet);
private:
		const static uint32 numParams = 1;
		static uint32 enumArray[numParams];
};

class COMLoginHandler : public AllStar::Core::MessageHandler
{
public:
        /* \brief Handle a login command.
         *
         *  \param packet FSWPacket containing the command.
         *  \return ReturnMessage from COM hardware.
         */
		AllStar::Core::FSWPacket * Handle(const AllStar::Core::FSWPacket & packet);
};

class COMLogoutHandler : public AllStar::Core::MessageHandler
{
public:
        /* \brief Handle a logout command.
         *
         *  \param packet FSWPacket containing the command.
         *  \return ReturnMessage from COM hardware.
         */
		AllStar::Core::FSWPacket * Handle(const AllStar::Core::FSWPacket & packet);
};

class COMResetHandler : public AllStar::Core::MessageHandler
{
public:
        /* \brief Handle a reset command.
         *
         *  \param packet FSWPacket containing the command.
         *  \return ReturnMessage from COM hardware.
         */
		AllStar::Core::FSWPacket * Handle(const AllStar::Core::FSWPacket & packet);
};

class COMDataReceiveHandler : public AllStar::Core::MessageHandler
{
public:
        /* \brief Handle a reset command.
         *
         *  \param packet FSWPacket containing the command.
         *  \return ReturnMessage from COM hardware.
         */
		AllStar::Core::FSWPacket * Handle(const AllStar::Core::FSWPacket & packet);
private:
		const static uint32 numParams = 1;
		static uint32 enumArray[numParams];
};

class COMErrorHandler : public AllStar::Core::MessageHandler
{
public:
        /* \brief Handle an error.
         *
         *  \param packet FSWPacket containing the command.
         *  \return ReturnMessage from COM hardware.
         */
		AllStar::Core::FSWPacket * Handle(const AllStar::Core::FSWPacket & packet);
};

#endif  //COMHANDLERS_H_
