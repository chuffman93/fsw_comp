/*
 * PLDHandlers.h
 *
 *  Created on: Mar 26, 2013
 *      Author: Brian Campuzano
 */

#ifndef PLDHANDLERS_H_
#define PLDHANDLERS_H_

#include "core/MessageHandler.h"
#include "servers/COMStdTasks.h"

/* Message handlers for the COM server.
 * Any time a message is sent(from Hardware or another Server)
 * to COM it should execute one of these handlers.
 */


class PLDHSHandler : public AllStar::Core::MessageHandler
{
public:
        /* \brief Handle a health and status request.
         *
         *  \param packet: FSWPacket containing the command.
         *  \return: ReturnMessage from PLD hardware.
         */
		AllStar::Core::FSWPacket * Handle(const AllStar::Core::FSWPacket & packet);
};

class PLDPictureTakeHandler : public AllStar::Core::MessageHandler
{
public:
        /* \brief Handle a take picture request.
         *
         *  \param packet: FSWPacket containing the command.
         *  \return: ReturnMessage from PLD hardware.
         */
		AllStar::Core::FSWPacket * Handle(const AllStar::Core::FSWPacket & packet);
};

class PLDGetPictureHandler : public AllStar::Core::MessageHandler
{
public:
        /* \brief Handle a get picture request.
         *
         *  \param packet: FSWPacket containing the command.
         *  \return: ReturnMessage from PLD hardware.
         */
		AllStar::Core::FSWPacket * Handle(const AllStar::Core::FSWPacket & packet);
};
#ifndef EDR_TEST

class PLDDataHandler : public AllStar::Core::MessageHandler
{
public:
        /* \brief Handle a data request.
         *
         *  \param packet: FSWPacket containing the command.
         *  \return: ReturnMessage from PLD hardware.
         */
		AllStar::Core::FSWPacket * Handle(const AllStar::Core::FSWPacket & packet);
private:
		const static uint32 numParams = 1;
		static uint32 enumArray[numParams];
};

#endif

class PLDResSetHandler : public AllStar::Core::MessageHandler
{
public:
        /* \brief Handle a picture resolution resize request.
         *
         *  \param packet: FSWPacket containing the command.
         *  \return: ReturnMessage from PLD hardware.
         */
		AllStar::Core::FSWPacket * Handle(const AllStar::Core::FSWPacket & packet);
private:
		const static uint32 numParams = 2;
		static uint32 enumArray[numParams];
};

class PLDSetChunkSizeHandler : public AllStar::Core::MessageHandler
{
public:
        /* \brief Handle a picture chunk size request.
         * sets the size of the data packet coming back.
         *
         *  \param packet: FSWPacket containing the command.
         *  \return: ReturnMessage from PLD hardware.
         */
		AllStar::Core::FSWPacket * Handle(const AllStar::Core::FSWPacket & packet);
private:
		const static uint32 numParams = 1;
		static uint32 enumArray[numParams];
};

class PLDSetGainHandler : public AllStar::Core::MessageHandler
{
public:
        /* \brief Handle a gain change request.
         *
         *  \param packet: FSWPacket containing the command.
         *  \return: ReturnMessage from PLD hardware.
         */
		AllStar::Core::FSWPacket * Handle(const AllStar::Core::FSWPacket & packet);
private:
		const static uint32 numParams = 1;
		static uint32 enumArray[numParams];
};

class PLDSetExpTimeHandler : public AllStar::Core::MessageHandler
{
public:
        /* \brief Handle a exposure time change request.
         *
         *  \param packet: FSWPacket containing the command.
         *  \return: ReturnMessage from PLD hardware.
         */
		AllStar::Core::FSWPacket * Handle(const AllStar::Core::FSWPacket & packet);
private:
		const static uint32 numParams = 1;
		static uint32 enumArray[numParams];
};

class PLDResetHandler : public AllStar::Core::MessageHandler
{
public:
        /* \brief Handle a reset request.
         *
         *  \param packet: FSWPacket containing the command.
         *  \return: ReturnMessage from PLD hardware.
         */
		AllStar::Core::FSWPacket * Handle(const AllStar::Core::FSWPacket & packet);
};

class PLDErrorHandler : public AllStar::Core::MessageHandler
{
public:
        /* \brief Handle an error.
         *
         *  \param packet: FSWPacket containing the command.
         *  \return: ReturnMessage from PLD hardware.
         */
		AllStar::Core::FSWPacket * Handle(const AllStar::Core::FSWPacket & packet);
};

#ifdef EDR_TEST

class PLDDataHandler : public AllStar::Core::MessageHandler
{
public:
        /* \brief Handle data retrieval.
         *
         *  \param packet: FSWPacket containing the command.
         *  \return: ReturnMessage from PLD hardware.
         */
		AllStar::Core::ReturnMessage * Handle(const AllStar::Core::FSWPacket & packet);
};

#endif

#endif /* PLDHANDLERS_H_ */
