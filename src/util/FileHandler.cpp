/*
 * FileHandler.cpp
 *
 *  Created on: Aug 7, 2012
 *      Author: Caitlyn Cooke
 *
 *  Modified: February, 2016
 *  	Authors: Alex St. Clair, Adam St. Amand
 */

/*TODO List - BEFORE TUESDAY
 * 1.) Change over to C style file pointers XXXXXXFinishedXXXXX
 * 2.) Implement Time Retrieval (GPS)
 * 3.) Design Testing to exercise all the above
 *AFTER TUESDAY
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
#include "util/itoa.h"
#include "HAL/RTC.h"

//size in bytes that is required to compress the files

#define FILE_HANDLER_DEBUG                      0
#define DEBUG_VAL(m, val) cout << m << ": " << val << endl;
#define DEBUG_COUT(m) cout << m << endl;

// Instantiate static members
const char * FileHandler::errLog = NULL;
const char * FileHandler::modeLog = NULL;
const char * writeDir = "/home/root/filehandler/";
const char * truDat = "/home/root/filehandler/tru.dat";
const unsigned int sizeToZip = 10 * (1024);
uint32 crcTable[256];


using namespace Phoenix::HAL;
using namespace Phoenix::Core;
using namespace std;




//////////////////////////////
//////////Initialization//////
//////////////////////////////

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
        return (Singleton::IsFullyInitialized());
}

FileHandler::FileHandler(void)
{
        // If the File Handler is initialized, run max data point reference initialize
        //FileSizeReferenceCreate();
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
/////////// Log Data/////////////////
/////////////////////////////////////

bool FileHandler::Log(FileHandlerIDEnum logType, MessageCodeType dataOne, MessageCodeType dataTwo)
{
        Phoenix::Servers::GPSServer * gpsServer =
                        dynamic_cast<Phoenix::Servers::GPSServer *>(Factory::GetInstance(
                                        GPS_SERVER_SINGLETON));
        string file;
        uint32 seconds;


        Phoenix::Servers::GPSData * gpsData = gpsServer->GetGPSDataPtr();
        int week = gpsData->GPSWeek;


        FetchFileName(logType, &file);

		uint8 *buffer = new uint8[10];

		buffer[0] = (uint8) ((week >> 24) & 0xff);
		buffer[1] = (uint8) ((week >> 16) & 0xff);
		buffer[2] = (uint8) ((week >> 8) & 0xff);
		buffer[3] = (uint8) (week & 0xff);

		if (!(Phoenix::HAL::RTCGetTime(&seconds, NULL)))
		{
				DEBUG_COUT("return false H");
				delete buffer;
				this->GiveLock();
				return false;
		}

		buffer[4] = (uint8) ((seconds >> 24) & 0xff);
		buffer[5] = (uint8) ((seconds >> 16) & 0xff);
		buffer[6] = (uint8) ((seconds >> 8) & 0xff);
		buffer[7] = (uint8) (seconds & 0xff);
		buffer[8] = (uint8) dataOne;
		buffer[9] = (uint8) dataTwo;



    	FileWrite(file.c_str(), (char*) buffer, 10);
        // Success!
        DEBUG_COUT("LogAppend success!");
        return true;

}

uint8_t FileHandler::FetchFileName(FileHandlerIDEnum logType, string* file){
	*file = writeDir;
	string tempFile;
    char *temp = new char[25];
    string fileExtension = ".dat";
    //int week;
    int seconds;

	//
	if (logType == LOG_MODE)
	{
			DEBUG_VAL("modeLog", modeLog);
			file->append(modeLog);
	}
	else
	{
			DEBUG_VAL("errLog", errLog);
			file->append(errLog);
	}

	mkdir(writeDir, 0777);

	//Retrieve Old file name and assign it to tempFile.
	tempFile = *file;
	//week = weekRef[subsystem][opCode];
	itoa(week, temp, 10);
	tempFile.append(temp);
	tempFile.append(fileExtension);
	delete temp;

	// Check if Old file is full. If so, create new file with current time info,
	// else assign tempFile to file.
	if (fileSize(tempFile.c_str())>=MAXFILESIZE){
		printf("file_size = %d\n", fileSize(tempFile.c_str()));
		printf("file path= %s\n", tempFile.c_str());
		puts("file full or nonexistent");
		printf("week = %d\n", week);

		//get current time in seconds and week
		char *temp2 = new char[25];
		itoa(week+2, temp2, 10);
		file->append(temp2);
		file->append(fileExtension);
		//weekRef[subsystem][opCode] = week;
	}
	else{
		*file = tempFile;
	}


	return 0;
}

bool FileHandler::Log(FileHandlerIDEnum subsystem, MessageCodeType opCode,
                const MultiDataMessage & message)
{
	string file;

	/*Fetch Time*/
	//TODO: add the GPS stuff back
	//Phoenix::Servers::GPSData * gpsData = gpsServer->GetGPSDataPtr();
	//Phoenix::Servers::GPSServer * gpsServer = dynamic_cast<Phoenix::Servers::GPSServer *>(Factory::GetInstance(GPS_SERVER_SINGLETON));
	int week = 1;   // gpsData->GPSWeek;
	int seconds = 1;
	/*Create file name using subsystem, opcode, and time. If file exists and is full, create new file, else
	 * append file.*/
	FetchFileName(subsystem, opCode, &file);

	/*Fill buffer with data to be written and write buffer to file*/
	size_t size = 0;
	uint8 *buffer = new uint8[size];
	if ((subsystem != SUBSYSTEM_PLD) || (subsystem == SUBSYSTEM_PLD && opCode != PLD_DATA_SUCCESS))
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
	for (; it != params.end(); it++)
	{
			//cout<<"Data: "<<*((uint32 *) (*it)->GetData())<<endl; //  <-- easy way to check handler data (ensure correct type casting)
			size += (*it)->GetFlattenSize();
			numParams += 1;
	}

	//buffer keeps pointer to the whole buffer, bufferPtr points to the next place to flattened data//
	uint8 *bufferPtr = buffer;
	if ((subsystem != SUBSYSTEM_PLD)
					|| (subsystem == SUBSYSTEM_PLD && opCode != PLD_DATA_SUCCESS))
	{
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

	buffer[8] = numParams;
	bufferPtr += 1;

	it = params.begin();
	for (; it != params.end(); it++)
	{
			bufferPtr += (*it)->Flatten(bufferPtr, (*it)->GetFlattenSize());
	}
	if ((subsystem == SUBSYSTEM_PLD) && (opCode == PLD_DATA_SUCCESS))
	{
			size -= 5;
			bufferPtr = buffer + 5; //remove variable type data header
			memcpy(buffer, bufferPtr, size);
	}

	// Write into the file.
	//puts("writing file..");
	//printf("string = %s\n", file.c_str());
	int err = FileWrite(file.c_str(), (char*) buffer, (long int) size);
	//printf("%d\n", err);
	//perror("hello");
	if (err < 0) return false;
	return true;
}

