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
	CDHServer * cdhServer = dynamic_cast<CDHServer *> (Factory::GetInstance(CDH_SERVER_SINGLETON));
	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
	struct sysinfo si;
	if(sysinfo(&si) != 0){
		logger->Log(LOGGER_LEVEL_ERROR, "CDHStdTasks: CDHCPUUsage(): Error");
		return;
	}

	float cpu1  = si.loads[0]/6553.6;
	float cpu5  = si.loads[1]/6553.6;
	float cpu15 = si.loads[2]/6553.6;
	float mem = 100.0*(259964928.0 - ((float) si.freeram)) / (259964928.0); //hard-coded total ram: 100*(total - free)/total = percent use
	cdhServer->CDHState = {cpu1,cpu5,cpu15,mem};

	logger->Log(LOGGER_LEVEL_DEBUG, "CDHCPUUsage(): Checking loads");
	logger->Log(LOGGER_LEVEL_DEBUG, "    CPU  1 min: %f", si.loads[0]/65536.0);
	logger->Log(LOGGER_LEVEL_DEBUG, "    CPU  5 min: %f", si.loads[1]/65536.0);
	logger->Log(LOGGER_LEVEL_DEBUG, "    CPU 15 min: %f", si.loads[2]/65536.0);
	logger->Log(LOGGER_LEVEL_DEBUG, "    CPU memory: %f", mem);

	FMGServer * fmgServer = dynamic_cast<FMGServer *> (Factory::GetInstance(FMG_SERVER_SINGLETON));
	char str[100];
	sprintf(str,"%ld,%f,%f,%f,%f",getTimeInSec(),cpu1,cpu5,cpu15,mem);
	fmgServer->Log(DESTINATION_HST, string(str));
}

//ACPPacket * CDHTempStart(void){
//	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
//
//	// Start all of the sensors
//	bool validStart[4][16];
//	bool success = true;
//	for(uint8 bus = 0; bus < 4; bus++){
//		for(uint8 sensor = 0; sensor < 16; sensor++){
//			validStart[bus][sensor] = StartTempSensor(bus+1,sensor);
//			success &= validStart[bus][sensor];
//		}
//	}
//
//	if(success){
//		logger->Log(LOGGER_LEVEL_INFO, "CDHStdTasks: CDHTempStart(): Started sensors");
//		ACPPacket * ret = new ACPPacket(CDH_TEMP_START_SUCCESS, true);
//		return ret;
//	}else{
//		logger->Log(LOGGER_LEVEL_ERROR, "CDHStdTasks: CDHTempStart(): Error starting sensors!");
//		ACPPacket * ret = new ACPPacket(CDH_TEMP_START_FAILURE, false);
//		return ret;
//	}
//}

//ACPPacket * CDHTempRead(void){
//	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
//	logger->Log(LOGGER_LEVEL_INFO, "CDHStdTasks: CDHTempRead(): Reading temp sensors");
//
//	// Setup
//	float temperatures[4][16];
//	uint8 * buffer = (uint8 *) malloc(64*sizeof(float));
//
//	// Read and add to list
//	for(uint8 bus = 0; bus < 4; bus++){
//		for(uint8 sensor = 0; sensor < 16; sensor++){
//			temperatures[bus][sensor] = ReadTempSensor(bus+1,sensor);
//			AddFloat(buffer + 4*(16*bus + sensor), temperatures[bus][sensor]);
//		}
//	}
//
//	// Send return
//	ACPPacket * ret = new ACPPacket(CDH_TEMP_READ_SUCCESS, true, 64*sizeof(float), buffer);
//	return ret;
//}

ACPPacket * CDHHotSwaps(void){
	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
	logger->Log(LOGGER_LEVEL_INFO, "CDHStdTasks: CDHHotSwaps(): Reading hot swaps");

	// Setup
	AllStar::Servers::CDHServer * cdhServer = dynamic_cast<AllStar::Servers::CDHServer *>(Factory::GetInstance(CDH_SERVER_SINGLETON));
	float vcRead[32];
	uint8 * buffer = (uint8 *) malloc(32*sizeof(float));

	// Read and add to buffer
	cdhServer->devMngr->getHSStatus(vcRead);
	for(uint8 i = 0; i < 32; i++){
		AddFloat(buffer + i, vcRead[i]);
	}

	ACPPacket * ret = new ACPPacket(CDH_HOT_SWAPS_SUCCESS, true, 64*sizeof(float), buffer);
	return ret;
}

