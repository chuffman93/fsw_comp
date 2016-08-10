#include "core/StdTypes.h"
#include "HAL/I2C.h"

#include <stdio.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <string.h>

namespace AllStar
{
	namespace HAL
	{
		void I2CInit(void)
		{

		}


		//TODO:expand set outbuf to be able to send at least 4 bytes and use val to determine how much of that to use
        bool I2CSetReg(i2c_chip_info * chip, uint8_t reg, uint8_t * val_buf, size_t num_bytes)
        {
        	uint8_t * outbuf;
			struct i2c_rdwr_ioctl_data packets;
			struct i2c_msg messages[1];

			outbuf = (uint8_t *) malloc(sizeof(num_bytes + 1));

			messages[0].addr  = chip->slave_addr;
			messages[0].flags = 0;
			messages[0].len   = num_bytes;
			messages[0].buf   = outbuf;

			/* The first byte indicates which register we'll write */
			outbuf[0] = reg;

			/*
			 * All bytes after the first are values to write.
			 */
			for(int i = 1; i < num_bytes; i++)
			{
				outbuf[i] = val_buf[i-1];
			}

			/* Transfer the i2c packets to the kernel and verify it worked */
			packets.msgs  = messages;
			packets.nmsgs = 1;
			if(ioctl(chip->fd, I2C_RDWR, &packets) < 0) {
				perror("Unable to send data");
				free(outbuf);
				return false;
			}

			free(outbuf);
			return true;
        }

        //TODO:expand set outbuf to be able to send at least 4 bytes and use val to determine how much of that to use
        bool I2CGetReg(i2c_chip_info * chip, uint8_t reg, uint8_t * val, size_t num_bytes)
        {
            uint8_t outbuf;
            uint8_t * inbuf = (uint8_t *) malloc(sizeof(num_bytes));
            struct i2c_rdwr_ioctl_data packets;
            struct i2c_msg messages[2];

            /*
             * In order to read a register, we first do a "dummy write" by writing
             * 0 bytes to the register we want to read from.  This is similar to
             * the packet in set_i2c_register, except it's 1 byte rather than 2.
             */
            outbuf = reg;
            messages[0].addr  = chip->slave_addr;
            messages[0].flags = 0;
            messages[0].len   = sizeof(outbuf);
            messages[0].buf   = &outbuf;

            /* The data will get returned in this structure */
            messages[1].addr  = chip->slave_addr;
            messages[1].flags = I2C_M_RD/* | I2C_M_NOSTART*/;
            messages[1].len   = num_bytes;
            messages[1].buf   = inbuf;

            /* Send the request to the kernel and get the result back */
            packets.msgs      = messages;
            packets.nmsgs     = 2;
            if(ioctl(chip->fd, I2C_RDWR, &packets) < 0) {
                perror("Unable to send data");
                free(inbuf);
                return false;
            }

            for(int i = 0; i  < num_bytes; i++)
            {
            	val[i] = inbuf[i];
            }

            free(inbuf);

            return true;
        }



	} //HAL
} //AllStar
