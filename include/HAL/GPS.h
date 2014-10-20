/*! \file GPS.h
 *  \brief Header file for the GPS interface
 *
 *  This file contains the definition for the HAL GPS interface.
 */

#ifndef _GPS_H_
#define _GPS_H_

#include "core/StdTypes.h"

namespace Phoenix
{
    namespace HAL
    {
		enum messageType
		{
            LOG = 6,
			COMMAND
        };

        enum gpsCommand
        {
	        TIME = 0,
	        POSITION,
			RXSTATUS,
			COMMANDTPYE
        };

        /*! \brief Initialize the GPS interface
         *
         *  Initializes the GPS interface.
         *
         *  \return true if successfully initialized, false otherwise.
         */
        bool GPSInit(void);

        /*! \brief Get data logs from GPS
         *
         *  Gets the data log corresponding to the input parameter
         *
         *  \param log Data log to retrieve from GPS
         *  \return true if states successfully read from GPS hardware.
         */
        bool GetGPSData(uint8 messageType, uint8 gpsCommand, uint8 *buffer,uint32 * input_size);


    }
}

#endif /* _GPS_H_ */
