/*
 * CDHStdTasks.cpp
 *
 *  Created on: Mar 8, 2016
 *      Author: Alex St. Clair
 */

#include "servers/CDHServer.h"
#include "servers/CDHStdTasks.h"
#include "servers/DispatchStdTasks.h"
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
Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));

//------------------------------------------- Message Handlers -------------------------------------------

ACPPacket * CDHCPUUsage(void){
	CDHServer * cdhServer = dynamic_cast<CDHServer *> (Factory::GetInstance(CDH_SERVER_SINGLETON));
	if(sysinfo(&cdhServer->si) != 0){
		logger->Log("CDHStdTasks: CDHCPUUsage(): Error", LOGGER_LEVEL_ERROR);
		ACPPacket * ret = new ACPPacket(CDH_CPU_USAGE_FAILURE);
		return ret;
	}

	logger->Log("CDHCPUUsage(): Checking loads", LOGGER_LEVEL_INFO);
	logger->Log("    CPU 1  min: %u", cdhServer->si.loads[0], LOGGER_LEVEL_DEBUG);
	logger->Log("    CPU 5  min: %u", cdhServer->si.loads[1], LOGGER_LEVEL_DEBUG);
	logger->Log("    CPU 15 min: %u", cdhServer->si.loads[2], LOGGER_LEVEL_DEBUG);

	uint8 * buffer = (uint8 *) malloc(12);
	AddUInt32(buffer, cdhServer->si.loads[0]);
	AddUInt32(buffer + 4, cdhServer->si.loads[1]);
	AddUInt32(buffer + 8, cdhServer->si.loads[2]);

	ACPPacket * ret = new ACPPacket(CDH_CPU_USAGE_SUCCESS, 12, buffer);
	return ret;
}

ACPPacket * CDHMemUsage(void){
	CDHServer * cdhServer = dynamic_cast<CDHServer *> (Factory::GetInstance(CDH_SERVER_SINGLETON));
	if(sysinfo(&cdhServer->si) != 0){
		logger->Log("CDHStdTasks: CDHMemUsage(): Error", LOGGER_LEVEL_ERROR);
		ACPPacket * ret = new ACPPacket(CDH_MEM_USAGE_FAILURE);
		return ret;
	}

	logger->Log("CDHStdTasks: CDHMemUsage(): Checking Memory", LOGGER_LEVEL_INFO);
	float mem = 100.0*(259964928.0 - ((float) cdhServer->si.freeram)) / (259964928.0); //hard-coded total ram: 100*(total - free)/total = percent use
	logger->Log("    MEM: %f", mem, LOGGER_LEVEL_DEBUG);


	uint8 * buffer = (uint8 *) malloc(4);
	AddFloat(buffer, mem);

	ACPPacket * ret = new ACPPacket(CDH_MEM_USAGE_SUCCESS, 4, buffer);
	return ret;
}

ACPPacket * CDHTempStart(void){
	// Start all of the sensors
	bool validStart[4][16];
	bool success = true;
	for(uint8 bus = 0; bus < 4; bus++){
		for(uint8 sensor = 0; sensor < 16; sensor++){
			validStart[bus][sensor] = StartTempSensor(bus+1,sensor);
			success &= validStart[bus][sensor];
		}
	}

	if(success){
		logger->Log("CDHStdTasks: CDHTempStart(): Started sensors", LOGGER_LEVEL_INFO);
		ACPPacket * ret = new ACPPacket(CDH_TEMP_START_SUCCESS);
		return ret;
	}else{
		logger->Log("CDHStdTasks: CDHTempStart(): Error starting sensors!", LOGGER_LEVEL_ERROR);
		ACPPacket * ret = new ACPPacket(CDH_TEMP_START_FAILURE);
		return ret;
	}
}

ACPPacket * CDHTempRead(void){
	logger->Log("CDHStdTasks: CDHTempRead(): Reading temp sensors", LOGGER_LEVEL_INFO);

	// Setup
	float temperatures[4][16];
	uint8 * buffer = (uint8 *) malloc(64*sizeof(float));

	// Read and add to list
	for(uint8 bus = 0; bus < 4; bus++){
		for(uint8 sensor = 0; sensor < 16; sensor++){
			temperatures[bus][sensor] = ReadTempSensor(bus+1,sensor);
			AddFloat(buffer + 4*(16*bus + sensor), temperatures[bus][sensor]);
		}
	}

	// Send return
	ACPPacket * ret = new ACPPacket(CDH_TEMP_READ_SUCCESS, 64*sizeof(float), buffer);
	return ret;
}

ACPPacket * CDHHotSwaps(void){
	logger->Log("CDHStdTasks: CDHHotSwaps(): Reading hot swaps", LOGGER_LEVEL_INFO);

	// Setup
	AllStar::Servers::CDHServer * cdhServer = dynamic_cast<AllStar::Servers::CDHServer *>(Factory::GetInstance(CDH_SERVER_SINGLETON));
	float vcRead[32];
	uint8 * buffer = (uint8 *) malloc(32*sizeof(float));

	// Read and add to buffer
	cdhServer->devMngr->getHSStatus(vcRead);
	for(uint8 i = 0; i < 32; i++){
		AddFloat(buffer + i, vcRead[i]);
	}

	ACPPacket * ret = new ACPPacket(CDH_HOT_SWAPS_SUCCESS, 64*sizeof(float), buffer);
	return ret;
}

