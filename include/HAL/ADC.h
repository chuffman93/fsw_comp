/*! \file ADC.h
 *  \brief Header file for the ADC interface
 *
 *  This file contains the definition for the HAL ADC interface.
 */

#ifndef _ADC_H_
#define _ADC_H_

#include "core/StdTypes.h"

namespace AllStar
{
    namespace HAL
    {
        /*! \brief Initialize OS elements for ADC interface
         *
         *  Creates semaphores which are used in other ADC functions.
         *
         *  \note This function must be called before any other ADC
         *  functions and may only be called exactly once.
         */
        void ADCOSInit(void);

        /*! \brief Initialize a ADC device
         *
         *  Initializes the ADC hardware.
         *
         *  \note This function must be called before accessing the
         *  device for the first time. It may be called multiple times
         *  to reinitialize the hardware with different settings.
         *
         *  \param dev Device to initialize.
         *  \return true if device successfully initialized, false otherwise.
         */
        bool ADCInit(uint8 dev);

        bool ADCReleaseMutex(uint8 dev);

        /*! \brief Sample one channel on the ADC device
         *
         *  Samples a single channel on the ADC device.
         *
         *  \param dev Device to read from.
         *  \param channel Which channel to sample.
         *  \param value Where to store result.
         *  \return true if value successfully read, false otherwise.
         */
        bool ADCSample(uint8 dev, uint8 channel, uint16 *value);

        /*! \brief Sample all channels on the ADC device
         *
         *  Samples all channels on the ADC device.
         *
         *  \param dev Device to read from.
         *  \param values Where to store results.
         *  \return true if values successfully read, false otherwise.
         */
        bool ADCSweep(uint8 dev, uint16 *values);
    }
}

#endif /* _ADC_H_ */
