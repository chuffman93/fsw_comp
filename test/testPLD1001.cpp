#include "util/Logger.h"
#include "HAL/SPI_Server.h"
#include "gtest/gtest.h"
#include "core/StdTypes.h"
#include "core/Factory.h"
#include <string>

using namespace std;
using namespace rel_ops;
using namespace Phoenix::Core;

void * StartSPI_HAL(void * params)
{
	Logger * logger = dynamic_cast<Logger *>(Factory::GetInstance(LOGGER_SINGLETON));
	SPI_HALServer * spi_server = dynamic_cast<SPI_HALServer *> (Factory::GetInstance(SPI_HALSERVER_SINGLETON));

	for(int i = 0; i < 10; i++)
	{
		//wdm->Kick();
		usleep(100000);
	}

	logger->Log("Kicking off the SPI HAL server", LOGGER_LEVEL_INFO);
	spi_server->SPI_HALServerLoop();
	pthread_exit(NULL);
}

TEST(TestPLD, sendSPI){
	Logger * logger = dynamic_cast<Logger *>(Factory::GetInstance(LOGGER_SINGLETON));

	// ------------------------------------- SPI Thread -------------------------------------
	pthread_t SPIThread;
	bool threadCreated = pthread_create(&SPIThread ,NULL,&StartSPI_HAL, NULL );

	if(!threadCreated)
	{
		logger->Log("SPI HALServer Thread Creation Success", LOGGER_LEVEL_INFO);
	}
	else
	{
		logger->Log("SPI HALServer Thread Creation Failed", LOGGER_LEVEL_FATAL);
	}

	usleep(1500000);

	SPI_HALServer * spi_server = dynamic_cast<SPI_HALServer *> (Factory::GetInstance(SPI_HALSERVER_SINGLETON));
	FSWPacket * packet = new FSWPacket(SERVER_LOCATION_PLD, HARDWARE_LOCATION_EPS, 0, 0, true, false, MESSAGE_TYPE_COMMAND);
	spi_server->SPIDispatch(*packet);
}
