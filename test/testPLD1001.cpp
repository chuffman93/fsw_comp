#include "util/Logger.h"
#include "HAL/SPI_Server.h"
#include "gtest/gtest.h"
#include "core/StdTypes.h"
#include "core/Factory.h"
#include <string>

using namespace std;
using namespace rel_ops;
using namespace AllStar::Core;

void * StartSPI_HAL(void * params)
{
	Logger * logger = static_cast<Logger *>(Factory::GetInstance(LOGGER_SINGLETON));
	SPI_HALServer * spi_server = static_cast<SPI_HALServer *> (Factory::GetInstance(SPI_HALSERVER_SINGLETON));

	for(int i = 0; i < 10; i++)
	{
		//wdm->Kick();
		usleep(100000);
	}

	logger->Info("Kicking off the SPI HAL server");
	spi_server->SPI_HALServerLoop();
	pthread_exit(NULL);
}

TEST(TestPLD, sendSPI){
	Logger * logger = static_cast<Logger *>(Factory::GetInstance(LOGGER_SINGLETON));

	// ------------------------------------- SPI Thread -------------------------------------
	pthread_t SPIThread;
	bool threadCreated = pthread_create(&SPIThread ,NULL,&StartSPI_HAL, NULL );

	if(!threadCreated)
	{
		logger->Info("SPI HALServer Thread Creation Success");
	}
	else
	{
		logger->Fatal("SPI HALServer Thread Creation Failed");
	}

	usleep(1500000);

	SPI_HALServer * spi_server = static_cast<SPI_HALServer *> (Factory::GetInstance(SPI_HALSERVER_SINGLETON));
	ACPPacket * packet = new ACPPacket(SERVER_LOCATION_PLD, HARDWARE_LOCATION_PLD, 0, true, false, MESSAGE_TYPE_COMMAND);
	//spi_server->SPIDispatch(*packet);
}