uint8_t FileHandler::FetchFileName(FileHandlerIDEnum subsystem, MessageCodeType opCode, string* file)
{
    string filePath;
    string tempFile;
    string fileExtension;
    char *temp = new char[25];
    filePath = writeDir;
    fileExtension = ".dat"; //Type of the files
    //int week;
    int seconds;

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
		case SUBSYSTEM_THM:
				filePath.append("THM/");
				*file = "THM_";
				break;
		case SUBSYSTEM_SCH:
				filePath.append("SCH/");
				*file = "SCH_";
				break;
		case SUBSYSTEM_STR:
				filePath.append("STR/");
				*file = "STR_";
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
				delete temp;
				return -1;
	}

	mkdir(filePath.c_str(), 0777);

	itoa(opCode, temp, 10);
	file->append(temp);
	file->append("_");
	itoa(epochNum, temp, 10);
	file->append(temp);
	file->append("_");


	//Retrieve Old file name and assign it to tempFile.
	tempFile = *file;
	int week2 = weekRef[subsystem][opCode];
	itoa(week2, temp, 10);
	tempFile.append(temp);
	tempFile.append(fileExtension);
	tempFile.insert(0, filePath);
	delete temp;

	// Check if Old file is full. If so, create new file with current time info,
	// else assign tempFile to file.
	// TODO: Add GPS back in (de-globalize week)
	if (fileSize(tempFile.c_str())>=MAXFILESIZE){
		printf("file_size = %d\n", fileSize(tempFile.c_str()));
		printf("file path= %s\n", tempFile.c_str());
		puts("file full or nonexistent");
		printf("week = %d\n", week);

		//get current time in seconds and week
		char *temp2 = new char[25];
		itoa(week, temp2, 10);
		file->append(temp2);
		*file = filePath.append(file->append(fileExtension));
		weekRef[subsystem][opCode] = week;
	}
	else{
		*file = tempFile;
	}
	return 0;
}




////////////////////////////////
////////Utilities///////////////
////////////////////////////////

uint8 * FileHandler::ReadFile(const char * fileName, size_t * bufferSize)
{
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
                        delete buffer;
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
        uint32 rv = 0;
        size_t result;
        //open file for write only
        FILE * fp;
        if (true == TakeLock(MAX_BLOCK_TIME))
        {
                fp = fopen(fileName, "a");
                cout<<"fp init: "<<fp<<endl;
                if (!fp)
				{
						this->GiveLock();
						return -1;
				}
                result = fwrite(buffer,1,numBytes,fp);

                if(result!=numBytes){
                	fclose(fp);
                	this->GiveLock();
                	return -2;
                }
                rv = (uint32) this->fileSize(fp);
                cout<<"fp final: "<<fp<<endl;
				fclose(fp); // <------------------------------------------------------------------------------------------------- why error?
                this->GiveLock();
        }
        return rv;
}

bool FileHandler::DeleteFile(const char * fileName)
{

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
        if (true == TakeLock(MAX_BLOCK_TIME))
        {
        		fp = fopen(fileName, "r");
				if (!fp)
				{
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
        if (dp != NULL)
        {
                while ((ep = readdir(dp)))
                {
                        if (strcmp(ep->d_name, "..") != 0 && strcmp(ep->d_name, ".") != 0)
                        {
                                switch (ep->d_type)
                                {
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

void FileHandler::FileSizePLDPicture(uint32 resolution, uint32 chunckSize)
{
        numDataPointsMax[SUBSYSTEM_PLD][PLD_PIC_CMD] = resolution / chunckSize;
        numDataPoints[SUBSYSTEM_PLD][PLD_PIC_CMD] = 0;
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

						delete buffer;
						this->GiveLock();
				}
		return crc;
}
