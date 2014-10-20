/*
 * Interrupt.cpp

 *
 *  Created on: July 9, 2014
 *      Author: Connor
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <poll.h>
#include <time.h>

#include <stdint.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#include "HAL/Interrupt.h"
#include "POSIX.h"

#if INTERRUPT_DEBUG
#include <iostream>
#define DEBUG_COUT(m) cout << m << endl;
#else
#define DEBUG_COUT(m)
#endif // DEBUG

#define NUM_OF_INTERRUPTS 3
#define SYSFS_GPIO_DIR "/sys/class/gpio"
#define MAX_BUF 64
#define PIO_BANK_SIZE 32
#define SUBSYS_QUEUE_LENGTH 32

char* subsystemQueueName = "/subsystemQueueHandle";
int gpioIntPins[NUM_OF_INTERRUPTS] = {81,80};
int gpioIntFds[NUM_OF_INTERRUPTS];
pollfd gpioIntPoll[NUM_OF_INTERRUPTS];

using namespace Phoenix::Core;

namespace Phoenix
{
    namespace HAL
    {
    	void GPIOExport(int gpio)
        {
       		std::ofstream exportFile;

       		exportFile.open(SYSFS_GPIO_DIR "/export",std::ios::out);
       		exportFile << gpio;
       		exportFile.close();
       	}

       	void GPIOUnExport(int gpio)
       	{
       		std::ofstream unExportFile;
       		unExportFile.open(SYSFS_GPIO_DIR "/unexport",std::ios::out);
           	unExportFile << gpio;
           	unExportFile.close();
        }

        void GPIOSetDir(std::string pioString, bool in)
        {
        	std::stringstream dirString;
        	dirString << pioString << "/direction";
       		std::ofstream gpioDirFile;
       		std::cout << dirString.str() << std::endl;

        	gpioDirFile.open(dirString.str().c_str(), std::ios::out);
       		in ? gpioDirFile << "in" : gpioDirFile << "out";
       		gpioDirFile.close();
       	}

       	void GPIOSetEdge(std::string pioString, bool rising)
       	{
       		std::stringstream edgeString;
       		edgeString << pioString << "/edge";
       		std::cout << edgeString.str() << std::endl;
       		std::ofstream gpioEdgeFile;

        	gpioEdgeFile.open(edgeString.str().c_str(), std::ios::out);
       		rising ? gpioEdgeFile << "rising" : gpioEdgeFile << "falling";
        	gpioEdgeFile.close();
        }

        std::string GPIOtoPIO(int gpio)
        {
        	std::stringstream pioString;

        	pioString << SYSFS_GPIO_DIR << "/pio";

        	if(gpio >= 128)
        	{
       			pioString << "E";
       		}
       		else if(gpio >= 96)
       		{
       			pioString << "D";
       		}
       		else if(gpio >= 64)
        	{
        		pioString << "C";
       		}
       		else if(gpio >= 32)
       		{
       			pioString << "B";
       		}
       		else
       		{
       			pioString << "A";
       		}

        	int pioInt = gpio % PIO_BANK_SIZE;
       		pioString << pioInt;

        	return pioString.str();
       	}

       	int OpenGPIOIntFd(std::string pioString)
        {
       		char dummyBuf[100];

       		std::stringstream edgeString;
       		edgeString << pioString << "/value";

       		int gpioFd = open(edgeString.str().c_str(),O_RDONLY);
       		read(gpioFd,dummyBuf,100);
       		std::cout << edgeString.str() << std::endl;
       		return gpioFd;
       	}

        bool InterruptHandler(int gpioPin)
        {
        	SubsystemInterruptAlertType * alert;
            struct mq_attr queueAttr;

            try
            {
                alert = new SubsystemInterruptAlertType;
            }
            catch (std::bad_alloc & e)
            {
                return false;
            }

			mqd_t queueHandle = mq_open(subsystemQueueName, O_WRONLY);
            size_t numPackets = mq_size(queueHandle, queueAttr);

            clock_gettime(CLOCK_REALTIME, &(alert->time));
            alert->pin = gpioPin;

            if (numPackets < SUBSYS_QUEUE_LENGTH)
            {
                //bool ret = mq_subsys_send(subsystemQueueName, &alert, MAX_BLOCK_TIME, 0);
                return true; //delete alert?
			}
			else
			{
				delete alert;
				mq_close(queueHandle);
				return false;
			}
            mq_close(queueHandle);
            return false;
        }

        void interruptUnInit()
        {
        	for(int i = 0;i < NUM_OF_INTERRUPTS;i++)
        	{
            	std::string pioPath = GPIOtoPIO(gpioIntPins[i]);
            	GPIOUnExport(gpioIntPins[i]);
            	close(gpioIntPoll[i].fd);
        	}
        }

        void intInit()
        {
        	char dummyBuf[100];
        	//pollfd gpioIntPoll[NUM_OF_INTERRUPTS];

        	for(int i = 0; i < NUM_OF_INTERRUPTS; i++)
        	{
        	//creating string to sys/class/gpio/pioXXX
        	std::string pioPath = GPIOtoPIO(gpioIntPins[i]);

        	GPIOExport(gpioIntPins[i]);
        	GPIOSetDir(pioPath, 1);
        	GPIOSetEdge(pioPath, 0);

        	gpioIntPoll[i].fd = OpenGPIOIntFd(pioPath);
        	gpioIntPoll[i].events = POLLPRI|POLLERR;
        	gpioIntPoll[i].revents = 0;
        	}

    		std::cout << "waiting for interrupt" << std::endl;
        	while(1)
        	{
        		int pollRet = poll(gpioIntPoll,NUM_OF_INTERRUPTS,-1);
        		if(pollRet > 0)
        		{
        			for(int i = 0; i < NUM_OF_INTERRUPTS; i++)
        			{
        				if(gpioIntPoll[i].revents & POLLPRI)
        				{
        					lseek(gpioIntPoll[i].fd,0,SEEK_SET);
        					gpioIntPoll[i].revents = 0;
        					read(gpioIntPoll[i].fd,dummyBuf,100);
        					InterruptHandler(gpioIntPins[i]);
        				}
        			}
        		}
        	}
        }
    }
}
