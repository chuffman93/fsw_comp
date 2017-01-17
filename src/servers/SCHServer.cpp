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
#include <list>
#include <iterator>

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

int SCHServer::SetNewMode(void){
	ModeManager * modeManager = dynamic_cast<ModeManager *> (Factory::GetInstance(MODE_MANAGER_SINGLETON));

	currentSchedule.pop_front();
	if (currentSchedule.empty()) {
		modeManager->SetMode(MODE_BUS_PRIORITY);
		return -1;
	}
	else {
		modeManager->SetMode(currentSchedule.front().mode);
		return 1;
	}
}

int SCHServer::LoadDefaultScheduleConfigurations(void){
	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
	configManager.LoadConfig();
	for (int i = 0; i < configManager.config.sizeOfDefaultSchedule; i++) {
		logger->Log(LOGGER_LEVEL_INFO, "Added item to default schedule: {%f, %f, %f, %d, %d %d, %d}",
				configManager.config.defaultScheduleArray[i].latitude,
				configManager.config.defaultScheduleArray[i].longitude,
				configManager.config.defaultScheduleArray[i].radius,
				configManager.config.defaultScheduleArray[i].enter_mode,
				configManager.config.defaultScheduleArray[i].timeout,
				configManager.config.defaultScheduleArray[i].mode,
				configManager.config.defaultScheduleArray[i].duration);
		defaultSchedule.push_back(configManager.config.defaultScheduleArray[i]);
	}
	return 1;
}

void SCHServer::SubsystemLoop(void)
{
	ModeManager * modeManager = dynamic_cast<ModeManager *> (Factory::GetInstance(MODE_MANAGER_SINGLETON));
	WatchdogManager * wdm = dynamic_cast<WatchdogManager *> (Factory::GetInstance(WATCHDOG_MANAGER_SINGLETON));
	GPSServer * gpsServer = dynamic_cast<GPSServer *>(Factory::GetInstance(GPS_SERVER_SINGLETON));
	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));

	uint64_t LastTimeSwitched = getTimeInMillis();
	currentSchedule = std::list<SCHItem>();

	LoadDefaultScheduleConfigurations();

	while(1)
	{
		int64 LastWakeTime = getTimeInMillis();
		wdm->Kick();

		if (currentSchedule.empty()){
			logger->Log(LOGGER_LEVEL_INFO, "Fetching next schedule");
			LoadNextSchedule();
		}

		SCHItem CurrentEvent = currentSchedule.front();

		bool inRange = gpsServer->DistanceTo(CurrentEvent.latitude, CurrentEvent.longitude) < CurrentEvent.radius;
		if (inRange){
			SetNewMode();
		}
		else if (CurrentEvent.timeout < getTimeInSec()){
			if (CurrentEvent.enter_mode){
				logger->Log(LOGGER_LEVEL_INFO, "Schedule event timeout exceeded, entering mode");
				SetNewMode();
			}
			else {
				logger->Log(LOGGER_LEVEL_INFO, "Schedule event timeout exceeded, entering Bus Priority Mode and fetching next schedule");
				modeManager->SetMode(MODE_BUS_PRIORITY);
				currentSchedule.pop_front();
				LoadNextSchedule();
			}
		}
		else if (CurrentEvent.duration < getTimeInSec()) {
			logger->Log(LOGGER_LEVEL_INFO, "Schedule event duration exceeded, entering Bus Priority Mode");
			modeManager->SetMode(MODE_BUS_PRIORITY);
		}

		waitUntil(LastWakeTime, 1000);
	}
}

void SCHServer::LoadDefaultSchedule(){
	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));

	currentSchedule = defaultSchedule;
	logger->Log(LOGGER_LEVEL_INFO, "Default Schedule Loaded");
	remove(SCH_SCHEDULE_FILE);
}

SCHServer::SCHItem SCHServer::ParseLine(string line){
	SCHItem newSchedule;

	// TODO: Error bounds
	newSchedule.latitude = atof(line.substr(0,8).c_str());
	newSchedule.longitude = atof(line.substr(8,8).c_str());
	newSchedule.radius = atof(line.substr(16,8).c_str());
	newSchedule.enter_mode = atoi(line.substr(24,1).c_str());
	newSchedule.timeout = atoi(line.substr(25,8).c_str()); // TODO: Change to 4 bytes after serializing
	newSchedule.mode = static_cast<SystemModeEnum> (atoi(line.substr(33,1).c_str()));
    int dur = atoi(line.substr(34,8).c_str()); // TODO: Change to 4 bytes after serializing
	newSchedule.duration = getTimeInSec() + dur;

	return newSchedule;
}

int SCHServer::LoadNextSchedule(){
	// TODO: What to do on reboot?
	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
	currentSchedule = std::list<SCHItem>();

	if(access(SCH_SCHEDULE_FILE, F_OK) == -1){
		logger->Log(LOGGER_LEVEL_WARN, "LoadNextSchedule: No new schedule, loading default...");
		LoadDefaultSchedule();
		return -1;
	}

	FILE * fp = fopen(SCH_SCHEDULE_FILE, "r");
	if(fp == NULL){
		logger->Log(LOGGER_LEVEL_WARN, "LoadNextSchedule: Failed to open new schedule file, loading default...");
		LoadDefaultSchedule();
		return -2;
	}

	char * line = NULL;
	size_t len = 0;
	ssize_t bytesRead;

	// TODO: NEED TO CHANGE THIS TO RAW INSTEAD OF ASCII!!!!
	while ((bytesRead = getline(&line, &len, fp)) != -1) {
		if(bytesRead != 43){ // TODO: Does this actually work?
			logger->Log(LOGGER_LEVEL_WARN, "LoadNextSchedule: New schedule file is improperly formatted, loading default...");
			LoadDefaultSchedule();
			return -3;
		}

		currentSchedule.push_back(ParseLine(string(line)));
	}

	logger->Log(LOGGER_LEVEL_INFO, "LoadNextSchedule: New schedule loaded");
	remove(SCH_SCHEDULE_FILE);

	return 1;
}

} // End of namespace Servers
} // End of namespace AllStar
