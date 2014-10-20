/*! \file SPI.h
 *  \brief Header file for the SPI interface
 *
 *  This file contains the definition for the HAL SPI interface.
 */

#ifndef _SPI_H_
#define _SPI_H_

#include "core/StdTypes.h"
#include <stdint.h>
//#include "backplane_common.h"

#define NUM_SPIS 2
#define SPI_CHIP_MAX 1

namespace Phoenix
{
    namespace HAL
    {

		struct spi_chip_info{
			uint8_t bits_per_word;
			uint8_t mode;
			uint32_t speed; //max speed
			uint16_t cs_delay;
			int chip_fd;
			char *path = "/dev/spidev12345.6";
		};

		typedef enum {
			SPI_TYPE_SPI = 0,
			SPI_TYPE_USART
		} SPIType;

		/*! \brief Struct holds SPI Master information
		 *
		 * Holds the number of chips and the SPItype
		 * for each SPI master
		 */

		struct spi_master_info{
			uint8 num_chips;
			SPIType type;
		};

        /*! \brief Initialize OS elements for SPI interface
         *
         *  Places parameters and information for each SPI device & chip
         *  in a struct to be accessed by the other SPI functions later,
         *  and opens file descriptors for each defined SPI device
         *
         *  \note This function must be called before any other SPI
         *  functions and may only be called exactly once.
         */
        void SPIOSInit(void);

        /*! \brief Optionally set certain SPI transfer parameters
                *
                *  Can optionally reset the SPI Mode, Max Transfer Speed,
                *  and bits per word to values other than the default.
                *
                *  \param dev Device to open.
                *  \param chip Chip to be selected.
                *  \param
                *  \return true if device successfully opened, false otherwise.
                */
        bool SPIChipInit(uint8 dev, uint8 chip,uint8_t spi_mode, unsigned long speed_hz,
                         uint8 bitsPerWord);

        /*! \brief Read/Write data to/from the selected chip
         *
         *  Reads and writes data to/from the SPI device.
         *
         *  \param dev Device to write to.
         *  \param chip chip to write.
         *  \param inData Where to store received data.
         *  \param size Number of bytes to read/write.
         *  \return true if all data read/written, false otherwise.
         */
        bool SPIReadWrite(uint8 dev, uint8 chip, uint8_t *out_data, uint8_t *in_data, std::size_t size,
				  uint8_t cs_toggle);

        /* Convenience Macros */
//        #define SPIWrite(dev, chip, out, in, size, cs_toggle) SPIReadWrite(dev, chip, out, NULL, size, cs_toggle)
//        #define SPIRead(dev, chip, out, in, size, cs_toggle) SPIReadWrite(dev, chip, NULL, in, size, cs_toggle)
    }
}

#endif /* _SPI_H_ */
