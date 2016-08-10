#define HAL_TEST
#ifdef HAL_TEST

#include "gtest/gtest.h"

#include "POSIX.h"
#include "HAL/SPI.h"
#include "HAL/Interrupt.h"
#include "HAL/GPIO.h"

#include <iostream>
#include <poll.h>
#include <fstream>
#include <sstream>

#define NUM_OF_INTERRUPTS 1
#define HAL_TEST
#define ARRAY_SIZE(array) sizeof(array)/sizeof(array[0])

using namespace std;
using namespace rel_ops;
using namespace AllStar::HAL;

string testCounterGPIO;
int gpioFd;
Thi

TEST(TestHAL, DISABLED_GPIOSetup){ //Initalizes GPIO for output to trigger the next testing setup
	uint32 gpioTogglePin = 16;
	//string gpioString;

	testCounterGPIO = GPIOtoPIO(gpioTogglePin);
	std::cout << testCounterGPIO << std::endl;
	GPIOExport(gpioTogglePin);
	GPIOSetDir(testCounterGPIO, OUTPUT);
	//export gpio and then set it to output.should fix up the gpio/interrupt code
}


TEST(TestHAL, DISABLED_GPIOtoggle)
{
	std::stringstream valueString;
	valueString << testCounterGPIO << "/value";
	//std::cout << valueString.str() << std::endl;

	int gpioFd = open(valueString.str().c_str(),O_WRONLY);
	//open file descriptor for gpio value

	/*
	int i = 0;
	while(1)
	{
		if(i%2 == 0)
		{
			write(gpioFd,"1",1);
		}
		else
		{
			write(gpioFd,"0",1);
		}
		i++;
		usleep(1000000);
	}
	close(gpioFd);
	*/
}


TEST(TestHAL, DISABLED_TestInterrupts){
	uint32 subsysIntPin = 80;
	pollfd gpioIntPoll;
	string gpioString;
	char dummyBuf;

	gpioString = GPIOtoPIO(subsysIntPin);
	GPIOExport(subsysIntPin);
	GPIOSetDir(gpioString, INPUT);
	GPIOSetEdge(gpioString, FALLING);

	std::cout << gpioString << std::endl;

	gpioIntPoll.fd = OpenGPIOIntFd(gpioString);
	gpioIntPoll.events = POLLPRI|POLLERR;
	gpioIntPoll.revents = 0;

	std::cout << "about to while" << std::endl;
	while(1)
	        	{
	        		int pollRet = poll(&gpioIntPoll,1,-1);
	        		if(pollRet > 0)
	        		{
	        			for(int i = 0; i < NUM_OF_INTERRUPTS; i++)
	        			{
	        				if(gpioIntPoll.revents & POLLPRI)
	        				{
	        					std::cout << "INTERRUPTED" << std::endl;
	        					lseek(gpioIntPoll.fd,0,SEEK_SET);
	        					gpioIntPoll.revents = 0;
	        					read(gpioIntPoll.fd,(void*) dummyBuf,100);
	        					break;
	        				}
	        			}
	        			break;
	        		}
	        	}
	SUCCEED();
}

TEST(TestHAL, DISABLED_TestUART){
	int test_dev = 1;
	int test_chip = 0;
	const char txBuf[9]  = "ABCDEF\r\n";
	char rxBuf[54];
    uint8_t buf[2];
    bool ret;

    std::cout << "SPI OS Init\n";
    SPIOSInit();

    puts("running spi chip int");
	ASSERT_TRUE(SPIChipInit(test_dev,test_chip));
	puts("writing uart buffer");
	ASSERT_TRUE(SPIWrite(test_dev, test_chip,(unsigned char*) txBuf,9,0));
	//wrote txBuf. Now wait for sync char.
	puts("waiting for sync char '#");
	do{
		ret = SPIRead(test_dev,test_chip,buf,1,0);
		if(buf[0] == '#' && ret == true)
		{
			break;
		}
	}while(1);
	//read sync char. now read into rxBuf
	puts("reading into rxBuf");
	int i = 0;
	do{
		if(!SPIRead(test_dev,test_chip,buf,1,0))
		{
			puts("UART Read Error");
		}
		else
		{
			rxBuf[i] = buf[0];
			i++;
			std::cout << "i = " << i << std::endl;
		}
	}while(!(rxBuf[i-1] == '\r' && rxBuf[i] == '\n'));

	usleep(100);

	for(int i = 0; i < 54; i ++)
	{
		std::cout << rxBuf[i];
		if(rxBuf[i] != txBuf[i])
		{
			write(gpioFd, "0", 1);
			FAIL() << "Did not receive original UART message";
		}
	}
	write(gpioFd, "0", 1);
	SUCCEED() << "Received original UART message";
}


TEST(TestHAL, TestSPI){
	int test_dev = 0;
	int test_chip = 0;
	uint8_t txBuf[54]  = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklnopqrstuvwxyz\r\n";
	uint8_t rxBuf[ARRAY_SIZE(txBuf)];
	uint8_t txChar = 'B';
	uint8_t rxChar;

	cout << "SPI Device: " << test_dev << " SPI Chip: " << test_chip << endl;
	SPIOSInit();
	usleep(10000);
	ASSERT_TRUE(SPIChipInit(test_dev,test_chip));
	cout << "Sending Message: ";
	//ASSERT_TRUE(SPIReadWrite(test_dev,test_chip,(uint8 *) txBuf,(uint8 *) rxBuf,ARRAY_SIZE(txBuf),ARRAY_SIZE(rxBuf),0));

	cout << "sending " << ARRAY_SIZE(txBuf) << "bytes\n";
	cout << "receiving " << ARRAY_SIZE(rxBuf) << "bytes\n";
	usleep(10000);

	while(1)
	{
		if(SPIReadWrite(test_dev,test_chip,txBuf, rxBuf, ARRAY_SIZE(txBuf),ARRAY_SIZE(rxBuf),0) == false)
		//if(SPIReadWrite(test_dev,test_chip,&txChar,&rxChar,1,1,0) == false)
			cout << "error\n";
		else
		{
			cout << "Received:\n";
			for(int i = 0; i < ARRAY_SIZE(rxBuf);i++)
			{
				cout << rxBuf[i];
				rxBuf[i] = 0;
			}
			//cout << rxChar;
			cout << "\n";
			//rxChar = 0;
		}
		usleep(100000);
	}


	/*
	int i = 0;
	while(1)
	{
		SPIRead(test_dev,test_chip,(uint8 *) rxBuf,1,0);
		cout << "read: " << rxBuf[i];
		i++;
	}
	*/

	cout << "received message: ";

	for(int j = 0; j < ARRAY_SIZE(rxBuf);j++)
	{
		//cout << rxBuf[j];
		if(txBuf[j] != rxBuf[j])
		{
			write(gpioFd, "1", 1);
			//FAIL() << "Did not receive original SPI message";
		}
	}
	cout << endl;
	write(gpioFd, "0", 1);
	close(gpioFd);
	SUCCEED() << "Received original SPI message";
}


#endif
