/*! \file Interrupt.h
 *  \brief Header file for the interrupt interface
 *
 *  This file contains the definition for the HAL interrupt interface.
 */

#ifndef _INTERRUPT_H_
#define _INTERRUPT_H_

#include "core/StdTypes.h"
#include <time.h>

namespace Phoenix
{
    namespace HAL
    {
        //typedef void (InterruptHandler)(void);

        enum InterruptPullState
        {
            INTERRUPT_PULL_NONE = 0,
            INTERRUPT_PULL_UP,
            INTERRUPT_PULL_DOWN
        };

        enum InterruptEdge
        {
            INTERRUPT_EDGE_RISING = 0,
            INTERRUPT_EDGE_FALLING,
            INTERRUPT_EDGE_BOTH
        };

        struct SubsystemInterruptAlertType
                    {
                        uint8 pin;
                        timespec time;
                    };

        /*! \brief Exports a GPIO pin to /sys/class/gpio
         *
         *  \GPIO pin to export to sysfs. Must pass the GPIO num in
         *
         *  \return -
         */
        void GPIOExport(int gpio);

        /*! \brief Unexports a GPIO pin from /sys/class/gpio
         *
         *
         *  \note
         *
         *  \return-
         */
        void GPIOUnExport(int gpio);

        /*! \brief Sets input or output for a GPIO pin
         *
         *  Writes "in" or "out" to the file located at
         *  /sys/class/gpio/pioXXX/direction
         *
         *  \note The function GPIOtoPIO must be called before
         *  this to obtain the string that contains the path to
         *  a particular GPIO
         *
         *  \param pioString is string to /sys/class/gpio/pioXXX
         *  \param in 1 for Input, 0 for output
         *
         *  \return -
         */
        void GPIOSetDir(std::string pioString, bool in);

        /*! \brief Sets rising or falling edge for a GPIO pin
         *
         *	Writes "falling" or "rising" to the file located at
         *	/sys/class/gpio/pioXXX/edge
         *
         *  \param pioString is the string to /sys/class/gpio/pioXXX
         *  \param rising: 1 for rising edge trigger and 0 for falling edge
         *
         *  \return -
         *
         *  \note The function GPIOSetDir must be called before this if the
         *  GPIO pin is not set to an input.
         */
        void GPIOSetEdge(std::string pioString, bool rising);

        /*! \Returns string for filepath to access a GPIO device
         *
         *	Returns string to /sys/class/gpio/pioXXX in order to open
         *	and read gpio files at that
         *
         *  \return string to pioXXX
         */
        std::string GPIOtoPIO(int gpio);

        /*! \Open file descriptor at /sys/class/gpio/pioXXX/value for monitoring
         *
         *  Opens file descriptor that can be used to poll(2) for interrupts
         *
         *  \return integer file descriptor to .../pioXXX/value
         *
         */
        int OpenGPIOIntFd(std::string pioString);

        /*! \ Cleans up GPIO resources that were created during initialization
         *  process
         *
         *  Closes all file descriptors and unexports all GPIO pins from /sys/class/gpio
         *
         *  \return -
         *
         */

        bool InterruptHandler(int gpioPin);
        /*! \Dispatches an alert to the subsystem queue
         *
         *
         */

        void interruptUnInit();

        /*! \Initalizes all required resources and monitors for GPIO interrupts
          *
          *  Uses other function calls to export GPIO pins to filesystem,
          *  and sets up strings for file paths to GPIO pins. Begins monitoring
          *  for interrupts and will call the interrupt handler in the event one occurs
          *
          *  \return -
          *
          */
        void intInit();
    }
}

#endif /* _INTERRUPT_H_ */
