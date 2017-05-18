/*
 * CDHStdTasks.cpp
 *
 *  Created on: Mar 8, 2016
 *      Author: Alex St. Clair
 */

#include "servers/CDHServer.h"
#include "servers/CDHStdTasks.h"
#include "servers/DispatchStdTasks.h"
#include "servers/FMGServer.h"
#include "core/Singleton.h"
#include "core/Factory.h"
#include "core/ModeManager.h"
#include "core/Dispatcher.h"
#include "util/itoa.h"
#include "util/FileHandler.h"
#include "util/Logger.h"
#include "util/TLM.h"
#include <sys/sysinfo.h>
#include <iostream>
#include <stdlib.h>
#include <cstring>
#include <string.h>
#include <sys/stat.h>

using namespace std;
using namespace AllStar::Core;

namespace AllStar{
namespace Servers{

struct stat sb;

//------------------------------------------- Message Handlers -------------------------------------------
void CDHSystemInfo(void){
	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
	struct sysinfo si;
	if(sysinfo(&si) != 0){
		logger->Log(LOGGER_LEVEL_ERROR, "CDHStdTasks: CDHCPUUsage(): Error");
		return;
	}

	CDHServer * cdhServer = dynamic_cast<CDHServer *> (Factory::GetInstance(CDH_SERVER_SINGLETON));
	cdhServer->CDHState.cpu1  = si.loads[0]/6553.6;
	cdhServer->CDHState.cpu5  = si.loads[1]/6553.6;
	cdhServer->CDHState.cpu15 = si.loads[2]/6553.6;
	cdhServer->CDHState.memory = 100.0*(259964928.0 - ((float) si.freeram)) / (259964928.0); //hard-coded total ram: 100*(total - free)/total = percent use

	logger->Log(LOGGER_LEVEL_INFO, "CDHCPUUsage(): Checking loads");
	logger->Log(LOGGER_LEVEL_DEBUG, "    CPU  1 min: %f", cdhServer->CDHState.cpu1);
	logger->Log(LOGGER_LEVEL_DEBUG, "    CPU  5 min: %f", cdhServer->CDHState.cpu5);
	logger->Log(LOGGER_LEVEL_DEBUG, "    CPU 15 min: %f", cdhServer->CDHState.cpu15);
	logger->Log(LOGGER_LEVEL_DEBUG, "    CPU memory: %f", cdhServer->CDHState.memory);

	TLM_CDH_HEALTH_AND_STATUS(getTimeInSec(),cdhServer->CDHState.cpu1,cdhServer->CDHState.cpu5,cdhServer->CDHState.cpu15,cdhServer->CDHState.memory);
}

void CDHTempStart(void){
	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));

	// Start all of the sensors
	bool sensor = false;
	bool someSuccess = false;
	bool allSuccess = true;
	for(uint8 bus = 0; bus < 4; bus++){
		for(uint8 sensor = 0; sensor < 16; sensor++){
			printf("sensor %u, %u\n", bus, sensor);
			sensor = StartTempSensor(bus+1,sensor);
			someSuccess |= sensor;
			allSuccess &= sensor;
		}
	}

	if(allSuccess){
		logger->Log(LOGGER_LEVEL_DEBUG, "CDHStdTasks: started all temp sensors");
	}else if(someSuccess){
		logger->Log(LOGGER_LEVEL_WARN, "CDHStdTasks: error starting some temp sensors");
	}else{
		logger->Log(LOGGER_LEVEL_ERROR, "CDHStdTasks: error starting all temp sensors");
	}
}

void CDHTempRead(float tempArray[4][16]){
	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
	logger->Log(LOGGER_LEVEL_DEBUG, "CDHStdTasks: CDHTempRead(): Reading temp sensors");

	// Read and add to list
	CDHServer * cdhServer = dynamic_cast<CDHServer *> (Factory::GetInstance(CDH_SERVER_SINGLETON));
	for(uint8 bus = 0; bus < 4; bus++){
		for(uint8 sensor = 0; sensor < 16; sensor++){
			tempArray[bus][sensor] = ReadTempSensor(bus+1,sensor);
		}
	}
}

