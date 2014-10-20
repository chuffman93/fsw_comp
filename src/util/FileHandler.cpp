/*
 * FileHandler.cpp
 *
 *  Created on: Aug 7, 2012
 *      Author: Caitlyn Cooke
 */

#include <cstring>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>

#include "servers/GPSServer.h"

#include "util/FileHandler.h"
#include "util/itoa.h"

#include "HAL/RTC.h"

//#include "boards/backplane/dbg_led.h"

//File path
//to write on sd change to /media/card/
const char * writeDir="../../../filehandlertest/";

//size in bytes that is required to compress the files
const unsigned int sizeToZip = 10*(1024);

#ifdef WIN32
#include <stdio.h>
#endif

#ifndef WIN32
extern "C"
{ 
//#include "util/fsaccess.h"
}
#endif //WIN32

#define FILE_HANDLER_DEBUG			0

#if FILE_HANDLER_DEBUG && defined(WIN32)
#include <iostream>
#define DEBUG_COUT(m) cout << m << endl;
#else
#define DEBUG_COUT(m)
#endif // DEBUG

using namespace Phoenix::HAL;
using namespace Phoenix::Core;
using namespace std;

// kill me now
// Instantiate static members
const char * FileHandler::errLog = NULL;
const char * FileHandler::modeLog = NULL;

void FileHandler::Initialize(void)
{
	errLog = "Error_Log.dat";
	modeLog = "Mode_Log.dat";
}

#ifdef WIN32
void FileHandler::Destroy(void)
{

}
#endif

bool FileHandler::IsFullyInitialized(void)
{
	return(Singleton::IsFullyInitialized());
}

FileHandler::FileHandler(void)
{
	// If the File Handler is initialized, run max data point reference initialize
	FileSizeReferenceCreate();
}

FileHandler::~FileHandler()
{
	// Left Intentionally Blank
}

FileHandler & FileHandler::operator=(const FileHandler & source)
{
	return *this;
}

/*
bool FileHandler::logAppend(FileHandlerIDEnum logType, MessageCodeType dataOne, MessageCodeType dataTwo)
{
	Phoenix::Servers::GPSServer * gpsServer = dynamic_cast<Phoenix::Servers::GPSServer *>(Factory::GetInstance(GPS_SERVER_SINGLETON));
	char file[50];
	int fd;
	uint32 seconds;
	bool isMade = true;

	Phoenix::Servers::GPSData * gpsData = gpsServer->GetGPSDataPtr();
	int week = gpsData->GPSWeek;

	if(!((this->FileExists(errLog))&&(logType) == LOG_ERROR) || !((this->FileExists(modeLog))&&(logType) == LOG_MODE))
	{
		isMade = false;
	}

	if (true == TakeLock(MAX_BLOCK_TIME))
	{
		if(!isMade)
		{
			if(logType == LOG_MODE)
			{
				file[0] = modeLog[0];
				file[1] = modeLog[1];
				file[2] = modeLog[2];
				file[3] = modeLog[3];
				file[4] = modeLog[4];
				file[5] = modeLog[5];
				file[6] = modeLog[6];
				file[7] = modeLog[7];
				file[8] = modeLog[8];
				file[9] = modeLog[9];
				file[10] = modeLog[10];
				file[11] = modeLog[11];
				file[12] = modeLog[12];

			}
			else
			{
				file[0] = errLog[0];
				file[1] = errLog[1];
				file[2] = errLog[2];
				file[3] = errLog[3];
				file[4] = errLog[4];
				file[5] = errLog[5];
				file[6] = errLog[6];
				file[7] = errLog[7];
				file[8] = errLog[8];
				file[9] = errLog[9];
				file[10] = errLog[10];
				file[11] = errLog[11];
				file[12] = errLog[12];
				file[13] = errLog[13];
			}
			// open the file.
			fd = open(file, O_CREAT);
			if(fd < 0)
			{
				// error: failed to open the new file
				GiveLock( );
				return false;
			}
			if (close(fd) == -1)
			{
				// error: couldn't close the file
				GiveLock( );
				return false;
			}

			//write new file to list of file names
			char truDat[] = "tru.dat";
			// open the file.
			fd = open(truDat, O_APPEND);
			if(fd < 0)
			{
				// error: failed to open the new file
				GiveLock( );
				return false;
			}

			// Write into the file.
			if(write(fd, file, strlen(file)) == -1)
			{
				// error: failed to write correctly
				close(fd);
				GiveLock( );
				return false;
			}
			//insert carriage return and new line feed
			char newLine[] = {13, 10};
			// Write into the file.
			if(write(fd, newLine, 2) == -1)
			{
				// error: failed to write correctly
				close(fd);
				GiveLock( );
				return false;
			}

			if (close(fd) == -1)
			{
				// error: couldn't close the file
				GiveLock( );
				return false;
			}
		}

		// open the file.
		fd = open(file, O_APPEND);
		if(fd < 0)
		{
			// error: failed to open the new file
			GiveLock( );
			return false;
		}

		uint8 *buffer = new uint8[10];

		buffer[0] = (uint8)((week >> 24) & 0xff);
		buffer[1] = (uint8)((week >> 16) & 0xff);
		buffer[2] = (uint8)((week >> 8) & 0xff);
		buffer[3] = (uint8)(week & 0xff);

		if(!(Phoenix::HAL::RTCGetTime(&seconds, NULL)))
		{
			delete buffer;
			GiveLock( );
			return false;
		}

		buffer[4] = (uint8)((seconds >> 24) & 0xff);
		buffer[5] = (uint8)((seconds >> 16) & 0xff);
		buffer[6] = (uint8)((seconds >> 8) & 0xff);
		buffer[7] = (uint8)(seconds & 0xff);
		buffer[8] = (uint8) dataOne;
		buffer[9] = (uint8) dataTwo;


		// Write into the file.
		if(write(fd, buffer, 10) == -1)
		{
			// error: failed to write correctly
			close(fd);
			GiveLock( );
			return false;
		}
		if (close(fd) == -1)
		{
			// error: couldn't close the file
			GiveLock( );
			return false;
		}
		GiveLock( );
	}
	else
	{
		return false;
	}

	// Success!
	return true;

}
*/

