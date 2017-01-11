/*
 *  Diagnostics.cpp
 *
 *  Created on: September 15, 2016
 *      Author: Alex St. Clair
 *
 *  This file contains all of the tests that can be run in diagnostic mode. When tests are
 *  added, change the NUM_TESTS define statement in CDHStdTasks.cpp to reflect the new number,
 *  and then add the tests to the runDiagnostic() function
 */

#include "util/Diagnostics.h"
#include "util/Logger.h"
#include "servers/FMGServer.h"
#include "servers/DispatchStdTasks.h"
#include "HAL/SPI_Server.h"

using namespace AllStar::Core;
using namespace AllStar::Servers;

void BusLoadTest(bool enable){
	if(enable){
		Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
		FMGServer * fmgServer = dynamic_cast<FMGServer *> (Factory::GetInstance(FMG_SERVER_SINGLETON));
		logger->Log(LOGGER_LEVEL_INFO, "Running bus load test");
		fmgServer->Log(LOCATION_DGN, "0x0100");
	}
}

void SPIStats(bool enable){
	if(enable){
		Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
		FMGServer * fmgServer = dynamic_cast<FMGServer *> (Factory::GetInstance(FMG_SERVER_SINGLETON));
		logger->Log(LOGGER_LEVEL_INFO, "Running SPI Statistics");
		fmgServer->Log(LOCATION_DGN, "0x0101");

		SPI_HALServer * spiServer = dynamic_cast<SPI_HALServer *> (Factory::GetInstance(SPI_HALSERVER_SINGLETON));
		uint64 duration = spiServer->ResetStatsTime();

		FILE * fp = fopen("diagnostic_results.txt", "a+");
		fprintf(fp, "SPI Stats:\n\tPackets Dropped: %lu %lu %lu %lu\n",
				spiServer->packetsDroppedTX[0], spiServer->packetsDroppedTX[1], spiServer->packetsDroppedTX[2], spiServer->packetsDroppedTX[3]);
		fprintf(fp, "\tPackets Sent: %lu %lu %lu %lu\n",
				spiServer->packetsSentTX[0], spiServer->packetsSentTX[1], spiServer->packetsSentTX[2], spiServer->packetsSentTX[3]);
		fprintf(fp, "\tBytes Dropped: %llu %llu %llu %llu\n",
				spiServer->bytesDroppedTX[0], spiServer->bytesDroppedTX[1], spiServer->bytesDroppedTX[2], spiServer->bytesDroppedTX[3]);
		fprintf(fp, "\tBytes Sent: %llu %llu %llu %llu\n",
				spiServer->bytesSentTX[0], spiServer->bytesSentTX[1], spiServer->bytesSentTX[2], spiServer->bytesSentTX[3]);
		fprintf(fp, "\tDuration: %llu ms\n", duration);
		fclose(fp);
		for(uint8 i = 0; i < 4; i++){
			spiServer->packetsDroppedTX[i] = 0;
			spiServer->packetsSentTX[i] = 0;
			spiServer->bytesDroppedTX[i] = 0;
			spiServer->bytesSentTX[i] = 0;
		}
	}
}

void SampleTest2(bool enable){
	if(enable){
		Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
		FMGServer * fmgServer = dynamic_cast<FMGServer *> (Factory::GetInstance(FMG_SERVER_SINGLETON));
		logger->Log(LOGGER_LEVEL_INFO, "Running Sample Test 2");
		fmgServer->Log(LOCATION_DGN, "0x0102");

		FILE * fp = fopen("diagnostic_results.txt", "a+");
		char results[] = "Ran sample test 2\n";
		fwrite(results, 1, sizeof(results), fp);
		fclose(fp);
	}
}

void SampleTest3(bool enable){
	if(enable){
		Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
		FMGServer * fmgServer = dynamic_cast<FMGServer *> (Factory::GetInstance(FMG_SERVER_SINGLETON));
		logger->Log(LOGGER_LEVEL_INFO, "Running Sample Test 3");
		fmgServer->Log(LOCATION_DGN, "0x0103");

		FILE * fp = fopen("diagnostic_results.txt", "a+");
		char results[] = "Ran sample test 3\n";
		fwrite(results, 1, sizeof(results), fp);
		fclose(fp);
	}
}