// Helper Functions ---------------------------------------------------------------
bool StartTempSensor(int bus, int sensor){
	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
	// create filename
	char * temp = new char[1];
	string start = "echo 1 > /sys/bus/w1/devices/w1_bus_master";
	itoa(bus, temp, 10);
	start.append(temp);
	start.append("/TEMP");
	itoa(sensor, temp, 10);
	start.append(temp);
	start.append("/start");
	delete temp;

	// start sensor
	if(system(start.c_str()) == -1){
		logger->Log(LOGGER_LEVEL_DEBUG, "CDHStdTasks: StartTempSensor(): Error Starting Sensor!");
		return false;
	}
	return true;
}

float ReadTempSensor(int bus, int sensor){
	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
	// create filename
	char * temp = new char[1];
	string read = "/sys/bus/w1/devices/w1_bus_master";
	itoa(bus, temp, 10);
	read.append(temp);
	read.append("/TEMP");
	itoa(sensor, temp, 10);
	read.append(temp);
	read.append("/temp");
	delete temp;

	FILE * fp;
	fp = fopen(read.c_str(), "r");

	bool isGood = false;
	if(fp)
	{
		char * c = new char[1];
		char * tempRead = new char[9];
		int tempHold;
		int count = 0;
		float temperature;

		// Get temperature part of string
		while((*c = fgetc(fp)) != '\n')
		{
			tempRead[count] = *c;
			count++;
		}

		// Get float value
		sscanf(tempRead, "t=%d", &tempHold);
		temperature = (float) tempHold / 1000.0;
		//cout<<"Current Temperature: "<<temperature<<endl;

		// Check validity
		for(int i = 0; i < 28; i++){
			*c = fgetc(fp);
			if(i==27){
				if(*c=='V'){
					isGood = true;
				}
			}
		}

		// Act on validity
		if(isGood){
			logger->Log(LOGGER_LEVEL_DEBUG, "CDHStdTasks: ReadTempSensor: Good data from sensor %d on bus %d", sensor, bus);
			delete c;
			delete tempRead;
			fclose(fp);
			return temperature;
		}else{
			logger->Log(LOGGER_LEVEL_DEBUG, "CDHStdTasks: ReadTempSensor: Bad data from sensor %d on bus %d!", sensor, bus);
			delete c;
			delete tempRead;
			fclose(fp);
			return -300;
		}
	}else{
		logger->Log(LOGGER_LEVEL_DEBUG, "CDHStdTasks: ReadTempSensor: Error opening file: sensor %d on bus %d", sensor, bus);
		return -301;
	}
}

