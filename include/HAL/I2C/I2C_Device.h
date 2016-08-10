/*! \file I2C_Device.h
 *  \brief Define all things related to i2c devices and communication
 *
 * Robert Belter Summer 2016
 *
 */
#include <stdint.h>

#ifndef I2C_DEVICE_H_
#define I2C_DEVICE_H_

namespace AllStar
{
	namespace HAL
	{
		class I2C_Device{
		public:
			I2C_Device(int bus, uint8_t address);
			bool I2C_write(uint8_t* buffer, int numBytes);
			bool I2C_write(uint8_t byte);
			bool I2C_read(uint8_t* buffer, int numBytes);
			uint8_t I2C_readReg(uint8_t reg);
			void I2C_writeReg(uint8_t reg, uint8_t value);
		private:
			struct{
				char filename[20];
				uint8_t slave_addr;
			}i2c_chip_info;
		};

	}
}

#endif
