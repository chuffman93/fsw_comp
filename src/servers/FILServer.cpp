/*
* FILServer.cpp
*
*  Created on: Nov 28, 2016
*      Author: Jack Dinkel
*  Modified by:
*/

#include "servers/FILServer.h"
#include "core/Singleton.h"
#include "core/Factory.h"
#include "core/StdTypes.h"
#include "core/WatchdogManager.h"
#include "util/FileHandler.h"
#include "util/Logger.h"
#include <iostream>
#include <fstream>
#include <queue>

using namespace std;
using namespace AllStar::Core;

namespace AllStar{
namespace Servers{

FILServer::FILServer(string nameIn, LocationIDType idIn) :
	SubsystemServer(nameIn, idIn), Singleton(), arby(idIn) {
}

FILServer::~FILServer(){
}

void FILServer::Initialize(void){

}

#ifdef TEST
void FILServer::Destroy(void){

}
#endif

bool FILServer::IsFullyInitialized(void){
	return(Singleton::IsFullyInitialized());
}

FILServer & FILServer::operator=(const FILServer & source){
	if (this == &source){
		return *this;
	}

	SubsystemServer::operator =(source);

	return *this;
}

bool FILServer::RegisterHandlers(){
	bool success = true;
	return success;
}

// ----- File Logging functions ------------------------------------------------------------------------------------
void FILServer::OpenNewFile(){
	// Opens a new file with the appropriate naming scheme. File Descriptor is saved in GeneralInfo struct

	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));

	// Get new file name
	int time = getTimeInSec();
	int boot_count = 0;// TODO
	char fileName[100];
	sprintf(fileName, "%s/GEN_%d_%d", GENERAL_FILE_LOCATION, boot_count, time);
	GeneralInfo.file_name = fileName;

	// Create and open new file
	GeneralInfo.file = fopen(fileName, "a");
	if (GeneralInfo.file == NULL) { logger->Log(LOGGER_LEVEL_WARN, "Error opening file"); }
	GeneralInfo.file_open = true;
	GeneralInfo.bytes_written = 0;
}

bool FILServer::GeneralLog(string buf){
	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
	printf("General: %s\n", buf.c_str());
	int buf_size = buf.size();

	if (buf_size >= MAX_FILE_SIZE) { logger->Log(LOGGER_LEVEL_WARN, "Telemetry larger than file size"); }
	else{
		// Check if file is open
		if (!GeneralInfo.file_open) {
			logger->Log(LOGGER_LEVEL_DEBUG, "File not open");
			OpenNewFile();
		}

		if (GeneralInfo.bytes_written + buf_size + 1 < MAX_FILE_SIZE) {// Add one to account for the newline
			fwrite(buf.c_str(), sizeof(char), buf_size, GeneralInfo.file);
			fwrite("\n", sizeof(char), 1, GeneralInfo.file);
			fflush(GeneralInfo.file);
			GeneralInfo.bytes_written += buf_size + 1;
		}
		else { // File is full, open new file and write to it
			fclose (GeneralInfo.file);;
			GeneralInfo.file_open = false;
			OpenNewFile();
			fwrite(buf.c_str(), sizeof(char), buf_size, GeneralInfo.file);
			fwrite("\n", sizeof(char), 1, GeneralInfo.file);
			fflush(GeneralInfo.file);
			GeneralInfo.bytes_written += buf_size + 1;
		}
	}
	return true;
}

bool FILServer::HealthLog(string buf){
	printf("Health: %s\n", buf.c_str());
	ofstream file;
	file.open (HEALTH_FILE_LOCATION);
	file << buf;
	file.close();
	return true;
}

bool FILServer::ModeLog(string buf){
	printf("Mode: %s\n", buf.c_str());
	ofstream file;
	file.open (MODE_FILE_LOCATION);
	file << buf;
	file.close();
	return true;
}

bool FILServer::HotSwapLog(string buf){
	printf("HotSwap: %s\n", buf.c_str());
	ofstream file;
	file.open (HOTSWAP_FILE_LOCATION);
	file << buf;
	file.close();
	return true;
}

bool FILServer::ErrorLog(string buf){
	printf("Error: %s\n", buf.c_str());
	ofstream file;
	file.open (ERROR_FILE_LOCATION);
	file << buf;
	file.close();
	return true;
}

bool FILServer::DiagnosticLog(string buf){
	printf("Diagnostic: %s\n", buf.c_str());
	ofstream file;
	file.open (DIAGNOSTIC_FILE_LOCATION);
	file << buf;
	file.close();
	return true;
}

bool FILServer::RadLog(string buf){
	// Do we need this?
	printf("Rad: %s\n", buf.c_str());
	ofstream file;
	file.open (RAD_FILE_LOCATION);
	file << buf;
	file.close();
	return true;
}
// -----------------------------------------------------------------------------------------------------------------


// --------- State Machine -----------------------------------------------------------------------------------------

void FILServer::loopInit(void){
	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
	logger->Log(LOGGER_LEVEL_FATAL, "File server loop");
	usleep(10000);


	if (!FileQueue.empty()){
		string str;
		str = FileQueue.front().buffer;

		switch (FileQueue.front().dest){
		case DESTINATION_GENERAL:
			if ( !GeneralLog(str.c_str()) ) {
				logger->Log(LOGGER_LEVEL_WARN, "Error writing to General File");
			}
			break;
		case DESTINATION_HEALTH:
			if( !HealthLog(str.c_str()) ) {
				logger->Log(LOGGER_LEVEL_WARN, "Error writing to Health File");
			}
			break;
		case DESTINATION_MODE:
			if( !ModeLog(str.c_str()) ) {
				logger->Log(LOGGER_LEVEL_WARN, "Error writing to Mode File");
			}
			break;
		case DESTINATION_HOTSWAP:
			if ( !HotSwapLog(str.c_str()) ) {
				logger->Log(LOGGER_LEVEL_WARN, "Error writing to HotSwap File");
			}
			break;
		case DESTINATION_ERROR:
			if ( !ErrorLog(str.c_str()) ) {
				logger->Log(LOGGER_LEVEL_WARN, "Error writing to Error File");
			}
			break;
		case DESTINATION_DIAGNOSTIC:
			if ( !DiagnosticLog(str.c_str()) ) {
				logger->Log(LOGGER_LEVEL_WARN, "Error writing to Diagnostic File");
			}
			break;
		case DESTINATION_RAD:
			if ( !RadLog(str.c_str()) ) {
				logger->Log(LOGGER_LEVEL_WARN, "Error writing to Rad File");
			}
			break;
		default:
		    logger->Log(LOGGER_LEVEL_WARN, "FILServer: Unknown destination!");
		    break;
		}

		FileQueue.pop();
	}
	else {
		printf("Nothing to pop\n");
	}


}

// -----------------------------------------------------------------------------------------------------------------




} // End of namespace Servers
} // End of namespace AllStar
