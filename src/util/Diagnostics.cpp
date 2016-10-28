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
#include "servers/DispatchStdTasks.h"
#include "HAL/SPI_Server.h"

using namespace AllStar::Core;
using namespace AllStar::Servers;

void BusLoadTest(bool enable){
	if(enable){
		SPI_HALServer * spiServer = dynamic_cast<SPI_HALServer *> (Factory::GetInstance(SPI_HALSERVER_SINGLETON));
		Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
		logger->Log("Running bus load test", LOGGER_LEVEL_INFO);

		// Test SPI TX Speed
		ACPPacket * query = new ACPPacket(SERVER_LOCATION_EPS, HARDWARE_LOCATION_EPS, EPS_NO_RETURN);

		uint8 packetsSent = 0;
		uint8 tries = 0;
		uint64 currTime = 0;
		uint64 endTime = getTimeInMillis() + 2000;
		while(currTime < endTime){
			if(mq_size(spiServer->queueHandleTX, spiServer->queueAttrTX) == 0){
				tries++;
				if(DispatchNoResponse(query)){
					packetsSent++;
				}
				currTime = getTimeInMillis();
				usleep(10000);
			}
		}

		FILE * fp = fopen("diagnostic_results.txt", "a+");
		fprintf(fp, "Bus Load Results: %d packets sent out of %d tries in 2 seconds\n", packetsSent, tries);
		fclose(fp);
	}
}

void SPIStats(bool enable){
	if(enable){
		Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
		logger->Log("Running SPI Statistics", LOGGER_LEVEL_INFO);

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
		logger->Log("Running Sample Test 2", LOGGER_LEVEL_INFO);

		FILE * fp = fopen("diagnostic_results.txt", "a+");
		char results[] = "Ran sample test 2\n";
		fwrite(results, 1, sizeof(results), fp);
		fclose(fp);
	}
}

void SampleTest3(bool enable){
	if(enable){
		Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
		logger->Log("Running Sample Test 3", LOGGER_LEVEL_INFO);

		FILE * fp = fopen("diagnostic_results.txt", "a+");
		char results[] = "Ran sample test 3\n";
		fwrite(results, 1, sizeof(results), fp);
		fclose(fp);
	}
}
