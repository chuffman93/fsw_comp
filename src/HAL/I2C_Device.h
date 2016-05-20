/*! \file I2C_Device.h
 *  \brief Define all things related to i2c devices and communication
 *
 *
 *
 */
#include <stdint.h>

namespace Phoenix
{
	namespace HAL
	{
		class I2C_Device{
		public:
			I2C_Device(int bus, uint8_t address);

		private:
			struct{
						char path[20];
						int fd;
						uint32_t slave_addr;
					}i2c_chip_info;
		};

	}
}
