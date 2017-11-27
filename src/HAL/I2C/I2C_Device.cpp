/*! \file I2C_Device.cpp
 *  \brief Implement functionality for I2C Communications
 *
 * Robert Belter Summer 2016
 *
 */

#include <fcntl.h>
#include "HAL/I2C/I2C_Device.h"
#include <stdio.h>
#include <unistd.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>

#include "util/Logger.h"
#include "core/StdTypes.h"

namespace AllStar
{
	namespace HAL
	{
		I2C_Device::I2C_Device(int bus, uint8_t address){

			i2c_chip_info.slave_addr = address >> 1;
			sprintf(i2c_chip_info.filename, "/dev/i2c-%d", bus);

		}


		//------------------------Raw Read Write Methods---------------------------------

		bool I2C_Device::I2C_write(uint8_t* buffer, int numBytes){
			int fd = open(i2c_chip_info.filename, O_WRONLY);
			if(fd < 0){
				return false;
			}
			if(ioctl(fd, I2C_SLAVE, i2c_chip_info.slave_addr) < 0){
				close(fd);
				return false;
			}

			int retval = write(fd, buffer, numBytes);
			close(fd);

			if(retval != numBytes){
				return false;
			}
			return true;
		}

		bool I2C_Device::I2C_write(uint8_t byte){
			return I2C_write(&byte, 1);
		}

		bool I2C_Device::I2C_read(uint8_t* buffer, int numBytes){
			Logger * logger = static_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
			//logger->Debug("I2CDevice Filename: %s", i2c_chip_info.filename);
			int fd = open(i2c_chip_info.filename, O_RDONLY);
			if(fd < 0){
				//logger->Debug("----I2C_Device: file descriptor error");
				return false;
			}
			if(ioctl(fd, I2C_SLAVE, i2c_chip_info.slave_addr) < 0){
				//logger->Debug("----I2C_Device: ioctl error");
				close(fd);
				return false;
			}

			int retval = read(fd, buffer, numBytes);
			close(fd);

			if(retval != numBytes){
				//logger->Debug("----I2C_Device: Num bytes error");
				return false;
			}
			return true;
		}
		uint8_t I2C_Device::I2C_readReg(uint8_t reg){
			I2C_write(reg);
			uint8_t retval;
			I2C_read(&retval, 1);
			return retval;
		}

		void I2C_Device::I2C_writeReg(uint8_t reg, uint8_t value){
			uint8_t buf[2];
			buf[0] = reg;
			buf[1] = value;
			I2C_write(buf, 2);
		}
	}
}
