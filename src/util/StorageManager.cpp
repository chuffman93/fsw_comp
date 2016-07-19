/*
 * StorageManager.cpp
 * Written by: Alex St. Clair
 * Created: 7/18/16
 */

#include "util/StorageManager.h"
#include "util/Logger.h"
#include "servers/GPSServer.h"
#include <sys/statvfs.h>

using namespace Phoenix::Core;
using namespace Phoenix::Servers;

StorageManager::StorageManager(float threshold_in){
	GPSServer * gpsServer = dynamic_cast<GPSServer *> (Factory::GetInstance(GPS_SERVER_SINGLETON));
	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));

	logger->Log("StorageManager started using GPS time as threshold", LOGGER_LEVEL_INFO);

	storage_threshold = threshold_in;
	last_week = gpsServer->GetWeek();
	last_seconds = gpsServer->GetSeconds();
	errorMode = false;
}

StorageManager::StorageManager(int week_start, float seconds_start, float threshold_in){
	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));

	logger->Log("StorageManager started using input threshold", LOGGER_LEVEL_INFO);

	storage_threshold = threshold_in;
	last_week = week_start;
	last_seconds = seconds_start;
	errorMode = false;

	logger->Log("StorageManager week: %d", last_week, LOGGER_LEVEL_INFO);
	logger->Log("StorageManager secs: %f", last_seconds, LOGGER_LEVEL_INFO);
}

void StorageManager::CheckAndClean(void){
	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));

	if(!errorMode){
		if(!CheckStorage()){
			logger->Log("StorageManager: storage threshold reached!", LOGGER_LEVEL_WARN);
			uint8 count = 0;
			while(count++ < 10){
				CleanFiles();
				if(CheckStorage()){
					logger->Log("StorageManager: successfully cleaned storage", LOGGER_LEVEL_INFO);
					return;
				}
			}
			logger->Log("StorageManager: cannot clean fs, entering error mode!", LOGGER_LEVEL_FATAL);
			errorMode = true;
		}
	}
}

// TODO: update with SD cards
bool StorageManager::CheckStorage(){
	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));

	if(statvfs((char *) "/", &svfs) != 0){
		// TODO: handle this
		logger->Log("StorageManager: error reading svfs", LOGGER_LEVEL_ERROR);
		return true;
	}

	uint32 free = svfs.f_bfree;

	return true;
}

void StorageManager::CleanFiles(void){
	// do stuff
}