///////////////////////////////////
// Append to Data File
///////////////////////////////////
bool FileHandler::Append(FileHandlerIDEnum subsystem, MessageCodeType opCode, const MultiDataMessage & message)
{
	//#ifndef WIN32
	//Phoenix::Servers::GPSServer * gpsServer = dynamic_cast<Phoenix::Servers::GPSServer *>(Factory::GetInstance(GPS_SERVER_SINGLETON));
	string file;
	string filePath;
	string tempFile;
	string fileExtension;
	string zipSubSystem;
	char *temp = new char[25];
	fstream fd;
	uint32 seconds;
	//TODO: add the GPS stuff back
	//Phoenix::Servers::GPSData * gpsData = gpsServer->GetGPSDataPtr();
	int week = 1;// gpsData->GPSWeek;

	filePath = writeDir;

	//Type of the files
	fileExtension = ".dat";

	// File name lookup
	switch(subsystem)
	{
	case SUBSYSTEM_ACS :
		zipSubSystem = "ACS";
		filePath.append("ACS/");
		file = "ACS_";
		break;
	case SUBSYSTEM_COM :
		zipSubSystem = "COM";
		filePath.append("COM/");
		file = "COM_";
		break;
	case SUBSYSTEM_EPS :
		zipSubSystem = "EPS";
		filePath.append("EPS/");
		file = "EPS_";
		break;
	case SUBSYSTEM_GPS :
		zipSubSystem = "GPS";
		filePath.append("GPS/");
		file = "GPS_";
		break;
	case SUBSYSTEM_PLD :
		zipSubSystem = "PLD";
		filePath.append("PLD/");
		file = "PLD_";
		break;
	case SUBSYSTEM_THM :
		zipSubSystem = "THM";
		filePath.append("THM/");
		file = "THM_";
		break;
	case SUBSYSTEM_SCH :
		zipSubSystem = "SCH";
		filePath.append("SCH/");
		file = "SCH_";
		break;
	case SUBSYSTEM_STR :
		zipSubSystem = "STR";
		filePath.append("STR/");
		file = "STR_";
		break;
	case SUBSYSTEM_CMD :
		zipSubSystem = "CMD";
		filePath.append("CMD/");
		file = "CMD_";
		break;
	case SYSTEM_CDH :
		zipSubSystem = "CDH";
		filePath.append("CDH/");
		file = "CDH_";
		break;
	default:
		delete temp;
		return false;
	}
	mkdir(filePath.c_str(), 0777);

	itoa(opCode, temp, 10);
	file.append(temp);
	file.append("_");


	itoa(epochNum, temp, 10);
	file.append(temp);
	file.append("_");

	itoa(week, temp, 10);
	file.append(temp);
	file.append("_");

	if (true == TakeLock(MAX_BLOCK_TIME))
	{
		// Check if the file is full or doesn't exist (data points == 0)
		if((numDataPoints[subsystem][opCode] == 0) ||(numDataPoints[subsystem][opCode] >= numDataPointsMax[subsystem][opCode]))
		{

			if((numDataPoints[subsystem][opCode] >= numDataPointsMax[subsystem][opCode]))
			{
				tempFile = file;
				seconds = secRef[subsystem][opCode];
				itoa(seconds, temp, 10);
				file.append(temp);
				file.append(fileExtension);

				DeleteFile(file.c_str());
				file = tempFile;
			}
			//TODO: add the RTC back
			if(!(Phoenix::HAL::RTCGetTime(&seconds, NULL)))
			{
				GiveLock( );
				delete temp;
				return false;
			}

			if(seconds > 999999)
			{
				RTCSetTime(0);
				seconds = 0;
			}

			itoa(seconds, temp, 10);
			file.append(temp);
			file.append(fileExtension);

			this->unzipFile(filePath.c_str(), file.c_str(), zipSubSystem.c_str());
			tempFile = filePath;
			tempFile.append(file);
			file = tempFile;

			// open the file.
			fd.open(file.c_str(), ios::out | ios::app);
			if(!fd.is_open())
			{
				// error: failed to open the new file
				GiveLock( );
				delete temp;
				return false;
			}
			fd.close();
			if (fd.is_open())
			{
				// error: couldn't close the file
				GiveLock( );
				delete temp;
				return false;
			}
			secRef[subsystem][opCode] = seconds;
			weekRef[subsystem][opCode] = week;
			// Reset data point count
			numDataPoints[subsystem][opCode] = 0;
		}
		else
		{
			seconds = secRef[subsystem][opCode];
			week = weekRef[subsystem][opCode];
			// Write file name

			itoa(seconds, temp, 10);
			file.append(temp);
			file.append(fileExtension);

			this->unzipFile(filePath.c_str(), file.c_str(), zipSubSystem.c_str());
			tempFile = filePath;
			tempFile.append(file);
			file = tempFile;
		}
		delete temp;


		// open the file.
		fd.open(file.c_str(), ios::out | ios::app);
		if(!fd.is_open())
		{
			// error: failed to open the new file
			GiveLock( );
			return false;
		}
		//Get time stamp, both week and seconds, cast to variable type data//
		size_t size = 0;

		if((subsystem != SUBSYSTEM_PLD) || (subsystem == SUBSYSTEM_PLD && opCode != PLD_DATA_SUCCESS))
		{
			size += 9;
		}

		//Size holds the size of the buffer we will be storing the data to write//

		//Extract message from MultiDataMessage an place into list params//
		list<VariableTypeData *> params;
		params = message.GetParameters();
		uint8 numParams = 0;
		//Iterate through the params list and increment size for each value in params//
		std::list<VariableTypeData *>::iterator it = params.begin();
		for( ; it != params.end(); it++)
		{
			size += (*it)->GetFlattenSize();
			numParams += 1;
		}

		//buffer keeps pointer to the whole buffer, bufferPtr points to the next place to flattened data//
		uint8 *buffer = new uint8[size];
		uint8 *bufferPtr = buffer;


		if((subsystem != SUBSYSTEM_PLD) || (subsystem == SUBSYSTEM_PLD && opCode != PLD_DATA_SUCCESS))
		{
			buffer[0] = (uint8)((week >> 24) & 0xff);
			buffer[1] = (uint8)((week >> 16) & 0xff);
			buffer[2] = (uint8)((week >> 8) & 0xff);
			buffer[3] = (uint8)(week & 0xff);
			//TODO: add the RTC back
			if(!(Phoenix::HAL::RTCGetTime(&seconds, NULL)))
			{
				delete buffer;
				GiveLock( );
				return false;
			}

			buffer[4] = (uint8)((seconds >> 24) & 0xff);
			buffer[5] = (uint8)((seconds >> 16) & 0xff);
			buffer[6] = (uint8)((seconds >> 8) & 0xff);
			buffer[7] = (uint8)(seconds & 0xff);

			bufferPtr += 8;
		}

		buffer[8] = numParams;
		bufferPtr += 1;

		it = params.begin();
		for( ; it != params.end(); it++)
		{
			bufferPtr += (*it)->Flatten(bufferPtr, (*it)->GetFlattenSize());
		}

		if((subsystem == SUBSYSTEM_PLD) && (opCode == PLD_DATA_SUCCESS))
		{
			size -= 5;
			bufferPtr = buffer + 5; //remove variable type data header
			memcpy(buffer, bufferPtr, size);
		}

		// Write into the file.
		fd.write((char *) buffer, (long int) size);
		if(fd.bad())
		{
			// error: failed to write correctly
			fd.close();
			GiveLock( );
			return false;
		}
		fd.close();
		if (fd.is_open())
		{
			// error: couldn't close the file
			GiveLock( );
			return false;
		}
		GiveLock( );
	}
	else
	{
		return false;
	}
	// Increment Number of Data Points in File
	++numDataPoints[subsystem][opCode];

	//#endif //WIN32
	// Success!
	if (this->folderSize(filePath.c_str()) > sizeToZip)
		this->zipFiles(filePath.c_str(), zipSubSystem.c_str());
	return true;
}

