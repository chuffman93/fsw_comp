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
        *  \param packet Packet containing the message.
        *  \return ReturnMessage that serves as a response.
        */
    Phoenix::Core::ReturnMessage * Handle(const Phoenix::Core::Packet & packet);
};

class EPSStateofChargeHandler : public Phoenix::Core::MessageHandler
{
public:
	/* \brief Handle a request for state of charge data
	 *
	 * \param packet Packet containing the message
	 * \return ReturnMessage that serves as a response
	 */

	Phoenix::Core::ReturnMessage * Handle(const Phoenix::Core::Packet & packet);
};

class EPSPowerCycleHandler : public Phoenix::Core::MessageHandler
{
public:
        /*! \brief Handle the power cycle command.
         *
         *  \param packet Packet containing the message.
         *  \return ReturnMessage that serves as a response.
         */
        Phoenix::Core::ReturnMessage * Handle(const Phoenix::Core::Packet & packet);
};

class EPSDisableOCHandler : public Phoenix::Core::MessageHandler
{
public:
        /*! \brief Handle a disable overcurrent protection command.
         *
         *  \param packet Packet containing the message.
         *  \return ReturnMessage that serves as a response.
         */
        Phoenix::Core::ReturnMessage * Handle(const Phoenix::Core::Packet & packet);
};

class EPSEnableOCHandler : public Phoenix::Core::MessageHandler
{
public:
        /*! \brief Handle a enable overcurrent protection command.
         *
         *  \param packet Packet containing the message.
         *  \return ReturnMessage that serves as a response.
         */
        Phoenix::Core::ReturnMessage * Handle(const Phoenix::Core::Packet & packet);
};

class EPSErrorHandler : public Phoenix::Core::MessageHandler
{
public:
        /*! \brief Handle an error generated on EPS.
         *
         *  \param packet Packet containing the message.
         *  \return ReturnMessage that serves as a response.
         */
        Phoenix::Core::ReturnMessage * Handle(const Phoenix::Core::Packet & packet);
};

class EPSPowerHandler : public Phoenix::Core::MessageHandler
{
public:
	/*! \brief Handle a subsystem power off request
	 *
	 * \param packet Packet containing a message
	 * \return ReturnMessage that serves as a respone
	 */
	Phoenix::Core::ReturnMessage * Handle(const Phoenix::Core::Packet & packet);
private:
	const static uint32 numParams = 3;
	static uint32 enumArray[numParams];
};

#endif /* EPSHANDLERS_H_ */

