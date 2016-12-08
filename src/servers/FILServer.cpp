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

// ----- File Manager functions ------------------------------------------------------------------------------------
FileManager::FileManager(string path){
	file_path = path;
}
void FileManager::CloseFile(){
	fclose(file);
	file_open = false;
}

void FileManager::OpenFile(){
	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));

	file = fopen(file_name.c_str(), "a");
	if (file == NULL) { logger->Log(LOGGER_LEVEL_WARN, "Error opening file"); }
	file_open = true;
	bytes_written = 0;
}

void FileManager::GetFileName(){
	int time = getTimeInSec();
	int boot_count = 0;// TODO
	char fileName[100];
	sprintf(fileName, "%s/GEN_%d_%d", file_path.c_str(), boot_count, time);
	file_name = fileName;
}

void FileManager::OpenNewFile(){
	GetFileName();
	OpenFile();
}

void FileManager::Write(string buf, int buf_size){
	fwrite(buf.c_str(), sizeof(char), buf_size, file);
	fwrite("\n", sizeof(char), 1, file);
	fflush(file);
	bytes_written += buf_size + 1;
}

bool FileManager::Log(string buf){
	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
	printf("Buf: %s\n", buf.c_str());
	int buf_size = buf.size();

	if (buf_size >= MAX_FILE_SIZE) { logger->Log(LOGGER_LEVEL_WARN, "Telemetry larger than file size"); }
	else{
		// Check if file is open
		if (!file_open) {
			logger->Log(LOGGER_LEVEL_DEBUG, "File not open");
			OpenNewFile();
		}

		if (bytes_written + buf_size + 1 < MAX_FILE_SIZE) {
			Write(buf, buf_size);
		}
		else { // File is full, open new file and write to it
			CloseFile();
			OpenNewFile();
			Write(buf, buf_size);
		}
	}
	return true;
}


// ----- FILServer functions ------------------------------------------------------------------------------------
FILServer::FILServer(string nameIn, LocationIDType idIn) :
					SubsystemServer(nameIn, idIn),
					Singleton(),
					arby(idIn),
					GeneralLogger(GENERAL_FILE_PATH),
					HealthLogger(HEALTH_FILE_PATH),
					ModeLogger(MODE_FILE_PATH),
					HotSwapLogger(HOTSWAP_FILE_PATH),
					ErrorLogger(ERROR_FILE_PATH),
					DiagnosticLogger(DIAGNOSTIC_FILE_PATH),
					RadLogger(RAD_FILE_PATH)
					{
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
// -----------------------------------------------------------------------------------------------------------------


// --------- State Machine -----------------------------------------------------------------------------------------

void FILServer::loopInit(void){
	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
	logger->Log(LOGGER_LEVEL_FATAL, "File server loop");
	usleep(100);


	if (!FileQueue.empty()){
		string str;
		str = FileQueue.front().buffer;

		switch (FileQueue.front().dest){
		case DESTINATION_GENERAL:
			if ( !GeneralLogger.Log(str.c_str()) ) {
				logger->Log(LOGGER_LEVEL_WARN, "Error writing to General File");
			}
			break;
		case DESTINATION_HEALTH:
			if( !HealthLogger.Log(str.c_str()) ) {
				logger->Log(LOGGER_LEVEL_WARN, "Error writing to Health File");
			}
			break;
		case DESTINATION_MODE:
			if( !ModeLogger.Log(str.c_str()) ) {
				logger->Log(LOGGER_LEVEL_WARN, "Error writing to Mode File");
			}
			break;
		case DESTINATION_HOTSWAP:
			if ( !HotSwapLogger.Log(str.c_str()) ) {
				logger->Log(LOGGER_LEVEL_WARN, "Error writing to HotSwap File");
			}
			break;
		case DESTINATION_ERROR:
			if ( !ErrorLogger.Log(str.c_str()) ) {
				logger->Log(LOGGER_LEVEL_WARN, "Error writing to Error File");
			}
			break;
		case DESTINATION_DIAGNOSTIC:
			if ( !DiagnosticLogger.Log(str.c_str()) ) {
				logger->Log(LOGGER_LEVEL_WARN, "Error writing to Diagnostic File");
			}
			break;
		case DESTINATION_RAD:
			if ( !RadLogger.Log(str.c_str()) ) {
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