size_t removeSubstring(char * source, const char * toRemove, size_t sourceLength)
{
	//get the start position of the string we want to remove
	char * removeBeg = strstr(source, toRemove);
	if(removeBeg == NULL)
	{
		return 0;
	}

	char * removeEnd = removeBeg + strlen(toRemove);
	//move everything after the string to the start;
	char * end = source + sourceLength;
	//memmove(removeBeg, removeEnd, (end - removeEnd));
	for(; removeEnd != end; removeEnd++)
	{
		*removeBeg = *removeEnd;
		removeBeg++; 
	}
	return (sourceLength - strlen(toRemove));
}

///////////////////////////////////
// Delete Data File
///////////////////////////////////
bool FileHandler::DeleteFile(const char * fileName)
{
	//

	fstream file;
	if (true == TakeLock(MAX_BLOCK_TIME))
	{
		file.open(fileName, ios::in);
		if(!file.is_open()){
			GiveLock();
			return false;
		}
		file.close();
		if(file.is_open()){
			// error: couldn't close the file
			GiveLock( );
			return false;
		}
		if(remove(fileName)!=0){
			// error: failed to delete the file
			GiveLock( );
			return false;
		}
		GiveLock();
	}
	// Success!
	return true;
}

uint8 * FileHandler::ReadFile(const char * fileName, size_t * bufferSize)
{
	uint8 * buffer = NULL;
	//open file for read only
	fstream file;
	if (true == TakeLock(MAX_BLOCK_TIME)){
		file.open(fileName, ios::in);
		if(!file.is_open()){
			// error: failed to open the file
			buffer = NULL;
			GiveLock( );
			return buffer;
		}
		//length of the file
		* bufferSize = (size_t) this->fileSize(file);
		buffer = new uint8[(* bufferSize)];
		file.read((char *) buffer, * bufferSize);
		if(file.bad()){
			file.close();
			delete buffer;
			buffer = NULL;
			GiveLock( );
			return buffer;
		}
		file.close();
		if(file.is_open()){
			// error: couldn't close the file
			GiveLock( );
			return false;
		}
		GiveLock();
	}
	return buffer;
}

