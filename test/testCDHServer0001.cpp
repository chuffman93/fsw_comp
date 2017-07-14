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
#include "util/Logger.h"

using namespace std;
using namespace rel_ops;
//using namespace AllStar::HAL;
using namespace AllStar::Core;
using namespace AllStar::Servers;

//void * taskRunCDH(void * params) {
//
//	CDHServer * cdhServer = static_cast<CDHServer *>(Factory::GetInstance(CDH_SERVER_SINGLETON));
//	ModeManager * modeManager = static_cast<ModeManager *>(Factory::GetInstance(MODE_MANAGER_SINGLETON));
//	Logger * logger = static_cast<Logger *>(Factory::GetInstance(LOGGER_SINGLETON));
//
//	logger->Info("taskRunCDH");
//
//	modeManager->Attach(*cdhServer);
//
//	for (int i = 0; i < 0; i++) {
//		//wdm->Kick();
//		usleep(1000000);
//	}
//
//	logger->Info("Kicking off the CDH server");
//
//	bool handlers = cdhServer->RegisterHandlers();
//	if(!handlers)
//	{
//		logger->Fatal("Error starting CDH Handlers!");
//	}
//	static_cast<SubsystemServer*>(cdhServer)->SubsystemLoop();
//	pthread_exit(NULL);
//}
//
//bool startSensor(int bus, int sensor)
//{
//	// create filename
//	char * temp = new char[1];
//	string start = "echo 1 > /sys/bus/w1/devices/w1_bus_master";
//	itoa(bus, temp, 10);
//	start.append(temp);
//	start.append("/TEMP");
//	itoa(sensor, temp, 10);
//	start.append(temp);
//	start.append("/start");
//	delete temp;
//
//	// start sensor
//	if(system(start.c_str()) == -1){
//		cout<<"Error starting sensor"<<endl;
//		return false;
//	}
//
//	return true;
//}
//
//void readSensor(int bus, int sensor){
//
//	// create filename
//	char * temp = new char[1];
//	string read = "/sys/bus/w1/devices/w1_bus_master";
//	itoa(bus, temp, 10);
//	read.append(temp);
//	read.append("/TEMP");
//	itoa(sensor, temp, 10);
//	read.append(temp);
//	read.append("/temp");
//	delete temp;
//
//	FILE * fp;
//	fp = fopen(read.c_str(), "r");
//
//	cout<<"Attempting to read sensor "<<sensor<<" on bus "<<bus<<"!"<<endl;
//
//	bool isGood = false;
//	if(fp)
//	{
//		char * c = new char[1];
//		char * tempRead = new char[12];
//		int tempHold;
//		int count = 0;
//		float temperature;
//
//		// Get temperature part of string
//		while((*c = fgetc(fp)) != '\n')
//		{
//			tempRead[count] = *c;
//			count++;
//		}
//
//		// Get float value
//		sscanf(tempRead, "t=%d", &tempHold);
//		temperature = (float) tempHold / 1000.0;
//		cout<<"Current Temperature: "<<temperature<<endl;
//
//		// Check validity
//		for(int i = 0; i < 28; i++){
//			*c = fgetc(fp);
//			if(i==27){
//				if(*c=='V'){
//					isGood = true;
//				}
//			}
//		}
//
//		// Act on validity
//		if(isGood){
//			cout<<"GOOD DATA"<<endl;
//		}else{
//			cout<<"BAD DATA"<<endl;
//		}
//
//		// Cleanup
//		delete c;
//		delete tempRead;
//	}else
//	{
//		cout<<"Error opening file!"<<endl;
//	}
//}
//
//TEST(TestCDHServer, testPowerToggle){
//	// Grab instances / set mode ------------------------------------------------------------------------------------------------------
//	FileHandler * fileHandler = dynamic_cast<FileHandler *>(Factory::GetInstance(MODE_MANAGER_SINGLETON));
//	ModeManager * modeManager = static_cast<ModeManager *>(Factory::GetInstance(MODE_MANAGER_SINGLETON));
//	modeManager->SetMode(MODE_ACCESS, LOCATION_ID_INVALID);
//
//
//	// Create CDHServer ---------------------------------------------------------------------------------------------------------------
//	pthread_t CDHThread;
//	bool threadCreated = pthread_create(&CDHThread, NULL, &taskRunCDH, NULL);
//	if (!threadCreated) {
//		printf("CDH Server Thread Creation Success\n");
//	} else {
//		printf("CDH Server Thread Creation Failed\n");
//	}
//
//
//	// Grab CDHServer, check it -------------------------------------------------------------------------------------------------------
//	CDHServer * cdhServer = static_cast<CDHServer *>(Factory::GetInstance(CDH_SERVER_SINGLETON));
//
//	usleep(500000);
//	cdhServer->subPowerOn(HARDWARE_LOCATION_COM);
//	usleep(500000);
//	cdhServer->subPowerOn(HARDWARE_LOCATION_ACS);
//	usleep(500000);
//	cdhServer->subPowerOn(HARDWARE_LOCATION_PLD);
//	usleep(500000);
//	cdhServer->subPowerOn(HARDWARE_LOCATION_GPS);
//	usleep(500000);
//	cdhServer->subPowerOn(HARDWARE_LOCATION_EPS);
//	usleep(500000);
//
//	cdhServer->subPowerOff(HARDWARE_LOCATION_COM);
//	cdhServer->subPowerOff(HARDWARE_LOCATION_ACS);
//	cdhServer->subPowerOff(HARDWARE_LOCATION_PLD);
//	cdhServer->subPowerOff(HARDWARE_LOCATION_GPS);
//	cdhServer->subPowerOff(HARDWARE_LOCATION_EPS);
//
//	ASSERT_TRUE(true);
//}
//
//// Main Test for Message Handlers
//TEST(TestCDHServer, testHandlers) {
//
//
////	// Grab instances / set mode ------------------------------------------------------------------------------------------------------
////	FileHandler * fileHandler = dynamic_cast<FileHandler *>(Factory::GetInstance(MODE_MANAGER_SINGLETON));
////	ModeManager * modeManager = static_cast<ModeManager *>(Factory::GetInstance(MODE_MANAGER_SINGLETON));
////	modeManager->SetMode(MODE_ACCESS, LOCATION_ID_INVALID);
////
////
////	// Create CDHServer ---------------------------------------------------------------------------------------------------------------
////	pthread_t CDHThread;
////	bool threadCreated = pthread_create(&CDHThread, NULL, &taskRunCDH, NULL);
////	if (!threadCreated) {
////		printf("CDH Server Thread Creation Success\n");
////	} else {
////		printf("CDH Server Thread Creation Failed\n");
////	}
////
////
////	// Grab CDHServer, check it -------------------------------------------------------------------------------------------------------
////	CDHServer * cdhServer = static_cast<CDHServer *>(Factory::GetInstance(CDH_SERVER_SINGLETON));
////	EXPECT_TRUE(!threadCreated);
////	usleep(5000000); //ensure full boot up
////
////
////	// Check all handlers -------------------------------------------------------------------------------------------------------------
////	bool success = true;
////	for(int cmd = CDH_CPU_USAGE_CMD; cmd <= CDH_CPU_USAGE_CMD; cmd++) // (cmd < CDH_CMD_MAX) when all handlers finished
////	{
////		// Dispatch message, check return
////		ReturnMessage * ret = DispatchPacket(SERVER_LOCATION_ACS,SERVER_LOCATION_CDH, 1, 0, MESSAGE_TYPE_COMMAND, cmd);
////		success &= ret->GetSuccess();
////
////		// Process Message
////		MessageProcess(SERVER_LOCATION_CDH, ret);
////	}
////
////	// Perform check ------------------------------------------------------------------------------------------------------------------
////	ASSERT_TRUE(success);
//
//}
//
//TEST(TestCDHServer, runServer) {
//
//	// Grab instances / set mode ------------------------------------------------------------------------------------------------------
//	FileHandler * fileHandler = dynamic_cast<FileHandler *>(Factory::GetInstance(MODE_MANAGER_SINGLETON));
//	ModeManager * modeManager = static_cast<ModeManager *>(Factory::GetInstance(MODE_MANAGER_SINGLETON));
//	modeManager->SetMode(MODE_ACCESS, LOCATION_ID_INVALID);
//
//
//	// Create CDHServer ---------------------------------------------------------------------------------------------------------------
//	pthread_t CDHThread;
//	bool threadCreated = pthread_create(&CDHThread, NULL, &taskRunCDH, NULL);
//	if (!threadCreated) {
//		printf("CDH Server Thread Creation Success\n");
//	} else {
//		printf("CDH Server Thread Creation Failed\n");
//	}
//
//
//	// Grab CDHServer, check it -------------------------------------------------------------------------------------------------------
//
//	// NOTE: for some reason, boolean return messages seem to be unaffected by the errors
//
//	CDHServer * cdhServer = static_cast<CDHServer *>(Factory::GetInstance(CDH_SERVER_SINGLETON));
//	EXPECT_TRUE(!threadCreated);
//	usleep(30000000); // give server a chance to run
//
//	ASSERT_TRUE(true);
//
//}
//
////-------------- Old Tests ------------------
//// Old testing, pending deletion
//TEST(DISABLED_TestCDHServer, testTemp){
//
//	// Start all of the sensors
//	bool validStart[4][16];
//	for(uint8 bus = 1; bus < 5; bus++){
//		for(uint8 sensor = 0; sensor < 16; sensor++){
//			validStart[bus][sensor] = startSensor(bus,sensor);
//		}
//	}
//
//	// wait for read
//	usleep(750000);
//
//	// read all of the sensor data
//	for(uint8 bus = 1; bus < 5; bus++){
//		for(uint8 sensor = 0; sensor < 16; sensor++){
//			readSensor(bus,sensor);
//		}
//	}
//}
//
//// Old testing, pending deletion
//TEST(DISABLED_TestCDHServer, testSysInfo){
//
//	struct sysinfo si;
//	sysinfo (&si);
//
//	cout<<"1 min: "<<si.loads[0]<<endl;
//	cout<<"5 min: "<<si.loads[1]<<endl;
//	cout<<"15 min: "<<si.loads[2]<<endl;
//	cout<<"Free RAM: "<<si.freeram<<endl;
//	cout<<"Total RAM: "<<si.totalram<<endl;
//	cout<<"Percent used RAM: "<<(100.0*(259964928.0 - ((float) si.freeram)) / (259964928.0))<<endl;
//}
//
//// Old testing, pending deletion
//TEST(DISABLED_TestCDHServer, testStatvfs){
//
//	struct statvfs svfs;
//	statvfs((char *) "/", &svfs);
//
//	cout<<"Free blocks: "<<svfs.f_bfree<<endl;
//	cout<<"Total blocks: "<<svfs.f_blocks<<endl;
//
//}
//
//// Unused testing, pending deletion
//TEST(DISABLED_TestCDHServer, testProc) {
//
//	char * filename = (char *) "/proc/meminfo";
//	FILE * fp;
//
//	fp = fopen(filename, "r");
//
//	EXPECT_TRUE(fp);
//
//	uint8 * buffer = NULL;
//	uint8 bufferSize = 128;
//	size_t result;
//	buffer = new uint8[bufferSize];
//	result = fread(buffer, 1, bufferSize, fp);
//
//	for (int i = 0; i < 128; i++) {
//		cout << buffer[i];
//	}
//
//	delete buffer;
//}
