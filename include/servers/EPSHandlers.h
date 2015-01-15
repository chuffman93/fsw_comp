/*
 * EPSHandlers.h
 *
 *  Created on: Apr 29, 2011
 *      Author: Michael
 *  Updated On: 10/26/2012
 *  	Author: Gabrielle Massone
 */

#ifndef EPSHANDLERS_H_
#define EPSHANDLERS_H_

#include "core/MessageHandler.h"

// Split measurement handler into specific measurements
class EPSHSHandler : public Phoenix::Core::MessageHandler
{
public:
    /*! \brief Handle a request for power data.
        *
        *  \param packet FSWPacket containing the message.
        *  \return ReturnMessage that serves as a response.
        */
    Phoenix::Core::ReturnMessage * Handle(const Phoenix::Core::FSWPacket & packet);
};

class EPSStateofChargeHandler : public Phoenix::Core::MessageHandler
{
public:
	/* \brief Handle a request for state of charge data
	 *
	 * \param packet FSWPacket containing the message
	 * \return ReturnMessage that serves as a response
	 */

	Phoenix::Core::ReturnMessage * Handle(const Phoenix::Core::FSWPacket & packet);
};

class EPSPowerCycleHandler : public Phoenix::Core::MessageHandler
{
public:
        /*! \brief Handle the power cycle command.
         *
         *  \param packet FSWPacket containing the message.
         *  \return ReturnMessage that serves as a response.
         */
        Phoenix::Core::ReturnMessage * Handle(const Phoenix::Core::FSWPacket & packet);
};

class EPSDisableOCHandler : public Phoenix::Core::MessageHandler
{
public:
        /*! \brief Handle a disable overcurrent protection command.
         *
         *  \param packet FSWPacket containing the message.
         *  \return ReturnMessage that serves as a response.
         */
        Phoenix::Core::ReturnMessage * Handle(const Phoenix::Core::FSWPacket & packet);
};

class EPSEnableOCHandler : public Phoenix::Core::MessageHandler
{
public:
        /*! \brief Handle a enable overcurrent protection command.
         *
         *  \param packet FSWPacket containing the message.
         *  \return ReturnMessage that serves as a response.
         */
        Phoenix::Core::ReturnMessage * Handle(const Phoenix::Core::FSWPacket & packet);
};

class EPSErrorHandler : public Phoenix::Core::MessageHandler
{
public:
        /*! \brief Handle an error generated on EPS.
         *
         *  \param packet FSWPacket containing the message.
         *  \return ReturnMessage that serves as a response.
         */
        Phoenix::Core::ReturnMessage * Handle(const Phoenix::Core::FSWPacket & packet);
};

class EPSPowerHandler : public Phoenix::Core::MessageHandler
{
public:
	/*! \brief Handle a subsystem power off request
	 *
	 * \param packet FSWPacket containing a message
	 * \return ReturnMessage that serves as a respone
	 */
	Phoenix::Core::ReturnMessage * Handle(const Phoenix::Core::FSWPacket & packet);
private:
	const static uint32 numParams = 3;
	static uint32 enumArray[numParams];
};

#endif /* EPSHANDLERS_H_ */