//TODO: add back power monitors
ACPPacket * CDHPowerMonitors(void){
	logger->Log("CDHStdTasks: CDHPowerMonitors(): Unfinished function (variable type)!", LOGGER_LEVEL_FATAL);

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
	logger->Log("CDHStdTasks: CDHStartPM(): Unfinished function (variable type)!", LOGGER_LEVEL_FATAL);

	AllStar::Servers::CDHServer * cdhServer = dynamic_cast<AllStar::Servers::CDHServer *>(Factory::GetInstance(CDH_SERVER_SINGLETON));

	// Start all of the sensors
	cdhServer->devMngr->startPMMeas();

	logger->Log("CDHStdTasks: CDHStartPM(): Started PM measurement", LOGGER_LEVEL_INFO);
	ACPPacket * ret = new ACPPacket(CDH_START_PM_SUCCESS);
	return ret;
}

ACPPacket * CleanFiles(uint16 weekStart, uint16 weekEnd){
	CDHServer * cdhServer = dynamic_cast<CDHServer *> (Factory::GetInstance(CDH_SERVER_SINGLETON));
	cdhServer->storMngr->CleanFiles(weekStart, weekEnd);

	ACPPacket * ret = new ACPPacket(CDH_CLEAN_FS_SUCCESS);
	return ret;
}

// Helper Functions ---------------------------------------------------------------
bool StartTempSensor(int bus, int sensor){
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
		logger->Log("CDHStdTasks: StartTempSensor(): Error Starting Sensor!", LOGGER_LEVEL_WARN);
		return false;
	}
	return true;
}

float ReadTempSensor(int bus, int sensor){
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
			logger->Log(logbuf,LOGGER_LEVEL_DEBUG);
			delete c;
			delete tempRead;
			fclose(fp);
			return temperature;
		}else{
			sprintf(logbuf,"CDHStdTasks: ReadTempSensor: Bad data from sensor %d on bus %d!",sensor,bus);
			logger->Log(logbuf,LOGGER_LEVEL_WARN);
			delete c;
			delete tempRead;
			fclose(fp);
			return -300;
		}
	}else{
		sprintf(logbuf,"CDHStdTasks: ReadTempSensor: Error opening file: sensor %d on bus %d",sensor,bus);
		logger->Log(logbuf,LOGGER_LEVEL_WARN);
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
		system("echo \"out\" > \"/sys/class/gpio/pioB27/direction\"");;
	}

	// AUXCOM
	if(!(stat("/sys/class/gpio/pioB11/", &sb) == 0 && S_ISDIR(sb.st_mode))){
		system("echo 43 > \"/sys/class/gpio/export\"");
		system("echo \"out\" > \"/sys/class/gpio/pioB11/direction\"");;
	}

	//-----------------------RESET LINES--------------------------------------
	//EPS
	if(!(stat("/sys/class/gpio/pioE10/", &sb) == 0 && S_ISDIR(sb.st_mode))){
		system("echo 138 > \"/sys/class/gpio/export\"");
		system("echo \"out\" > \"/sys/class/gpio/pioE10/direction\"");;
	}

	//COM
	if(!(stat("/sys/class/gpio/pioA11/", &sb) == 0 && S_ISDIR(sb.st_mode))){
		system("echo 11 > \"/sys/class/gpio/export\"");
		system("echo \"out\" > \"/sys/class/gpio/pioA11/direction\"");;
	}

	//ACS
	if(!(stat("/sys/class/gpio/pioA12/", &sb) == 0 && S_ISDIR(sb.st_mode))){
		system("echo 12 > \"/sys/class/gpio/export\"");
		system("echo \"out\" > \"/sys/class/gpio/pioA12/direction\"");;
	}

	//PLD
	if(!(stat("/sys/class/gpio/pioE11/", &sb) == 0 && S_ISDIR(sb.st_mode))){
		system("echo 139 > \"/sys/class/gpio/export\"");
		system("echo \"out\" > \"/sys/class/gpio/pioE11/direction\"");;
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
		logger->Log("CDHStdTasks: Toggling power on COM", LOGGER_LEVEL_INFO);
		cmd.append("15/value\"");
		break;
	case HARDWARE_LOCATION_ACS:
		logger->Log("CDHStdTasks: Toggling power on ACS", LOGGER_LEVEL_INFO);
		cmd.append("25/value\"");
		break;
	case HARDWARE_LOCATION_PLD:
		logger->Log("CDHStdTasks: Toggling power on PLD", LOGGER_LEVEL_INFO);
		cmd.append("17/value\"");
		break;
	case HARDWARE_LOCATION_GPS:
		logger->Log("CDHStdTasks: Toggling power on GPS", LOGGER_LEVEL_INFO);
		cmd.append("27/value\"");
		break;
	default:
		logger->Log("CDHStdTasks: Invalid subsystem for power toggle", LOGGER_LEVEL_WARN);
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
		logger->Log("CDHStdTasks: Resetting EPS", LOGGER_LEVEL_INFO);
		cmd.append("E10/value\"");
		break;
	case HARDWARE_LOCATION_COM:
		logger->Log("CDHStdTasks: Resetting COM", LOGGER_LEVEL_INFO);
		cmd.append("A11/value\"");
		break;
	case HARDWARE_LOCATION_ACS:
		logger->Log("CDHStdTasks: Resetting ACS", LOGGER_LEVEL_INFO);
		cmd.append("A12/value\"");
		break;
	case HARDWARE_LOCATION_PLD:
		logger->Log("CDHStdTasks: Resetting PLD", LOGGER_LEVEL_INFO);
		cmd.append("E11/value\"");
		break;
	default:
		logger->Log("CDHStdTasks: Invalid subsystem for reset", LOGGER_LEVEL_WARN);
		return;
	}

	system(cmd.c_str());
}

}
}
