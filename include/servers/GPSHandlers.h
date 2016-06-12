/*
 * GPSHandlers.h
 *
 *  Created: 7/3/12
 *  Written by: Conrad Hougen
 */

#ifndef GPSHANDLERS_H_
#define	GPSHANDLERS_H_

#include "core/MessageHandler.h"

class GPSHealthAndStatusHandler: public Phoenix::Core::MessageHandler
{
public:
		/*! \brief Handle a request for H&S measurements.
		 *
		 *  \param packet FSWPacket containing the message.
		 *  \return ReturnMessage that serves as a response.
		 */
		Phoenix::Core::FSWPacket * Handle(const Phoenix::Core::FSWPacket & packet);
};

class GPSTimeHandler: public Phoenix::Core::MessageHandler
{
public:
		/*! \brief Handle a request for date/time data.
		 *
		 *  \param packet FSWPacket containing the message.
		 *  \return ReturnMessage that serves as a response.
		 */
		Phoenix::Core::FSWPacket * Handle(const Phoenix::Core::FSWPacket & packet);
};

class GPSPositionHandler: public Phoenix::Core::MessageHandler
{
public:
		/*! \brief Handle a request for position data.
		 *
		 *  \param packet FSWPacket containing the message.
		 *  \return ReturnMessage that serves as a response.
		 */
		Phoenix::Core::FSWPacket * Handle(const Phoenix::Core::FSWPacket & packet);
};

class GPSResetHandler: public Phoenix::Core::MessageHandler
{
public:
		/*! \brief Handle a reset request.
		 *
		 *  \param packet FSWPacket containing the message.
		 *  \return ReturnMessage that serves as a response.
		 */
		Phoenix::Core::FSWPacket * Handle(const Phoenix::Core::FSWPacket & packet);
};

class GPSErrorHandler: public Phoenix::Core::MessageHandler
{
public:
		/*! \brief Handle an error.
		 *
		 *  \param packet FSWPacket containing the message.
		 *  \return ReturnMessage that serves as a response.
		 */
		Phoenix::Core::FSWPacket * Handle(const Phoenix::Core::FSWPacket & packet);
};

#endif /* GPSHANDLERS_H */