uint32 FileHandler::FileExists(const char * fileName)
{
	uint32 rv = 0;
	fstream file;
	if (true == TakeLock(MAX_BLOCK_TIME))
	{
		//FILE * fd = fopen(fileName, "r");
		file.open(fileName, ios::out);
		if(!file.is_open())
		{
			// error: failed to open the file
			GiveLock( );
			return -1;
		}
		rv = (uint32) this->fileSize(file);
		file.close();
		if(file.is_open()){
			// error: couldn't close the file
			GiveLock( );
			return -2;
		}
		GiveLock( );
	}
	return rv;
}


uint32 FileHandler::FileWrite(char * fileName, char * buffer, size_t numBytes)
{
	uint32 rv = 0;
	//open file for write only
	fstream file;
	if (true == TakeLock(MAX_BLOCK_TIME)){
		file.open(fileName, ios::out  | ios::app);
		if(!file.is_open()){
			// error: failed to open the file
			GiveLock( );
			return -1;
		}
		//length of the file
		file.write(buffer, numBytes);
		if(file.bad()){
			file.close();
			GiveLock( );
			return -2;
		}
		rv = (uint32) this->fileSize(file);
		file.close();
		if(file.is_open()){
			// error: couldn't close the file
			GiveLock( );
			return -3;
		}
		GiveLock( );
	}
	return rv;
}

//////////////////////////////////////////////
// Create Max Data Point Reference Table
//////////////////////////////////////////////
void FileHandler::FileSizeReferenceCreate()
{
	// TODO: Make this assign real CON-OPS values
	// Assign enumerations to reference array
	for(size_t i = 0; i < SYSTEM_MAX; i++)
	{
		for(size_t j = 0; j < MAX_OPCODES; j++)
		{
			numDataPointsMax[i][j] = MAX_DATA_POINTS;
			numDataPoints[i][j] = 0;
		}
	}
}

