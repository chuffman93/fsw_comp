/*
 * FileHandler.cpp
 *
 *  Created on: Aug 7, 2012
 *      Author: Caitlyn Cooke
 *
 *  Modified: February, 2016
 *  	Authors: Alex St. Clair, Adam St. Amand
 */

/*TODO List
 * 1.) Change over to C style file pointers XXXXXXFinishedXXXXX
 * 2.) Implement Time Retrieval (GPS)
 * 3.) Design Testing to exercise all the above
 * 4.) Storage Management (Delete things when needed/Recognize when storage is full).
 * Implement storage monitor
 *
 */

/*
 *TODO
 * What happens if the GPS goes down? File naming
 * will retrieve same information and begin writing
 * files larger than they should
 *
 * ADD GPS BACK!
 */

#include <cstring>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>

#include <iostream>
#include "servers/GPSServer.h"
#include "util/FileHandler.h"
#include "util/Logger.h"
#include "util/itoa.h"
#include "HAL/RTC.h"

//size in bytes that is required to compress the files

#define FILE_HANDLER_DEBUG                      0
#define DEBUG_VAL(m, val) cout << m << ": " << val << endl;
#define DEBUG_COUT(m) cout << m << endl;

// Instantiate static members
const char * FileHandler::modeLog = NULL;
const char * writeDir = "/home/root/filehandler/";
const char * truDat = "/home/root/filehandler/tru.dat";
const unsigned int sizeToZip = 10 * (1024);
uint32 crcTable[256];


using namespace Phoenix::HAL;
using namespace Phoenix::Core;
using namespace Phoenix::Servers;
using namespace std;


////////////////////////////////
/////// Initialization /////////
////////////////////////////////

void FileHandler::Initialize(void)
{
        modeLog = "Mode_Log.dat";
}

#ifdef WIN32
void FileHandler::Destroy(void)
{

}
#endif

bool FileHandler::IsFullyInitialized(void)
{
        return (Singleton::IsFullyInitialized());
}

FileHandler::FileHandler(void)
{
        // If the File Handler is initialized, run max data point reference initialize
        makeCrcTable(crcTable);
}

FileHandler::~FileHandler()
{
        // Left Intentionally Blank
}

FileHandler & FileHandler::operator=(const FileHandler & source)
{
        return *this;
}


/////////////////////////////////////
/////////// Log Data ////////////////
/////////////////////////////////////

bool FileHandler::Log(FileHandlerIDEnum subsystem, const Phoenix::Core::FSWPacket * packet){
	GPSServer * gpsServer = dynamic_cast<GPSServer *> (Factory::GetInstance(GPS_SERVER_SINGLETON));
	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));

	uint8 opcode = packet->GetOpcode();
	bool notPLDData = (subsystem != SUBSYSTEM_PLD) || (opcode != PLD_DATA_SUCCESS);
	string file;

	// Fetch Time
	int week = gpsServer->GetWeek();
	int seconds = gpsServer->GetSeconds();

	// Get filename from dedicated method
	FetchFileName(subsystem, opcode, &file);
	logger->Log("FileHandler: Log(): Filename: %s", file, LOGGER_LEVEL_DEBUG);

	// Fill buffer with data to be written and write buffer to file
	size_t size = packet->GetMessageLength();
	if(notPLDData) size += 8;
	uint8 * buffer = new uint8[size];

	// keep a pointer for current location in buffer
	uint8 *bufferPtr = buffer;
	if(notPLDData){
		buffer[0] = (uint8) ((week >> 24) & 0xff);
		buffer[1] = (uint8) ((week >> 16) & 0xff);
		buffer[2] = (uint8) ((week >> 8) & 0xff);
		buffer[3] = (uint8) (week & 0xff);
		buffer[4] = (uint8) ((seconds >> 24) & 0xff);
		buffer[5] = (uint8) ((seconds >> 16) & 0xff);
		buffer[6] = (uint8) ((seconds >> 8) & 0xff);
		buffer[7] = (uint8) (seconds & 0xff);
		bufferPtr += 8;
	}

	if((size - (bufferPtr - buffer)) != packet->GetMessageLength()){
		logger->Log("FileHandler: Log(): Message length error!", LOGGER_LEVEL_ERROR);
		delete[] buffer;
		return false;
	}

	// Add message to the write buffer
	memcpy(bufferPtr, packet->GetMessageBufPtr(), packet->GetMessageLength());

	// Write into the file.
	logger->Log("FileHandler: Log(): File write size: %u", (uint32) size, LOGGER_LEVEL_DEBUG);
	int err = FileWrite(file.c_str(), (char*) buffer, (long int) size);
	delete[] buffer;
	if (err < 0){
		return false;
	}
	return true;
}

