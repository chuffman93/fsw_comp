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
class EPSHSHandler : public AllStar::Core::MessageHandler
{
public:
    /*! \brief Handle a request for power data.
	*
	*  \param packet FSWPacket containing the message.
	*  \return ReturnMessage that serves as a response.
	*/
    AllStar::Core::FSWPacket * Handle(const AllStar::Core::FSWPacket & packet);
};

class EPSStateofChargeHandler : public AllStar::Core::MessageHandler
{
public:
	/* \brief Handle a request for state of charge data
	 *
	 * \param packet FSWPacket containing the message
	 * \return ReturnMessage that serves as a response
	 */

	AllStar::Core::FSWPacket * Handle(const AllStar::Core::FSWPacket & packet);
};

class EPSPowerCycleHandler : public AllStar::Core::MessageHandler
{
public:
        /*! \brief Handle the power cycle command.
         *
         *  \param packet FSWPacket containing the message.
         *  \return ReturnMessage that serves as a response.
         */
        AllStar::Core::FSWPacket * Handle(const AllStar::Core::FSWPacket & packet);
};

class EPSDisableOCHandler : public AllStar::Core::MessageHandler
{
public:
        /*! \brief Handle a disable overcurrent protection command.
         *
         *  \param packet FSWPacket containing the message.
         *  \return ReturnMessage that serves as a response.
         */
        AllStar::Core::FSWPacket * Handle(const AllStar::Core::FSWPacket & packet);
};

class EPSEnableOCHandler : public AllStar::Core::MessageHandler
{
public:
        /*! \brief Handle a enable overcurrent protection command.
         *
         *  \param packet FSWPacket containing the message.
         *  \return ReturnMessage that serves as a response.
         */
        AllStar::Core::FSWPacket * Handle(const AllStar::Core::FSWPacket & packet);
};

class EPSErrorHandler : public AllStar::Core::MessageHandler
{
public:
        /*! \brief Handle an error generated on EPS.
         *
         *  \param packet FSWPacket containing the message.
         *  \return ReturnMessage that serves as a response.
         */
        AllStar::Core::FSWPacket * Handle(const AllStar::Core::FSWPacket & packet);
};

class EPSPowerHandler : public AllStar::Core::MessageHandler
{
public:
	/*! \brief Handle a subsystem power off request
	 *
	 * \param packet FSWPacket containing a message
	 * \return ReturnMessage that serves as a respone
	 */
	AllStar::Core::FSWPacket * Handle(const AllStar::Core::FSWPacket & packet);
private:
	const static uint32 numParams = 3;
	static uint32 enumArray[numParams];
};

#endif /* EPSHANDLERS_H_ */

