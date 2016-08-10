/*! \file UART.h
 *  \brief Header file for the UART interface
 *
 *  This file contains the definition for the HAL UART interface.
 */

#ifndef _UART_H_
#define _UART_H_

#include "core/StdTypes.h"

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus
/*! \brief Write data to the UART device Rx buffer.
*
*  Writes data to the UART device Rx buffer.  This should ONLY be used
*  internally to emulate a UART (such as the USB CDC task).
*
*  \param dev Device to write to.
*  \param data Data to write.
*  \param size Number of bytes to write.
*  \return Number of bytes written.
*/
size_t UARTRxBufferWrite(uint8 dev, const uint8 * data, size_t size);

/*! \brief Write data from the UART device Tx buffer.
*
*  Reads data from the UART device Tx buffer.  This should ONLY be used
*  internally to emulate a UART (such as the USB CDC task).
*
*  \param dev Device to read from.
*  \param data Where to store received bytes.
*  \param size Number of bytes to read.
*  \return Number of bytes read.
*/
size_t UARTTxBufferRead(uint8 dev, uint8 * data, size_t size);

#ifdef __cplusplus
}
#endif // __cplusplus

#ifdef __cplusplus
namespace AllStar
{
    namespace HAL
    {
        /*! \brief Initialize OS elements for UART interface
         *
         *  Creates ring buffers, semaphores, etc. which are used in
         *  other UART functions.
         *
         *  \note This function must be called before any other UART
         *  functions and may only be called exactly once.
         */
        void UARTOSInit(void);

        /*! \brief Initialize a UART device
         *
         *  Initializes the UART hardware.
         *
         *  \note This function must be called before accessing the
         *  device for the first time. It may be called multiple times
         *  to reinitialize the hardware with different settings.
         *
         *  \param dev Device to initialize.
         *  \param baud Baudrate.
         *  \return true if device successfully initialized, false otherwise.
         */
        bool UARTInit(uint8 dev, unsigned long baud);

        /*! \brief Open a UART device
         *
         *  Takes control of the UART device.
         *
         *  \note This function must be called before calling
         *  UARTWrite or UARTRead. UARTClose must be called when
         *  finished with the device.
         *
         *  \param dev Device to open.
         *  \return true if device successfully opened, false otherwise.
         */
        bool UARTOpen(uint8 dev);

        /*! \brief Close a UART device
         *
         *  Releases control of the UART device.
         *
         *  \param dev Device to close.
         *  \return true if device successfully closed, false otherwise.
         */
        bool UARTClose(uint8 dev);

        /*! \brief Write data to the UART device
         *
         *  Writes data to the UART device. The data is copied to an
         *  internal buffer and written asynchronously.
         *
         *  \note If the internal buffer becomes full, some data may not
         *  be written.
         *
         *  \param dev Device to write to.
         *  \param data Data to write.
         *  \param size Number of bytes to write.
         *  \return Number of bytes written, or -1 if an error occurred.
         */
        int UARTWrite(uint8 dev, const uint8 *data, std::size_t size);

        /*! \brief Read data from the UART device
         *
         *  Reads data from the UART device.
         *
         *  \param dev Device to read from.
         *  \param data Where to store received bytes.
         *  \param size Number of bytes to read.
         *  \param block If true, function will block until all requested
         *  bytes are received.
         *  \return Number of bytes read, or -1 if an error occurred.
         */
        int UARTRead(uint8 dev, uint8 *data, std::size_t size, bool block=false);
    }
}
#endif /* __cplusplus */
#endif /* _UART_H_ */
