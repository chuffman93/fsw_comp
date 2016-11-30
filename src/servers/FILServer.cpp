/*
* FILServer.cpp
*
*  Created on: Nov 28, 2016
*      Author: Jack Dinkel
*  Modified by:
*/

#include "servers/FILServer.h"
//#include "servers/FILStdTasks.h"
#include "servers/DispatchStdTasks.h"
#include "core/Singleton.h"
#include "core/Factory.h"
#include "core/StdTypes.h"
#include "core/WatchdogManager.h"
#include "util/FileHandler.h"
#include "util/Logger.h"
#include "POSIX.h"
#include <iostream>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
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

// File Logging functions
bool FILServer::HealthLog(){
	return true;
}

bool FILServer::ModeLog(){
	return true;
}

bool FILServer::HotSwapLog(){
	return true;
}

bool FILServer::ErrorLog(){
	return true;
}

bool FILServer::DiagnosticLog(){
	return true;
}

bool FILServer::GeneralLog(string buf){
	printf("buf: %s\n", buf.c_str());
	return true;
}

bool FILServer::RadLog(){
	// Do we need this?
	return true;
}


// --------- State Machine -----------------------------------------------------------------------------------------

void FILServer::loopInit(void){
	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
	logger->Log(LOGGER_LEVEL_FATAL, "File server loop");
	usleep(10000);

	/*queue<FilePacket> myQueue;

	FilePacket myPacket;
	myPacket.buffer = "hello world!";

	myQueue.push(myPacket);
	string str;
	str = myQueue.front().buffer;
	GeneralLog(str);
	myQueue.pop();*/

	/*FilePacket myPacket;
	myPacket.buffer = "hello world!";
	FileQueue.push(myPacket);*/

	if (!FileQueue.empty()){
		string str;
		str = FileQueue.front().buffer;
		GeneralLog(str);
		FileQueue.pop();
	}
	else {
		printf("Nothing to pop\n");
	}


}

// -----------------------------------------------------------------------------------------------------------------




} // End of namespace Servers
} // End of namespace AllStar
