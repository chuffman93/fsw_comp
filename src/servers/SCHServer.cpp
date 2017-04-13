/*
* SCHServer.cpp
*
*  Created on: July 7, 2016
*      Author: Robert Belter
*/
#include "servers/SCHServer.h"
#include "servers/GPSServer.h"
#include "servers/CMDServer.h"
#include "servers/FileSystem.h"

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
		: SubsystemServer(nameIn, idIn), Singleton(), configManager(SCH_CONFIG), surpriseCOM(false), resetRequest(false), lastWakeTime(0), modeEnterTime(0), lastBusEnter(0), itemEntered(false)
{

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

void SCHServer::RequestCOMMode(void){
	surpriseCOM = true;
}

void SCHServer::RequestReset(void){
	resetRequest = true;
}

//void SCHServer::EnterNextMode(void){
//	ModeManager * modeManager = dynamic_cast<ModeManager *> (Factory::GetInstance(MODE_MANAGER_SINGLETON));
//	SystemModeEnum currentMode = modeManager->GetMode();
//
//	if(currentMode == MODE_BUS_PRIORITY && (lastBusEnter + 10 < getTimeInSec())){ // make sure we've been in bus priority mode for >10 seconds
//		modeManager->SetMode(currentEvent.mode);
//		itemEntered = true;
//		modeEnterTime = getTimeInSec();
//	}else if(currentMode != MODE_BUS_PRIORITY){
//		modeManager->SetMode(MODE_BUS_PRIORITY);
//		lastBusEnter = getTimeInSec();
//	}
//}
//
//void SCHServer::EnterBusMode(void){
//	ModeManager * modeManager = dynamic_cast<ModeManager *> (Factory::GetInstance(MODE_MANAGER_SINGLETON));
//
//	modeManager->SetMode(MODE_BUS_PRIORITY);
//	lastBusEnter = getTimeInSec();
//	currentSchedule.pop_front();
//	itemEntered = false;
//}

int SCHServer::LoadDefaultScheduleConfigurations(void){
	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
	configManager.LoadConfig();
	defaultSchedule.clear();

	// reload the schedule
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
		defaultSchedule.back().timeout += getTimeInSec(); // TODO: delete this to implement absolute timing
	}
	return 1;
}

void SCHServer::SubsystemLoop(void)
{
	ModeManager * modeManager = dynamic_cast<ModeManager *> (Factory::GetInstance(MODE_MANAGER_SINGLETON));
	WatchdogManager * wdm = dynamic_cast<WatchdogManager *> (Factory::GetInstance(WATCHDOG_MANAGER_SINGLETON));
	GPSServer * gpsServer = dynamic_cast<GPSServer *>(Factory::GetInstance(GPS_SERVER_SINGLETON));
	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));

	modeManager->SetMode(MODE_BUS_PRIORITY);

	currentSchedule = std::list<SCHItem>();
	itemEntered = false;
	lastBusEnter = getTimeInSec();

	while(1)
	{
		lastWakeTime = getTimeInMillis();
		wdm->Kick();

		if (currentSchedule.empty()){
			logger->Log(LOGGER_LEVEL_INFO, "Fetching next schedule");
			LoadNextSchedule();
		}

		if(surpriseCOM){
			surpriseCOM = false;
			modeManager->SetMode(MODE_BUS_PRIORITY);
			if(itemEntered){
				itemEntered = false;
				currentSchedule.pop_front();
			}
			SCHItem newCOM = {0,0,-1,1,1,MODE_COM,90}; // TODO: make this longer for flight
			currentSchedule.push_front(newCOM);
		}

		if(resetRequest){
			resetRequest = false;
			modeManager->SetMode(MODE_RESET);
			SCHItem resetItem = {0,0,-1,1,1,MODE_RESET,30};
			itemEntered = false;
			currentSchedule.push_front(resetItem);
		}

		SCHItem currentEvent = currentSchedule.front();

		if (itemEntered){
			if (currentEvent.duration < getTimeInSec() - modeEnterTime){
				logger->Log(LOGGER_LEVEL_INFO, "Schedule event duration exceeded, entering Bus Priority Mode");
				modeManager->SetMode(MODE_BUS_PRIORITY);
				lastBusEnter = getTimeInSec();
				currentSchedule.pop_front();
				itemEntered = false;
			}
		}
		else {
			bool inRange = gpsServer->DistanceTo(currentEvent.latitude, currentEvent.longitude) < currentEvent.radius;
			if (inRange){
				logger->Log(LOGGER_LEVEL_INFO, "In range of schedule event, setting mode");
				SystemModeEnum currentMode = modeManager->GetMode();

				if(currentMode == MODE_BUS_PRIORITY && (lastBusEnter + 10 < getTimeInSec())){ // make sure we've been in bus priority mode for >10 seconds
					modeManager->SetMode(currentEvent.mode);
					itemEntered = true;
					modeEnterTime = getTimeInSec();
				}else if(currentMode != MODE_BUS_PRIORITY){
					modeManager->SetMode(MODE_BUS_PRIORITY);
					lastBusEnter = getTimeInSec();
				}
			}
			else if (currentEvent.timeout < getTimeInSec()){
				if (currentEvent.enter_mode){
					logger->Log(LOGGER_LEVEL_INFO, "Schedule event timeout exceeded, entering mode");
					SystemModeEnum currentMode = modeManager->GetMode();

					if(currentMode == MODE_BUS_PRIORITY && (lastBusEnter + 10 < getTimeInSec())){ // make sure we've been in bus priority mode for >10 seconds
						modeManager->SetMode(currentEvent.mode);
						itemEntered = true;
						modeEnterTime = getTimeInSec();
					}else if(currentMode != MODE_BUS_PRIORITY){
						modeManager->SetMode(MODE_BUS_PRIORITY);
						lastBusEnter = getTimeInSec();
					}
				}
				else {
					logger->Log(LOGGER_LEVEL_INFO, "Schedule event timeout exceeded, skipping mode");
					modeManager->SetMode(MODE_BUS_PRIORITY);
					lastBusEnter = getTimeInSec();
					currentSchedule.pop_front();
					itemEntered = false;
				}
			}
		}
		waitUntil(lastWakeTime, 1000);
	}
}

void SCHServer::LoadDefaultSchedule(){
	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));

	LoadDefaultScheduleConfigurations();
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
	newSchedule.timeout = atoi(line.substr(25,8).c_str()) + getTimeInSec(); // TODO: Change to 4 bytes after serializing, change to absolute (currently relative to start of schedule)
	newSchedule.mode = static_cast<SystemModeEnum> (atoi(line.substr(33,1).c_str()));
	newSchedule.duration = atoi(line.substr(34,8).c_str()); // TODO: Change to 4 bytes after serializing

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

	// check password
	bytesRead = getline(&line, &len, fp);
	if(strcmp(line,UPLK_PASSWORD) != 0){
		logger->Log(LOGGER_LEVEL_ERROR, "LoadNextSchedule: invalid SCH password");
		fclose(fp);
		remove(SCH_SCHEDULE_FILE);
		LoadDefaultSchedule();
		return -4;
	}

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
