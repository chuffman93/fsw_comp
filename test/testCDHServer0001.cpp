/*
 * testCDHServer0001.cpp
 *
 *  Created on: Mar 4, 2016
 *      Author: Alex St. Clair
 */

#include <iostream>
#include "gtest/gtest.h"
#include "util/FileHandler.h"
#include "core/Factory.h"
#include "servers/DispatchStdTasks.h"
#include "core/StdTypes.h"
#include "core/ModeManager.h"
#include "stdlib.h"
#include <stdio.h>
#include <pthread.h>
#include "util/itoa.h"
#include <sys/sysinfo.h>
#include <sys/statvfs.h>
#include <cstring>
#include <string.h>

#include "servers/CDHServer.h"
#include "servers/CDHStdTasks.h"

using namespace std;
using namespace rel_ops;
//using namespace Phoenix::HAL;
using namespace Phoenix::Core;
using namespace Phoenix::Servers;

// Necessary to test CDH
void* taskRunCDH(void * params) {
	CDHServer * cdhServer = dynamic_cast<CDHServer *>(Factory::GetInstance(
			CDH_SERVER_SINGLETON));
	ModeManager * modeManager =
			dynamic_cast<ModeManager *>(Factory::GetInstance(
					MODE_MANAGER_SINGLETON));
	//WatchdogManager * wdm = dynamic_cast<WatchdogManager *> (Factory::GetInstance(WATCHDOG_MANAGER_SINGLETON));

	modeManager->Attach(*cdhServer);

	for (int i = 0; i < 0; i++) {
		//wdm->Kick();
		usleep(1000000);
	}

	printf("\r\nKicking off the CDH server\r\n");

	bool success = cdhServer->RegisterHandlers();
	cdhServer->SubsystemLoop();
	pthread_exit(NULL);
}

bool startSensor(int bus, int sensor)
{
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
		cout<<"Error starting sensor"<<endl;
		return false;
	}

	return true;
}

void readSensor(int bus, int sensor){

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

	cout<<"Attempting to read sensor!"<<endl;

	bool isBad = false;
	if(fp){
		char * c = new char[1];
		char temp[5];
		int count = 0;

		// Get temperature
		while((*c = fgetc(fp)) != '\n'){
			if((*c != 't') & (*c != '=')){
				//cout<<c;
				temp[count] = *c;
				count++;
			}
		}

		// Display Temperature
		cout<<"Temperature: ";
		for(int i=0; i<5; i++){
			cout<<temp[i];
		}
		cout<<endl;

		// Get validity
		// TODO: better way?
		while((*c = fgetc(fp)) != 'L'){ //since L isn't hex and is in (IN)VALID
			//cout<<c;
			if(*c=='I'){
				isBad = true;
			}
		}

		// Act on validity
		if(isBad){
			cout<<"BAD DATA"<<endl;
		}else{
			cout<<"GOOD DATA"<<endl;
		}
	}else{
		cout<<"Error opening file!"<<endl;
	}

	/*
	if(fp)
	{
		uint8 * buffer = NULL;
		uint8 bufferSize = 128;
		size_t result;
		buffer = new uint8[bufferSize];
		result = fread(buffer, 1, bufferSize, fp);

		for (int i = 0; i < 128; i++) {
			cout << buffer[i];
		}

		delete buffer;
	}
	else
	{
		cout<<"CDHStdTasks CHDTempBus(): Error opening file"<<endl;
	}
	*/
}

// Main Test for Message Handlers
TEST(TestCDHServer, testHandlers) {


	// Grab instances / set mode ------------------------------------------------------------------------------------------------------
	FileHandler * fileHandler = dynamic_cast<FileHandler *>(Factory::GetInstance(MODE_MANAGER_SINGLETON));
	ModeManager * modeManager = dynamic_cast<ModeManager *>(Factory::GetInstance(MODE_MANAGER_SINGLETON));
	modeManager->SetMode(MODE_ACCESS, LOCATION_ID_INVALID);


	// Create CDHServer ---------------------------------------------------------------------------------------------------------------
	pthread_t CDHThread;
	bool threadCreated = pthread_create(&CDHThread, NULL, &taskRunCDH, NULL);
	if (!threadCreated) {
		printf("CDH Server Thread Creation Success\n");
	} else {
		printf("CDH Server Thread Creation Failed\n");
	}


	// Grab CDHServer, check it -------------------------------------------------------------------------------------------------------
	CDHServer * cdhServer = dynamic_cast<CDHServer *>(Factory::GetInstance(CDH_SERVER_SINGLETON));
	EXPECT_TRUE(!threadCreated);
	usleep(5000000); //ensure full boot up


	// Check all handlers -------------------------------------------------------------------------------------------------------------
	bool success = true;
	for(int cmd = CDH_CMD_MIN; cmd <= CDH_MEM_USAGE_CMD; cmd++) // (cmd < CDH_CMD_MAX) when all finished
	{
		// Dispatch message, check return
		ReturnMessage * ret = DispatchPacket(SERVER_LOCATION_ACS,SERVER_LOCATION_CDH, 1, 0, MESSAGE_TYPE_COMMAND, cmd);
		success &= ret->GetSuccess();

		// Process Message
		MessageProcess(SERVER_LOCATION_CDH, ret);
	}

	// Perform check ------------------------------------------------------------------------------------------------------------------
	ASSERT_TRUE(success);

}

