/*
 */

//#include "boards/board.h"
//#include "backplane_common.h"

#include "core/StdTypes.h"
#include "HAL/SPI.h"

#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>
#include <string>

#define ARRAY_SIZE(array) sizeof(array)/sizeof(array[0])

using namespace Phoenix::HAL;

static struct spi_chip_info spi_chip[NUM_SPIS][SPI_CHIP_MAX];
static struct spi_master_info spi_master[NUM_SPIS];

namespace Phoenix
{
    namespace HAL
    {

    void SPIOSInit(void)
        	{
        	/*
        	 * Device 0:
        	 */
        	spi_master[0].num_chips = 1;
        	spi_master[0].type = SPI_TYPE_SPI;

        	/*
        	 * Device 0 Chip 0:
        	 */
        		spi_chip[0][0].bits_per_word = 8;
        		spi_chip[0][0].mode = 3;
        		spi_chip[0][0].speed = 1000000;
        		spi_chip[0][0].cs_delay = 0;
        		spi_chip[0][0].path = "/dev/spidev32765.0";
        		spi_chip[0][0].chip_fd = open(spi_chip[0][0].path, O_RDWR);
        	/*
        	 * Device * Chip *:
        	 */

        	}

    bool SPIChipInit(uint8 dev, uint8 chip, uint8_t spi_mode = 0, uint32 speed_hz = 0,
                    uint8 bits_per_word = 0)
            {
                if (dev >= NUM_SPIS || chip >= spi_master[dev].num_chips)
                {
                	//log error: SPI chip or master does not exist
                	return false;
                }

                if(spi_chip[dev][chip].chip_fd < 0)
                {
                	return false; //could not open SPI file descriptor
                }
                if (spi_master[dev].type == SPI_TYPE_SPI)
                {

                	//setting mode
                	if(spi_mode != 0)
                	{
                	 if(ioctl(spi_chip[dev][chip].chip_fd, SPI_IOC_WR_MODE,&spi_mode) == -1)
                     {
                    	//could not set mode
                		 return false;
                     }
                    }

                	//setting bits per word
                   if(bits_per_word != 0)
                   {
                    if(ioctl(spi_chip[dev][chip].chip_fd, SPI_IOC_WR_BITS_PER_WORD,&bits_per_word) == -1)
                    {
                    	//could not set bits per word
                    	return false;
                    }
                   }

                   //setting max speed
                  if(speed_hz != 0)
                  {
                    if(ioctl(spi_chip[dev][chip].chip_fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed_hz) == -1)
                    {
                    	//could not set speed
                    	return false;
                    }
                  }

                }
                else if (spi_master[dev].type == SPI_TYPE_USART) //FIXME implementing usart later. keeping for reference
                {
                	//FIXME Add USART
                }
                else
                {
                    // unknown spi type
                    return false;
                }

                return true;
            }

    bool SPIReadWrite(uint8 dev, uint8 chip, uint8_t *out_data, uint8_t *in_data, std::size_t size,
            				 uint8_t cs_toggle)
            {
                if (dev >= NUM_SPIS || chip >= spi_master[dev].num_chips)
                {
                    //log error: SPI chip or master does not exist
                	return false;
                }

                if(out_data == NULL)
                {
                	*out_data = 0xFF;
                }

            	struct spi_ioc_transfer spi_transfer = {
            		spi_transfer.tx_buf = (unsigned long)out_data,
            		spi_transfer.rx_buf = (unsigned long)in_data,
            		spi_transfer.len = size,
            		spi_transfer.delay_usecs = spi_chip[dev][chip].cs_delay,
            		spi_transfer.speed_hz = spi_chip[dev][chip].speed,
            		spi_transfer.bits_per_word = spi_chip[dev][chip].bits_per_word,
            		spi_transfer.cs_change = cs_toggle,
            	};


                if (spi_master[dev].type == SPI_TYPE_SPI)
                {
                    if(ioctl(spi_chip[dev][chip].chip_fd, SPI_IOC_MESSAGE(1), &spi_transfer) < 0)
                    {
                    	//log error: unable to send SPI message
                    	return false;
                    }

                }
                else if (spi_master[dev].type == SPI_TYPE_USART)
                {
                	//FIXME add USART
                	return false;
                }
                else
                {
                    //unknown spi type
                    return false;
                }
                return true;
           }
    }
}
