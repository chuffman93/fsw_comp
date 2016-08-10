/*! \file I2C.h
 *  \brief Header file for the I2C interface
 *
 *  This file contains the definition for the HAL I2C interface.
 */

#ifndef _I2C_H_
#define _I2C_H_

#include "core/StdTypes.h"
#include <stdint.h>

namespace AllStar
{
	namespace HAL
	{
		/*! \brief Struct holds I2C parameters
		 *
		 * path := absolute directory path for the i2c dev used
		 * chip_fd := file descriptor to hold the path fd
		 * slave_addr := slave address
		 */
		typedef struct i2c_chip_info{
			char path[20];
			int fd;
			uint32_t slave_addr;

		}i2c_chip_info;

        /*! \brief Initialize Slave elements for I2C interface
         *
         *  Places parameters and information for each I2C device & chip
         *  in a struct to be accessed by the other I2C functions later,
         *  and opens file descriptors for each defined I2C device
         *
         *  \note This function must be called before any other I2C
         *  functions and may only be called exactly once.
         */
        void I2CInit(void);

        bool I2CSetReg(i2c_chip_info chip, uint8_t reg, uint8_t val_buf, size_t num_bytes);

        bool I2CGetReg(i2c_chip_info chip, uint8_t reg, uint8_t * val, size_t num_bytes);
	}
}

#endif /* _I2C_H_ */