uint8_t FileHandler::FetchFileName(FileHandlerIDEnum subsystem, MessageCodeType opCode, string * file){
	GPSServer * gpsServer = dynamic_cast<GPSServer *> (Factory::GetInstance(GPS_SERVER_SINGLETON));
	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));

    string filePath;
    string tempFile;
    string fileExtension;
    filePath = writeDir;
    char *temp = new char[25];
    fileExtension = ".dat"; //Type of the files

    //Build file and filepath name using subsystem name and opcode
	switch (subsystem)
	{
		case SUBSYSTEM_ACS:
				filePath.append("ACS/");
				file->append("ACS_");
				break;
		case SUBSYSTEM_COM:
				filePath.append("COM/");
				*file = "COM_";
				break;
		case SUBSYSTEM_EPS:
				filePath.append("EPS/");
				*file = "EPS_";
				break;
		case SUBSYSTEM_GPS:
				filePath.append("GPS/");
				*file = "GPS_";
				break;
		case SUBSYSTEM_PLD:
				filePath.append("PLD/");
				*file = "PLD_";
				break;
		case SUBSYSTEM_SCH:
				filePath.append("SCH/");
				*file = "SCH_";
				break;
		case SUBSYSTEM_CMD:
				filePath.append("CMD/");
				*file = "CMD_";
				break;
		case SYSTEM_CDH:
				filePath.append("CDH/");
				*file = "CDH_";
				break;
		default:
				logger->Log("FileHandler: unknown server for file path", LOGGER_LEVEL_WARN);
				delete temp;
				return -1;
	}

	mkdir(filePath.c_str(), 0777);

	sprintf(temp, "%03d", opCode);
	file->append(temp);
	file->append("_");
	// Create current file name
	tempFile = *file;
	sprintf(temp, "%05d", weekRef[subsystem][opCode]);
	tempFile.append(temp);
	tempFile.append("_");
	sprintf(temp, "%06d", secRef[subsystem][opCode]);
	tempFile.append(temp);
	tempFile.insert(0, filePath);
	tempFile.append(fileExtension);
	// Check if old file is full
	if (fileSize(tempFile.c_str()) >= MAXFILESIZE){
		logger->Log("FileHandler: FetchFileName(): file full or nonexistent", LOGGER_LEVEL_DEBUG);
		//get current time in seconds and week
		uint16 week = gpsServer->GetWeek();
		uint32 secs = gpsServer->GetRoundSeconds();
		weekRef[subsystem][opCode] = week;
		secRef[subsystem][opCode] = secs;
		sprintf(temp, "%05d", week);
		file->append(temp);
		file->append("_");
		sprintf(temp, "%06d", secs);
		file->append(temp);
		*file = filePath.append(file->append(fileExtension));
	}else{
		*file = tempFile;
	}
	delete temp;
	return 0;
}




////////////////////////////////
//////// Utilities /////////////
////////////////////////////////

uint8 * FileHandler::ReadFile(const char * fileName, size_t * bufferSize)
{
	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
	char * out = new char[150];
	sprintf(out, "FileHandler: ReadFile(): %s", fileName);
	logger->Log(out, LOGGER_LEVEL_DEBUG);
	delete out;

	uint8 * buffer = NULL;
	FILE * fp;
	size_t result;
	if (true == TakeLock(MAX_BLOCK_TIME))
	{
		fp = fopen(fileName, "r");
		if (!fp)
		{
			// error: failed to open the file
			buffer = NULL;
			this->GiveLock();
			return buffer;
		}

		// Length of the file
		*bufferSize = fileSize(fp);

		// Create buffer
		buffer = new uint8[*bufferSize];
		result = fread(buffer,1,*bufferSize,fp);

		// Check for correct read
		if (result != *bufferSize)
		{
			fclose(fp);
			delete[] buffer;
			buffer = NULL;
			this->GiveLock();
			return buffer;
		}

		// Cleanup
		fclose(fp);
		this->GiveLock();
	}
	return buffer;
}

//Append instead of writing?
uint32 FileHandler::FileWrite(const char * fileName, char * buffer, size_t numBytes)
{
	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
	logger->Log("FileHandler: FileWrite(): %s", fileName, LOGGER_LEVEL_DEBUG);

	uint32 rv = 0;
	size_t result;

	//open file for write only
	FILE * fp;
	if (true == TakeLock(MAX_BLOCK_TIME))
	{
		fp = fopen(fileName, "a");
		if (!fp)
		{
			logger->Log("FileWrite(): Failed on file pointer", LOGGER_LEVEL_WARN);
			this->GiveLock();
			return -1;
		}
		result = fwrite(buffer,1,numBytes,fp);

		if(result!=numBytes){
			logger->Log("FileWrite(): Wrote the wrong number of bytes!", LOGGER_LEVEL_WARN);
			fclose(fp);
			this->GiveLock();
			return -2;
		}

		rv = (uint32) this->fileSize(fp);
		fclose(fp);
		this->GiveLock();
	}
	return rv;
}

