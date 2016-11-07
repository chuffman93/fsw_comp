/*
 * StorageManager.cpp
 * Written by: Alex St. Clair
 * Created: 7/18/16
 */

#include "util/StorageManager.h"
#include "util/Logger.h"
#include "servers/GPSServer.h"
#include <sys/statvfs.h>

using namespace AllStar::Core;
using namespace AllStar::Servers;

StorageManager::StorageManager(float threshold_in){
	GPSServer * gpsServer = dynamic_cast<GPSServer *> (Factory::GetInstance(GPS_SERVER_SINGLETON));
	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));

	logger->Log(LOGGER_LEVEL_INFO, "StorageManager started using GPS time as threshold");

	storage_threshold = threshold_in;
	oldest_week = gpsServer->GetWeek();
	oldest_seconds = gpsServer->GetSeconds();
	errorMode = false;
}

StorageManager::StorageManager(int week_start, float seconds_start, float threshold_in){
	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));

	logger->Log(LOGGER_LEVEL_INFO, "StorageManager started using input threshold");

	storage_threshold = threshold_in;
	oldest_week = week_start;
	oldest_seconds = seconds_start;
	errorMode = false;

	logger->Log(LOGGER_LEVEL_INFO, "StorageManager week: %d", oldest_week);
	logger->Log(LOGGER_LEVEL_INFO, "StorageManager secs: %f", oldest_seconds);
}

int StorageManager::CheckAndClean(void){
	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));

	if(!errorMode){
		if(!CheckStorage()){
			logger->Log(LOGGER_LEVEL_WARN, "StorageManager: storage threshold reached!");
			uint8 count = 0;
			while(count++ < 10){
				CleanFiles();
				if(CheckStorage()){
					logger->Log(LOGGER_LEVEL_INFO, "StorageManager: successfully cleaned storage");
					return 1;
				}
			}
			logger->Log(LOGGER_LEVEL_FATAL, "StorageManager: cannot clean fs, entering error mode!");
			errorMode = true;
			return -1; // entering error mode
		}
		return 0; // storage not full
	}
	return 0;
}

// TODO: update with SD cards
bool StorageManager::CheckStorage(){
	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));

	if(statvfs((char *) "/media/sd1/filehandler/", &svfs) != 0){
		// TODO: handle this
		logger->Log(LOGGER_LEVEL_ERROR, "StorageManager: error reading svfs");
		return true;
	}

	return (((float) svfs.f_blocks - (float) svfs.f_bfree )/(float) svfs.f_blocks * 100.0) < storage_threshold;
}

void StorageManager::CleanFiles(void){
	GPSServer * gpsServer = dynamic_cast<GPSServer *> (Factory::GetInstance(GPS_SERVER_SINGLETON));
	string subsystems[8] = {"ACS", "COM", "EPS", "GPS", "PLD", "SCH", "CMD", "CDH"};
	string cmd;
	char * temp = new char[16];
	uint8 deleteWeeks = 2;
	uint16 current_week = gpsServer->GetWeek();

	// Determine how many weeks of data to delete
	if((current_week - oldest_week) > 20){
		deleteWeeks = 5;
	}else if((current_week - oldest_week) > 10){
		deleteWeeks = 2;
	}
	else if((current_week - oldest_week) > 5){
		deleteWeeks = 1;
	}

	// Delete data
	for(uint8 i = 0; i < deleteWeeks; i++){
		if(current_week != oldest_week){
			for(uint8 j = 0; j < 8; j++){
				cmd = "rm /media/sd1/filehandler/";
				cmd.append(subsystems[j]);
				cmd.append("/");
				cmd.append(subsystems[j]);
				cmd.append("_???_");
				sprintf(temp, "%05d", oldest_week);
				cmd.append(temp);
				cmd.append("*");
				system(cmd.c_str());
			}
			oldest_week++;
			oldest_seconds = 0;
		}
	}
}

void StorageManager::CleanFiles(uint16 weekStart, uint16 weekEnd){
	string subsystems[8] = {"ACS", "COM", "EPS", "GPS", "PLD", "SCH", "CMD", "CDH"};
	string cmd;
	char * temp = new char[16];

	for(uint16 i = weekStart; i <= weekEnd; i++){
		for(uint8 j = 0; j < 8; j++){
			cmd = "rm /media/sd1/filehandler/";
			cmd.append(subsystems[j]);
			cmd.append("/");
			cmd.append(subsystems[j]);
			cmd.append("_???_");
			sprintf(temp, "%05d", i);
			cmd.append(temp);
			cmd.append("*");
			system(cmd.c_str());
		}
	}
}