TEST(TestCDHServer, testTemp){
	// Eventually make string arrays for iteration
	//char cdh1s[56];
	//strcpy(cdh1s, "echo 1 >> /sys/bus/w1/devices/w1_bus_master1/TEMP");
	//strcat(cdh1s, "0/start");

	// start all of the sensors
	/*
	bool valid[4][16];
	for(uint8 bus = 1; bus < 5; bus++){
		for(uint8 sensor = 0; sensor < 16; sensor++){
			valid[bus][sensor] = startSensor(bus,sensor);
		}
	}
	*/

	ASSERT_TRUE(startSensor(1,1));

	// wait for read
	usleep(750000);

	// read all of the sensor data
	readSensor(1,1);

	/*
	string cdh1s = "echo 1 > /sys/bus/w1/devices/w1_bus_master1/TEMP";
	cdh1s.append("0/start");

	char * cdh1t = (char *) "/sys/bus/w1/devices/w1_bus_master1/TEMP0/temp";
	//string cdh2 = "/sys/bus/w1/devices/w1_bus_master1/TEMP1/";
	*/

	/*
	if(system(cdh1s.c_str()) == -1){
		//error
		cout<<"CDHStdTasks CHDTempBus(): Error starting sensor"<<endl;
	}
	usleep(600000);

	FILE * fp;

	fp = fopen(cdh1t, "r");

	cout<<"CDHStdTasks CHDTempBus(): attempting to read sensor!"<<endl;

	if(fp)
	{
		uint8 * buffer = NULL;
		uint8 bufferSize = 128;
		size_t result;
		buffer = new uint8[bufferSize];
		result = fread(buffer, 1, bufferSize, fp);

		for (int i = 0; i < 128; i++) {
			cout << buffer[i];
		}

		delete buffer;
	}
	else
	{
		cout<<"CDHStdTasks CHDTempBus(): Error opening file"<<endl;
	}
	*/
}

//-------------- Old Tests ------------------
// Old testing, pending deletion
TEST(DISABLED_TestCDHServer, testSysInfo){

	struct sysinfo si;
	sysinfo (&si);

	cout<<"1 min: "<<si.loads[0]<<endl;
	cout<<"5 min: "<<si.loads[1]<<endl;
	cout<<"15 min: "<<si.loads[2]<<endl;
	cout<<"Free RAM: "<<si.freeram<<endl;
	cout<<"Total RAM: "<<si.totalram<<endl;
	cout<<"Percent used RAM: "<<(100.0*(259964928.0 - ((float) si.freeram)) / (259964928.0))<<endl;
}

// Old testing, pending deletion
TEST(DISABLED_TestCDHServer, testStatvfs){

	struct statvfs svfs;
	statvfs((char *) "/", &svfs);

	cout<<"Free blocks: "<<svfs.f_bfree<<endl;
	cout<<"Total blocks: "<<svfs.f_blocks<<endl;

}

// Unused testing, pending deletion
TEST(DISABLED_TestCDHServer, testProc) {

	char * filename = (char *) "/proc/meminfo";
	FILE * fp;

	fp = fopen(filename, "r");

	EXPECT_TRUE(fp);

	uint8 * buffer = NULL;
	uint8 bufferSize = 128;
	size_t result;
	buffer = new uint8[bufferSize];
	result = fread(buffer, 1, bufferSize, fp);

	for (int i = 0; i < 128; i++) {
		cout << buffer[i];
	}

	delete buffer;
}
