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
#include "servers/Structs.h"
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
		: SubsystemServer(nameIn, idIn), Singleton(), surpriseCOM(false), resetRequest(false), lastWakeTime(0), modeEnterTime(0), lastBusEnter(0), itemEntered(false)
{ }

SCHServer::~SCHServer() { }

SCHServer & SCHServer::operator=(const SCHServer & source) {
	if (this == &source) {
		return *this;
	}

	SubsystemServer::operator =(source);

	return *this;
}

void SCHServer::Initialize(void) { }

#ifdef TEST
void SCHServer::Destroy(void) { }
#endif

bool SCHServer::IsFullyInitialized(void) {
	return(Singleton::IsFullyInitialized());
}

void SCHServer::RequestCOMMode(void) {
	surpriseCOM = true;
}

void SCHServer::RequestReset(void) {
	resetRequest = true;
}

bool SCHServer::LoadDefaultScheduleConfigurations(char * filename) {
	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));

	FILE * fp = fopen(filename, "rb");
	uint8 buffer[SCHItem::size * SCHEDULE_MAX_SIZE];

	// make sure we get a valid file pointer
	if (fp == NULL) {
		logger->Log(LOGGER_LEVEL_ERROR, "SCHServer: NULL default schedule file pointer, cannot boot");
		SetDefaultBusSchedule();
		return false;
	}

	// check file size (and reset fp to beginning)
	fseek(fp,0,SEEK_END);
	int file_size = ftell(fp);
	fseek(fp,0,SEEK_SET);

	if (file_size == 0 || (file_size % SCHItem::size) != 0) {
		logger->Log(LOGGER_LEVEL_ERROR, "SCHServer: Invalid default schedule file size");
		SetDefaultBusSchedule();
		fclose(fp);
		return false;
	}

	// read and update the configs
	size_t bytes_read = fread(buffer, sizeof(uint8), file_size, fp);
	if (bytes_read == file_size) {
		defaultSchedule.clear();

		// deserialize and check each schedule item
		for (uint8 i = 0; i < bytes_read / SCHItem::size; i++) {
			DefaultArray[i].update(buffer, bytes_read, SCHItem::size*i, SCHItem::size*i);
			DefaultArray[i].deserialize();
			DefaultArray[i].timeout += getTimeInSec();
			if ((DefaultArray[i].enter_mode < 0 || DefaultArray[i].enter_mode > 1) ||
					(DefaultArray[i].mode < 0 || DefaultArray[i].mode > MODE_NUM_MODES)) {
				// error in file
				logger->Log(LOGGER_LEVEL_ERROR, "SCHServer: default schedule item out of range");
				SetDefaultBusSchedule();
				fclose(fp);
				return false;
			}
		}

		// push the new items to the default schedule
		UpdateDefaultSchedule(bytes_read / SCHItem::size);

		logger->Log(LOGGER_LEVEL_INFO, "SCHServer: successfully loaded default config");
		fclose(fp);
		return true;
	} else {
		logger->Log(LOGGER_LEVEL_ERROR, "SCHServer: error reading default schedule");
		SetDefaultBusSchedule();
		fclose(fp);
		return false;
	}
}

void SCHServer::UpdateDefaultSchedule(int numItems) {
	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));

	for (uint8 j = 0; j < 5; j++) {
		if (this->TakeLock(MAX_BLOCK_TIME)) {
			for (uint8 i = 0; i < numItems; i++) {
				defaultSchedule.push_back(DefaultArray[i]);
			}
			this->GiveLock();
			return;
		}
	}

	logger->Log(LOGGER_LEVEL_ERROR, "SCHServer: unable to update default schedule");
	// TODO: handle this
}

void SCHServer::SetDefaultBusSchedule(void) {
	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));

	SCHItem defaultItem(0,0,-1,1,1,MODE_BUS_PRIORITY,300);
	for (uint8 i = 0; i < 5; i++) {
		if (this->TakeLock(MAX_BLOCK_TIME)) {
			defaultSchedule.clear();
			defaultSchedule.push_back(defaultItem);
			this->GiveLock();
			return;
		}
	}

	logger->Log(LOGGER_LEVEL_ERROR, "SCHServer: unable to set default bus due to lock!");
	// TODO: handle this
}

void SCHServer::SubsystemLoop(void) {
	ModeManager * modeManager = dynamic_cast<ModeManager *> (Factory::GetInstance(MODE_MANAGER_SINGLETON));
	WatchdogManager * wdm = dynamic_cast<WatchdogManager *> (Factory::GetInstance(WATCHDOG_MANAGER_SINGLETON));
	GPSServer * gpsServer = dynamic_cast<GPSServer *>(Factory::GetInstance(GPS_SERVER_SINGLETON));
	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));

	modeManager->SetMode(MODE_BUS_PRIORITY);
	LoadDefaultScheduleConfigurations((char *) SCH_CONFIG);

	currentSchedule = std::list<SCHItem>();
	itemEntered = false;
	lastBusEnter = getTimeInSec();

	while(1) {
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
			SCHItem newCOM(0,0,-1,1,1,MODE_COM,90); // TODO: make this longer for flight
			currentSchedule.push_front(newCOM);
		}

		if(resetRequest){
			resetRequest = false;
			modeManager->SetMode(MODE_RESET);
			SCHItem resetItem(0,0,-1,1,1,MODE_RESET,30);
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
					modeManager->SetMode((SystemModeEnum) currentEvent.mode);
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
						modeManager->SetMode((SystemModeEnum) currentEvent.mode);
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

	currentSchedule = defaultSchedule;
	logger->Log(LOGGER_LEVEL_INFO, "Default Schedule Loaded");
	remove(SCHEDULE_FILE);
}

SCHItem SCHServer::ParseLine(string line){
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

	if(access(SCHEDULE_FILE, F_OK) == -1){
		logger->Log(LOGGER_LEVEL_WARN, "LoadNextSchedule: No new schedule, loading default...");
		LoadDefaultSchedule();
		return -1;
	}

	FILE * fp = fopen(SCHEDULE_FILE, "r");
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
		remove(SCHEDULE_FILE);
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
	remove(SCHEDULE_FILE);

	return 1;
}

} // End of namespace Servers
} // End of namespace AllStar
