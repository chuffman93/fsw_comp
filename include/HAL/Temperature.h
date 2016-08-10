/*! \file Temperature.h
 *  \brief Header file for the temperature sensor interface
 *
 *  This file contains the definition for the HAL temperature sensor interface.
 */

#ifndef _TEMPERATURE_H_
#define _TEMPERATURE_H_

#include "core/StdTypes.h"

namespace AllStar
{
    namespace HAL
    {
        /*! \brief Initialize the temperature sensor interface
         *
         *  Initializes the temperature sensor interface.
         *
         *  \note This function must be called before accessing the
         *  device for the first time.
         *
         *  \return true if device successfully initialized, false otherwise.
         */
        bool TemperatureInit(void);

        /*! \brief Convert temperature
         *
         *  Sends convert command to all temperature sensors and waits
         *  for conversion to finish.
         *
         *  \return true if command successful, false otherwise.
         */
        bool TemperatureConvert(void);

        /*! \brief Read a temperature value
         *
         *  Reads the converted temperature value from the sensor.
         *
         *  \param sensor Sensor to read from.
         *  \return true if value successfully read, false otherwise.
         */
        bool TemperatureRead(uint8 sensor, uint16 *value);
    }
}

#endif /* _TEMPERATURE_H_ */