void prepPowerGPIOs(void){
	// COM
	if(!(stat("/sys/class/gpio/pioB15/", &sb) == 0 && S_ISDIR(sb.st_mode))){
		system("echo 47 > \"/sys/class/gpio/export\"");
		system("echo \"out\" > \"/sys/class/gpio/pioB15/direction\"");
	}

	// ACS
	if(!(stat("/sys/class/gpio/pioB25/", &sb) == 0 && S_ISDIR(sb.st_mode))){
		system("echo 57 > \"/sys/class/gpio/export\"");
		system("echo \"out\" > \"/sys/class/gpio/pioB25/direction\"");
	}

	// PLD
	if(!(stat("/sys/class/gpio/pioB17/", &sb) == 0 && S_ISDIR(sb.st_mode))){
		system("echo 49 > \"/sys/class/gpio/export\"");
		system("echo \"out\" > \"/sys/class/gpio/pioB17/direction\"");
	}

	// GPS
	if(!(stat("/sys/class/gpio/pioB27/", &sb) == 0 && S_ISDIR(sb.st_mode))){
		system("echo 59 > \"/sys/class/gpio/export\"");
		system("echo \"out\" > \"/sys/class/gpio/pioB27/direction\"");
	}

	// AUXCOM
	if(!(stat("/sys/class/gpio/pioB11/", &sb) == 0 && S_ISDIR(sb.st_mode))){
		system("echo 43 > \"/sys/class/gpio/export\"");
		system("echo \"out\" > \"/sys/class/gpio/pioB11/direction\"");
	}

	//-----------------------RESET LINES--------------------------------------
	//EPS
	if(!(stat("/sys/class/gpio/pioE10/", &sb) == 0 && S_ISDIR(sb.st_mode))){
		system("echo 138 > \"/sys/class/gpio/export\"");
		system("echo \"out\" > \"/sys/class/gpio/pioE10/direction\"");
	}

	//COM
	if(!(stat("/sys/class/gpio/pioA11/", &sb) == 0 && S_ISDIR(sb.st_mode))){
		system("echo 11 > \"/sys/class/gpio/export\"");
		system("echo \"out\" > \"/sys/class/gpio/pioA11/direction\"");
	}
	system("echo 1 > \"/sys/class/gpio/pioA11/value\"");

	//ACS
	if(!(stat("/sys/class/gpio/pioA12/", &sb) == 0 && S_ISDIR(sb.st_mode))){
		system("echo 12 > \"/sys/class/gpio/export\"");
		system("echo \"out\" > \"/sys/class/gpio/pioA12/direction\"");
	}

	//PLD
	if(!(stat("/sys/class/gpio/pioE11/", &sb) == 0 && S_ISDIR(sb.st_mode))){
		system("echo 139 > \"/sys/class/gpio/export\"");
		system("echo \"out\" > \"/sys/class/gpio/pioE11/direction\"");
	}
}

void toggleSubPower(HardwareLocationIDType subsystem, bool state){
	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
	string cmd = "echo ";

	if(state){
		cmd.append("1 > \"/sys/class/gpio/pioB");
	}else{
		cmd.append("0 > \"/sys/class/gpio/pioB");
	}

	switch(subsystem){
	case HARDWARE_LOCATION_COM:
		logger->Log(LOGGER_LEVEL_INFO, "CDHStdTasks: Toggling power on COM");
		cmd.append("15/value\"");
		break;
	case HARDWARE_LOCATION_ACS:
		logger->Log(LOGGER_LEVEL_INFO, "CDHStdTasks: Toggling power on ACS");
		cmd.append("25/value\"");
		break;
	case HARDWARE_LOCATION_PLD:
		logger->Log(LOGGER_LEVEL_INFO, "CDHStdTasks: Toggling power on PLD");
		cmd.append("17/value\"");
		break;
	case HARDWARE_LOCATION_GPS:
		logger->Log(LOGGER_LEVEL_INFO, "CDHStdTasks: Toggling power on GPS");
		cmd.append("27/value\"");
		break;
	default:
		logger->Log(LOGGER_LEVEL_WARN, "CDHStdTasks: Invalid subsystem for power toggle");
		break;
	}

	system(cmd.c_str());
}

void toggleResetLine(HardwareLocationIDType subsystem, bool state){
	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
	string cmd = "echo ";

	if(state){
		cmd.append("1 > \"/sys/class/gpio/pio");
	}else{
		cmd.append("0 > \"/sys/class/gpio/pio");
	}

	switch(subsystem){
	case HARDWARE_LOCATION_EPS:
		logger->Log(LOGGER_LEVEL_INFO, "CDHStdTasks: Resetting EPS");
		cmd.append("E10/value\"");
		break;
	case HARDWARE_LOCATION_COM:
		logger->Log(LOGGER_LEVEL_INFO, "CDHStdTasks: Resetting COM");
		cmd.append("A11/value\"");
		break;
	case HARDWARE_LOCATION_ACS:
		logger->Log(LOGGER_LEVEL_INFO, "CDHStdTasks: Resetting ACS");
		cmd.append("A12/value\"");
		break;
	case HARDWARE_LOCATION_PLD:
		logger->Log(LOGGER_LEVEL_INFO, "CDHStdTasks: Resetting PLD");
		cmd.append("E11/value\"");
		break;
	default:
		logger->Log(LOGGER_LEVEL_WARN, "CDHStdTasks: Invalid subsystem for reset");
		return;
	}

	system(cmd.c_str());
}

}
}