//void FileHandler::FileSizePLDPicture(uint32 resolution, uint32 chunckSize)
//{
//	numDataPointsMax[SUBSYSTEM_PLD][PLD_PIC_CMD] = resolution/chunckSize;
//	numDataPoints[SUBSYSTEM_PLD][PLD_PIC_CMD] = 0;
//}


bool FileHandler::InitEpoch()
{
	fstream file;
	char fileName[] = "epoch.txt";
	if (true == TakeLock(MAX_BLOCK_TIME))
	{
		file.open(fileName, ios::out);
		if(!file.is_open()){
			GiveLock();
			return false;
		}

		char buffer[1];
		buffer [0] = 94;
		file.write(buffer, 1);
		if(file.bad()){
			file.close();
			GiveLock();
			return false;
		}
		file.close();
		if(file.is_open()){
			GiveLock();
			return false;
		}
		GiveLock();
	}
	return true;
}

uint16 FileHandler::GetEpoch()
{
	fstream file;
	char fileName[] = "epoch.txt";
	uint16 epochSize = 0;
	if (true == TakeLock(MAX_BLOCK_TIME))
	{
		file.open(fileName, ios::in);
		if(!file.is_open()){
			GiveLock();
			return 0;
		}

		epochSize = (uint16) this->fileSize(file);
		file.close();
		if(file.is_open()){
			GiveLock();
			return 0;
		}
		GiveLock();
	}
	return epochSize;
}

bool FileHandler::KermitLock(void)
{
	DEBUG_COUT("				KermitLock: Called")
	if (true == TakeLock(MAX_BLOCK_TIME))
	{
		DEBUG_COUT("				KermitLock: success")
		return true;
	}
	DEBUG_COUT("				KermitLock: failed")
	return false;
}

void FileHandler::KermitUnlock(void)
{
	DEBUG_COUT("				KermitUnlock: Called")
	GiveLock( );
}

int FileHandler::fileSize(fstream & file){
	file.seekg (0, file.end);
	int length = file.tellg();
	file.seekg (0, file.beg);
	return length;
}

uint32 FileHandler::fileSize(char * fileName)
{
	uint32 rv = 0;
	//open file for read only
	fstream file;
	if (true == TakeLock(MAX_BLOCK_TIME)){
		file.open(fileName, ios::in);
		if(!file.is_open()){
			// error: failed to open the file
			GiveLock( );
			return -1;
		}
		//length of the file
		rv = (uint32) this->fileSize(file);
		file.close();
		if(file.is_open()){
			// error: couldn't close the file
			GiveLock( );
			return -2;
		}
		GiveLock( );
	}
	return rv;
}

void FileHandler::zipFiles(const char * path, const char * zipName){
	struct stat buffer;
	string cmd;
	string zipPath = writeDir;
	zipPath.append(zipName);
	if( 0 == stat(zipPath.c_str(), &buffer))
		unzipFiles(path, zipName);
	cmd = "zip -9 -j -qq -r -m ";
	cmd.append(zipPath);
	cmd.append(" ");
	cmd.append(path);
	cmd.append(" > /dev/null 2>&1");
	system(cmd.c_str());
}

void FileHandler::unzipFiles(const char * path, const char * zipName){
	struct stat buffer;
	string cmd;
	string zipPath = writeDir;
	zipPath.append(zipName);
	cmd = "unzip -n -qq ";
	cmd.append(zipPath);
	cmd.append(" -d ");
	cmd.append(path);
	cmd.append(" > /dev/null 2>&1");
	system(cmd.c_str());
}

void FileHandler::unzipFile(const char * path, const char * file, const char * zipName){
	struct stat buffer;
	string cmd;
	string zipPath = writeDir;
	zipPath.append(zipName);
	cmd = "unzip -n -qq ";
	cmd.append(zipPath);
	cmd.append(" ");
	cmd.append(file);
	cmd.append(" -d ");
	cmd.append(path);
	cmd.append(" > /dev/null 2>&1");
	system(cmd.c_str());
}

unsigned int FileHandler::folderSize(const char * path)
{
	DIR *dp;
	unsigned int size;
	struct dirent *ep;
	struct stat sbuffer;
	dp = opendir(path);
	size=0;
	if (dp != NULL) {
		while(ep= readdir(dp)) {
			if (strcmp(ep->d_name, "..") != 0 && strcmp(ep->d_name, ".") != 0){
				switch (ep->d_type) {
					case DT_UNKNOWN :
					case DT_REG     : break;
					default		    : break;
				}
				string file;
				file=path;
				file.append((char*)ep->d_name);
				stat(file.c_str(), &sbuffer);
				size = size + (unsigned int) sbuffer.st_size;
			}
		}
		closedir(dp);
	}
	return size;
}
