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


class PLDHSHandler : public Phoenix::Core::MessageHandler
{
public:
        /* \brief Handle a health and status request.
         *
         *  \param packet: Packet containing the command.
         *  \return: ReturnMessage from PLD hardware.
         */
		Phoenix::Core::ReturnMessage * Handle(const Phoenix::Core::Packet & packet);
};

class PLDPictureTakeHandler : public Phoenix::Core::MessageHandler
{
public:
        /* \brief Handle a take picture request.
         *
         *  \param packet: Packet containing the command.
         *  \return: ReturnMessage from PLD hardware.
         */
		Phoenix::Core::ReturnMessage * Handle(const Phoenix::Core::Packet & packet);
};

class PLDGetPictureHandler : public Phoenix::Core::MessageHandler
{
public:
        /* \brief Handle a get picture request.
         *
         *  \param packet: Packet containing the command.
         *  \return: ReturnMessage from PLD hardware.
         */
		Phoenix::Core::ReturnMessage * Handle(const Phoenix::Core::Packet & packet);
};
#ifndef EDR_TEST

class PLDDataHandler : public Phoenix::Core::MessageHandler
{
public:
        /* \brief Handle a data request.
         *
         *  \param packet: Packet containing the command.
         *  \return: ReturnMessage from PLD hardware.
         */
		Phoenix::Core::ReturnMessage * Handle(const Phoenix::Core::Packet & packet);
private:
		const static uint32 numParams = 1;
		static uint32 enumArray[numParams];
};

#endif

class PLDResSetHandler : public Phoenix::Core::MessageHandler
{
public:
        /* \brief Handle a picture resolution resize request.
         *
         *  \param packet: Packet containing the command.
         *  \return: ReturnMessage from PLD hardware.
         */
		Phoenix::Core::ReturnMessage * Handle(const Phoenix::Core::Packet & packet);
private:
		const static uint32 numParams = 2;
		static uint32 enumArray[numParams];
};

class PLDSetChunkSizeHandler : public Phoenix::Core::MessageHandler
{
public:
        /* \brief Handle a picture chunk size request.
         * sets the size of the data packet coming back.
         *
         *  \param packet: Packet containing the command.
         *  \return: ReturnMessage from PLD hardware.
         */
		Phoenix::Core::ReturnMessage * Handle(const Phoenix::Core::Packet & packet);
private:
		const static uint32 numParams = 1;
		static uint32 enumArray[numParams];
};

class PLDSetGainHandler : public Phoenix::Core::MessageHandler
{
public:
        /* \brief Handle a gain change request.
         *
         *  \param packet: Packet containing the command.
         *  \return: ReturnMessage from PLD hardware.
         */
		Phoenix::Core::ReturnMessage * Handle(const Phoenix::Core::Packet & packet);
private:
		const static uint32 numParams = 1;
		static uint32 enumArray[numParams];
};

class PLDSetExpTimeHandler : public Phoenix::Core::MessageHandler
{
public:
        /* \brief Handle a exposure time change request.
         *
         *  \param packet: Packet containing the command.
         *  \return: ReturnMessage from PLD hardware.
         */
		Phoenix::Core::ReturnMessage * Handle(const Phoenix::Core::Packet & packet);
private:
		const static uint32 numParams = 1;
		static uint32 enumArray[numParams];
};

class PLDResetHandler : public Phoenix::Core::MessageHandler
{
public:
        /* \brief Handle a reset request.
         *
         *  \param packet: Packet containing the command.
         *  \return: ReturnMessage from PLD hardware.
         */
		Phoenix::Core::ReturnMessage * Handle(const Phoenix::Core::Packet & packet);
};

class PLDErrorHandler : public Phoenix::Core::MessageHandler
{
public:
        /* \brief Handle an error.
         *
         *  \param packet: Packet containing the command.
         *  \return: ReturnMessage from PLD hardware.
         */
		Phoenix::Core::ReturnMessage * Handle(const Phoenix::Core::Packet & packet);
};

#ifdef EDR_TEST

class PLDDataHandler : public Phoenix::Core::MessageHandler
{
public:
        /* \brief Handle data retrieval.
         *
         *  \param packet: Packet containing the command.
         *  \return: ReturnMessage from PLD hardware.
         */
		Phoenix::Core::ReturnMessage * Handle(const Phoenix::Core::Packet & packet);
};

#endif

#endif /* PLDHANDLERS_H_ */