//TODO: add back power monitors
ACPPacket * CDHPowerMonitors(void){
	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
	logger->Log(LOGGER_LEVEL_FATAL, "CDHStdTasks: CDHPowerMonitors(): Unfinished function (variable type)!");

	//			AllStar::Servers::CDHServer * cdhServer = dynamic_cast<AllStar::Servers::CDHServer *>(Factory::GetInstance(CDH_SERVER_SINGLETON));
	//			PowerMonitor_Data data[4];
	//			list<VariableTypeData *>params;
	//			VariableTypeData maxPHold[4];
	//			VariableTypeData minPHold[4];
	//			VariableTypeData maxVHold[4];
	//			VariableTypeData minVHold[4];
	//			VariableTypeData maxAHold[4];
	//			VariableTypeData minAHold[4];
	//			VariableTypeData maxIHold[4];
	//			VariableTypeData minIHold[4];
	//
	//			// Read and add to list
	//			cdhServer->devMan->getPMStatus(data);
	//			for(uint8 i = 0; i < 4; i++){
	//				maxPHold[i] = VariableTypeData(data[i].MaxPower);
	//				minPHold[i] = VariableTypeData(data[i].MinPower);
	//				maxVHold[i] = VariableTypeData(data[i].MaxVoltage);
	//				minVHold[i] = VariableTypeData(data[i].MinVoltage);
	//				maxAHold[i] = VariableTypeData(data[i].MaxADIN);
	//				minAHold[i] = VariableTypeData(data[i].MinADIN);
	//				maxIHold[i] = VariableTypeData(data[i].MaxCurrent);
	//				minIHold[i] = VariableTypeData(data[i].MinCurrent);
	//				params.push_back(&maxPHold[i]);
	//				params.push_back(&minPHold[i]);
	//				params.push_back(&maxVHold[i]);
	//				params.push_back(&minVHold[i]);
	//				params.push_back(&maxAHold[i]);
	//				params.push_back(&minAHold[i]);
	//				params.push_back(&maxIHold[i]);
	//				params.push_back(&minIHold[i]);
	//			}
	//
	//			DataMessage dat(CDH_POWER_MONITORS_SUCCESS, params);
	//			ReturnMessage * retMsg = new ReturnMessage(&dat, true);
	//			return retMsg;
}

ACPPacket * CDHStartPM(void){
	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
	logger->Log(LOGGER_LEVEL_FATAL, "CDHStdTasks: CDHStartPM(): Unfinished function (variable type)!");

	AllStar::Servers::CDHServer * cdhServer = dynamic_cast<AllStar::Servers::CDHServer *>(Factory::GetInstance(CDH_SERVER_SINGLETON));

	// Start all of the sensors
	cdhServer->devMngr->startPMMeas();

	logger->Log(LOGGER_LEVEL_INFO, "CDHStdTasks: CDHStartPM(): Started PM measurement");
	ACPPacket * ret = new ACPPacket(CDH_START_PM_SUCCESS, true);
	return ret;
}

ACPPacket * CleanFiles(uint16 weekStart, uint16 weekEnd){
	CDHServer * cdhServer = dynamic_cast<CDHServer *> (Factory::GetInstance(CDH_SERVER_SINGLETON));
	cdhServer->storMngr->CleanFiles(weekStart, weekEnd);

	ACPPacket * ret = new ACPPacket(CDH_CLEAN_FS_SUCCESS, true);
	return ret;
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
		logger->Log(LOGGER_LEVEL_WARN, "CDHStdTasks: StartTempSensor(): Error Starting Sensor!");
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
	char logbuf[150];

	FILE * fp;
	fp = fopen(read.c_str(), "r");

	//cout<<"Attempting to read sensor "<<sensor<<" on bus "<<bus<<"!"<<endl;

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
			sprintf(logbuf,"CDHStdTasks: ReadTempSensor: Good data from sensor %d on bus %d",sensor,bus);
			logger->Log(LOGGER_LEVEL_DEBUG, logbuf);
			delete c;
			delete tempRead;
			fclose(fp);
			return temperature;
		}else{
			sprintf(logbuf,"CDHStdTasks: ReadTempSensor: Bad data from sensor %d on bus %d!",sensor,bus);
			logger->Log(LOGGER_LEVEL_WARN, logbuf);
			delete c;
			delete tempRead;
			fclose(fp);
			return -300;
		}
	}else{
		sprintf(logbuf,"CDHStdTasks: ReadTempSensor: Error opening file: sensor %d on bus %d",sensor,bus);
		logger->Log(LOGGER_LEVEL_WARN, logbuf);
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

	printf("SYSTEM STRING: %s\n", cmd.c_str());
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
