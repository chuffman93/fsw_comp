/*
* SCHServer.cpp
*
*  Created on: July 7, 2016
*      Author: Robert Belter
*/
#include "servers/SCHServer.h"
#include "servers/GPSServer.h"
#include "servers/DispatchStdTasks.h"

#include "core/Dispatcher.h"
#include "core/WatchdogManager.h"
#include "core/StdTypes.h"
#include "core/ModeManager.h"
#include "util/Logger.h"

#include <sys/mman.h>
#include <stdio.h>

using namespace std;
using namespace AllStar::Core;

namespace AllStar{
namespace Servers{

SCHServer::SCHServer(string nameIn, LocationIDType idIn)
		: SubsystemServer(nameIn, idIn), Singleton(), arby(idIn), configManager(SCH_CONFIG_FILE)
{
	scheduleRunning = false;
	//TODO Setup default schedule
}

SCHServer::~SCHServer()
{
	// Left Intentionally Blank
}

SCHServer & SCHServer::operator=(const SCHServer & source)
{
	if (this == &source)
	{
		return *this;
	}

	SubsystemServer::operator =(source);

	return *this;
}

void SCHServer::Initialize(void)
{
}

#ifdef TEST
void SCHServer::Destroy(void)
{
}
#endif

bool SCHServer::IsFullyInitialized(void)
{
	return(Singleton::IsFullyInitialized());
}

bool SCHServer::RegisterHandlers()
{
	volatile bool success = true;

	Dispatcher * dispatcher = dynamic_cast<Dispatcher *> (Factory::GetInstance(DISPATCHER_SINGLETON));
	success &= dispatcher->AddRegistry(id, &reg, &arby);

	return success;
}

void SCHServer::SubsystemLoop(void)
{
	ModeManager * modeManager = dynamic_cast<ModeManager *> (Factory::GetInstance(MODE_MANAGER_SINGLETON));
	WatchdogManager * wdm = dynamic_cast<WatchdogManager *> (Factory::GetInstance(WATCHDOG_MANAGER_SINGLETON));
	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
	uint64_t LastTimeSwitched = getTimeInMillis();
	currentSchedule = std::list<SCHItem>();

	configManager.LoadConfig();
	for(int i = 0; i < configManager.config.sizeOfDefaultSchedule; i++){
		printf("Added item to default schedule: {%f, %f, %f, %d, %d}\n",
				configManager.config.defaultScheduleArray[i].latitude,
				configManager.config.defaultScheduleArray[i].longitude,
				configManager.config.defaultScheduleArray[i].radius,
				configManager.config.defaultScheduleArray[i].timeoutms,
				configManager.config.defaultScheduleArray[i].mode);
		defaultSchedule.push_back(configManager.config.defaultScheduleArray[i]);
	}
	printf("\n");

	while(1)
	{
		int64 LastWakeTime = getTimeInMillis();
		wdm->Kick();
		if(currentSchedule.empty()){
			logger->Log(LOGGER_LEVEL_INFO, "Fetching Next Schedule");
			LoadNextSchedule();
			LastTimeSwitched = getTimeInMillis();
			modeManager->SetMode(currentSchedule.front().mode);
		}

		GPSServer * gpsServer = dynamic_cast<GPSServer *>(Factory::GetInstance(GPS_SERVER_SINGLETON));
		SCHItem CurrentEvent = currentSchedule.front();
		bool inRange = gpsServer->DistanceTo(CurrentEvent.latitude, CurrentEvent.longitude) < CurrentEvent.radius;
		bool timeout = CurrentEvent.timeoutms < (getTimeInMillis() - LastTimeSwitched);
		if(inRange || timeout){
			currentSchedule.pop_front();
			if(currentSchedule.empty()){
				modeManager->SetMode(MODE_BUS_PRIORITY);
			}else{
				modeManager->SetMode(currentSchedule.front().mode);
			}
			LastTimeSwitched = getTimeInMillis();
		}
		waitUntil(LastWakeTime, 1000);
	}
}

//void SCHServer::LoadNextSchedule(){
//	bool failure = false;
//	currentSchedule = std::list<SCHItem>();
//	if(this->TakeLock(MAX_BLOCK_TIME)){
//		if(access(SCH_SCHEDULE_FILE, F_OK) != -1){
//			FILE *pFile;
//			pFile = fopen(SCH_SCHEDULE_FILE, "r");
//			if(pFile != NULL){
//				for(int i = 0; i < SCHEDULE_MAX_SIZE; i++){
//					SCHItem tmp;
//					if(fgets((char*) &tmp, (int) sizeof(tmp), pFile) != NULL){
//						if(tmp.mode >= MODE_FIRST_MODE && tmp.mode <= MODE_NUM_MODES)
//							currentSchedule.push_back(tmp);
//						else{
//							failure = true;
//							break;
//						}
//					}else{
//						failure = true;
//						break;
//					}
//				}
//				fclose(pFile);
//				remove(SCH_SCHEDULE_FILE);
//			}else{
//				failure = true;
//			}
//		}else{
//			failure = true;
//		}
//		this->GiveLock();
//	}else{
//		failure = true;
//	}
//	if(failure){
//		currentSchedule = defaultSchedule;
//	}
//}

void SCHServer::LoadNextSchedule(){
	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
	currentSchedule = std::list<SCHItem>();

	if(access(SCH_SCHEDULE_FILE, F_OK) == -1){
		logger->Log(LOGGER_LEVEL_WARN, "LoadNextSchedule: no new schedule");
		currentSchedule = defaultSchedule;
		return;
	}

	FILE * fp = fopen(SCH_SCHEDULE_FILE, "r");
	if(fp == NULL){
		logger->Log(LOGGER_LEVEL_WARN, "LoadNextSchedule: failed to open new schedule file");
		remove(SCH_SCHEDULE_FILE);
		currentSchedule = defaultSchedule;
		return;
	}

	char * line = NULL;
	size_t len = 0;
	ssize_t bytesRead;
	while ((bytesRead = getline(&line, &len, fp)) != -1) {
		if((bytesRead == -1) || (bytesRead != 30)){

		}

	}

	// go line by line, and parse in new schedule items
}

} // End of namespace Servers
} // End of namespace AllStar
