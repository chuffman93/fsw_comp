/*! \file I2C_Device.cpp
 *  \brief Implement functionality for I2C Communications
 *
 * Robert Belter Summer 2016
 *
 */

#include <fcntl.h>

namespace Phoenix
{
	namespace HAL
	{
		I2C_Device::I2C_Device(int bus, uint8_t address){

			i2c_chip_info.slave_addr = address >> 1;

			char filename[20];
			sprintf(filename, "/dev/i2c-%d", bus);


		}


		//------------------------Raw Read Write Methods---------------------------------

		bool I2C_Device::I2C_write(uint8_t* buffer, int numBytes){
			int fd = open(i2c_chip_info.filename, O_WRONLY);
			if(fd < 0){
				return false;
			}

			int retval = write(fd, buffer, numBytes);
			close(fd);

			if(retval != numBytes)
				return false;
			return true;
		}

		bool I2C_Device::I2C_write(uint8_t byte){
			return I2C_write(&byte, 1);
		}

		bool I2C_Device::I2C_read(uint8_t* buffer, int numBytes){
			int fd = open(i2c_chip_info.filename, O_RDONLY);
			if(fd < 0){
				return false;
			}

			int retval = read(fd, buffer, numBytes);
			close(fd);

			if(retval != numBytes)
				return false;
			return true;
		}
		uint8_t I2C_Device::I2C_readReg(uint8_t reg){
			I2C_write(reg);
			uint8_t retval;
			I2C_read(&retval, 1);
			return retval;
		}
	}
}