bool FileHandler::DeleteFile(const char * fileName)
{
	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
	char * out = new char[150];
	sprintf(out, "FileHandler: DeleteFile(): %s", fileName);
	logger->Log(out, LOGGER_LEVEL_DEBUG);
	delete out;
	if (true == TakeLock(MAX_BLOCK_TIME))
	{
		if (remove(fileName) != 0)
		{
			// error: failed to delete the file
			this->GiveLock();
			return false;
		}
		this->GiveLock();
	}
	// Success!
	return true;
}

uint32 FileHandler::fileSize(FILE * fp)
{
	fseek(fp, 0L, SEEK_END);
	uint32 size = (size_t) ftell(fp);
	fseek(fp, 0L, SEEK_SET);
	return size;
}

uint32 FileHandler::fileSize(const char * fileName)
{
	uint32 rv = 0;
	//open file for read only
	FILE * fp;
	if (true == TakeLock(MAX_BLOCK_TIME)){
		fp = fopen(fileName, "r");
		if (!fp){
			// error: failed to open the file
			this->GiveLock();
			return -1;
		}

		fseek(fp, 0L, SEEK_END);
		rv = (uint32) ftell(fp);
		fseek(fp, 0L, SEEK_SET);

		fclose(fp);
		this->GiveLock();
	}
	return rv;
}

void FileHandler::zipFiles(const char * path, const char * zipName)
{
	struct stat buffer;
	string cmd;
	string zipPath = writeDir;
	zipPath.append(zipName);
	if (0 == stat(zipPath.c_str(), &buffer))
			unzipFiles(path, zipName);
	cmd = "zip -9 -j -qq -r -m ";
	cmd.append(zipPath);
	cmd.append(" ");
	cmd.append(path);
	cmd.append(" > /dev/null 2>&1");
	system(cmd.c_str());
}

//Look at application and see if only one can be used
void FileHandler::unzipFiles(const char * path, const char * zipName)
{
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

void FileHandler::unzipFile(const char * path, const char * file,
                const char * zipName)
{
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

//Check if necessecary
unsigned int FileHandler::folderSize(const char * path)
{
	DIR *dp;
	unsigned int size;
	struct dirent *ep;
	struct stat sbuffer;

	dp = opendir(path);
	size = 0;
	if (dp != NULL){
		while ((ep = readdir(dp))){
			if (strcmp(ep->d_name, "..") != 0 && strcmp(ep->d_name, ".") != 0){
				switch (ep->d_type){
					case DT_UNKNOWN:
					case DT_REG:
						break;
					default:
						break;
				}
				string file;
				file = path;
				file.append((char*) ep->d_name);
				stat(file.c_str(), &sbuffer);
				size = size + (unsigned int) sbuffer.st_size;
			}
		}
		closedir(dp);
	}
	return size;
}

//////////////////////
///////////CRC////////
//////////////////////

void FileHandler::makeCrcTable(uint32 crcTable[])
{
    uint32 POLYNOMIAL = 0xEDB88320; //0x04C11DB7 reversed
    uint32 remainder;
    unsigned char b = 0;
    do{
        // Start with the data byte
        remainder = b;
        for (unsigned short bit = 8; bit > 0; --bit)
        {
            if (remainder & 1)
                remainder = (remainder >> 1) ^ POLYNOMIAL;
            else
                remainder = (remainder >> 1);
        }
        crcTable[(size_t)b] = remainder;
    } while(0 != ++b);
}

unsigned int FileHandler::generateCrc(uint8 *p, size_t n, unsigned int crcTable[], unsigned int crc)
{
	size_t i;
    for(i = 0; i < n; i++)
    {
        crc = crcTable[*p++ ^ (crc&0xff)] ^ (crc>>8);
    }
    return(~crc);
}

//CRC-32 (0x04C11DB7)
uint32 FileHandler::crcCheck(const char * fileName)
{
	uint32 crc = 0x00000000ul;
	//uint32 crcTable[256];
	//makeCrcTable(crcTable);
	size_t bufferSize;
	FILE* fp;
	uint8 * buffer;
	if (true == this->TakeLock(MAX_BLOCK_TIME))
		{
			fp = fopen(fileName, "r");
			bufferSize = (size_t) this->fileSize(fp);
			//Check BuffersSize for error
			//crc = NULL;
			//this->GiveLock();
			//return crc;
			buffer = new uint8[bufferSize];
			buffer = ReadFile(fileName, &bufferSize);
			//Check buffer output for error TODO
			//delete buffer;
			//buffer = NULL;
			//this->GiveLock();
			//return crc;

			//Calculate CRC
			for(uint8 i=0; i<bufferSize; i++){
				crc = generateCrc(buffer +  i,1,(unsigned int *) crcTable, (unsigned int)~crc);
			}

			delete[] buffer;
			this->GiveLock();
		}
	return crc;
}
