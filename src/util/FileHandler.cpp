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

#include <iostream>

#include "servers/GPSServer.h"

#include "util/FileHandler.h"
#include "util/itoa.h"

#include "HAL/RTC.h"

//size in bytes that is required to compress the files
const unsigned int sizeToZip = 10 * (1024);
uint32 crcTable[256];



#define FILE_HANDLER_DEBUG                      0


const char * writeDir = "/media/sdMount/";
const char * truDat = "/home/root/filehandler/tru.dat";

#define DEBUG_VAL(m, val) cout << m << ": " << val << endl;
#define DEBUG_COUT(m) cout << m << endl;




using namespace Phoenix::HAL;
using namespace Phoenix::Core;
using namespace std;

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
        return (Singleton::IsFullyInitialized());
}

FileHandler::FileHandler(void)
{
        // If the File Handler is initialized, run max data point reference initialize
        FileSizeReferenceCreate();
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

bool FileHandler::LogAppend(FileHandlerIDEnum logType, MessageCodeType dataOne,
                MessageCodeType dataTwo)
{
        Phoenix::Servers::GPSServer * gpsServer =
                        dynamic_cast<Phoenix::Servers::GPSServer *>(Factory::GetInstance(
                                        GPS_SERVER_SINGLETON));

        string file = writeDir;
        string errLogFullPath = writeDir;
        string modeLogFullPath = writeDir;

        uint32 seconds;
        bool isMade = true;
        ofstream f;

        Phoenix::Servers::GPSData * gpsData = gpsServer->GetGPSDataPtr();
        int week = gpsData->GPSWeek;

        errLogFullPath.append(errLog);
        modeLogFullPath.append(modeLog);

        mkdir(writeDir, 0777);


        if (!((this->FileExists(errLogFullPath)) && (logType) == LOG_ERROR)
                        || !((this->FileExists(modeLogFullPath)) && (logType) == LOG_MODE))
        {
                isMade = false;
        }

        if (TakeLock(MAX_BLOCK_TIME) == true)
        {
                if (logType == LOG_MODE)
                {
                        DEBUG_VAL("modeLog", modeLog);
                        file.append(modeLog);
                }
                else
                {
                        DEBUG_VAL("errLog", errLog);
                        file.append(errLog);
                }

                // If it's not made, make it. If it is made, jump straight to writing to it
                if (!isMade)
                {
                        DEBUG_VAL("creating file", file.c_str());
                        // create the file by opening it and flushing it
                        f.open(file.c_str(), ios::out | ios::app | ios::ate);
                        f << flush;
                        if (!f.is_open())
                        {
                                // error: failed to open the new file
                                DEBUG_COUT("return false A. May need to run as sudo");
                                this->GiveLock();
                                return false;
                        }

                        f.close();
                        if (f.is_open())
                        {
                                // error: couldn't close the file
                                DEBUG_COUT("return false B");
                                this->GiveLock();
                                return false;
                        }

                        // Append this new log file into the list of file names, if it isn't there
                        if (!this->FileExistsInTruDat(file.c_str()))
                        {
                                DEBUG_VAL("File doesn't exist in tru.dat", file.c_str());
                                f.open(truDat, ios::app);
                                if (!f.is_open())
                                {
                                        // error: failed to open the new file
                                        DEBUG_COUT("return false C");
                                        this->GiveLock();
                                        return false;
                                }

                                // Write into the file.
                                DEBUG_VAL("file.c_str()", file.c_str());
//                              DEBUG_VAL("strlen", strlen(file.c_str()));

                                f.write(file.c_str(), strlen(file.c_str()));
                                if (f.bad())
                                {
                                        // error: failed to write correctly
                                        DEBUG_COUT("return false D");
                                        f.close();
                                        this->GiveLock();
                                        return false;
                                }
                                // insert carriage return and new line feed
                                char newLine[] =
                                { 13, 10 };
                                // Write into the file.

                                f.write(newLine, 2);
                                if (f.bad())
                                {
                                        // error: failed to write correctly
                                        DEBUG_COUT("return false E");
                                        f.close();
                                        this->GiveLock();
                                        return false;
                                }
                                f.close();
                                if (f.is_open())
                                {
                                        // error: couldn't close the file
                                        DEBUG_COUT("return false F");
                                        this->GiveLock();
                                        return false;
                                }
                        } // end (if !FileExistsInTruDat)
                } // end (if !isMade)

                // open the log file.
                DEBUG_VAL("opening file", file);
//              DEBUG_VAL("file string length", strlen(file.c_str()));
                f.open(file.c_str(), ios::out | ios::app);
                if (!f.is_open())
                {
                        // error: failed to open the new file
                        this->GiveLock();
                        DEBUG_COUT("return false G");
                        return false;
                }

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

                // Write into the file.
                f.write((char *) buffer, 10);
                if (f.bad())
                {
                        // error: failed to write correctly
                        DEBUG_COUT("return false I");
                        f.close();
                        this->GiveLock();
                        return false;
                }
                f.close();
                if (f.is_open())
                {
                        // error: couldn't close the file
                        DEBUG_COUT("return false J");
                        this->GiveLock();
                        return false;
                }
                this->GiveLock();
        }
        else
        {
                DEBUG_COUT("return false K");
                return false;
        }

        // Success!
        DEBUG_COUT("LogAppend success!");
        return true;

}

///////////////////////////////////
// Append to Data File
///////////////////////////////////
bool FileHandler::Append(FileHandlerIDEnum subsystem, MessageCodeType opCode,
                const MultiDataMessage & message)
{
		//#ifndef WIN32
		//Phoenix::Servers::GPSServer * gpsServer = dynamic_cast<Phoenix::Servers::GPSServer *>(Factory::GetInstance(GPS_SERVER_SINGLETON));
        string file;
        string filePath;
        string tempFile;
        string fileExtension;
        string zipSubSystem;
        char *temp = new char[25];
        //fstream fd;
        FILE *fp;
        uint32 seconds;
        //TODO: add the GPS stuff back
        //Phoenix::Servers::GPSData * gpsData = gpsServer->GetGPSDataPtr();
        int week = 1;   // gpsData->GPSWeek;

        filePath = writeDir;
        //string filePath;

        //Type of the files
        fileExtension = ".dat";

        // File name lookup
        switch (subsystem)
        {
                case SUBSYSTEM_ACS:
                        zipSubSystem = "ACS";
                        filePath.append("ACS/");
                        file = "ACS_";
                        break;
                case SUBSYSTEM_COM:
                        zipSubSystem = "COM";
                        filePath.append("COM/");
                        file = "COM_";
                        break;
                case SUBSYSTEM_EPS:
                        zipSubSystem = "EPS";
                        filePath.append("EPS/");
                        file = "EPS_";
                        break;
                case SUBSYSTEM_GPS:
                        zipSubSystem = "GPS";
                        filePath.append("GPS/");
                        file = "GPS_";
                        break;
                case SUBSYSTEM_PLD:
                        zipSubSystem = "PLD";
                        filePath.append("PLD/");
                        file = "PLD_";
                        break;
                case SUBSYSTEM_THM:
                        zipSubSystem = "THM";
                        filePath.append("THM/");
                        file = "THM_";
                        break;
                case SUBSYSTEM_SCH:
                        zipSubSystem = "SCH";
                        filePath.append("SCH/");
                        file = "SCH_";
                        break;
                case SUBSYSTEM_STR:
                        zipSubSystem = "STR";
                        filePath.append("STR/");
                        file = "STR_";
                        break;
                case SUBSYSTEM_CMD:
                        zipSubSystem = "CMD";
                        filePath.append("CMD/");
                        file = "CMD_";
                        break;
                case SYSTEM_CDH:
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
                if ((numDataPoints[subsystem][opCode] == 0)
                                || (numDataPoints[subsystem][opCode]
                                                >= numDataPointsMax[subsystem][opCode]))
                {

                        if ((numDataPoints[subsystem][opCode]
                                        >= numDataPointsMax[subsystem][opCode]))
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
                        if (!(Phoenix::HAL::RTCGetTime(&seconds, NULL)))
                        {

                                this->GiveLock();
                                delete temp;
                                return false;
                        }

                        if (seconds > 999999)
                        {
                                RTCSetTime(0);
                                seconds = 0;
                        }

                        itoa(seconds, temp, 10);
                        file.append(temp);
                        file.append(fileExtension);

                        this->unzipFile(filePath.c_str(), file.c_str(),
                                        zipSubSystem.c_str());
                        tempFile = filePath;
                        tempFile.append(file);
                        file = tempFile;

                        //fd.open(file.c_str(), ios::out | ios::app | ios::trunc);
/*
                        if (!fd.is_open())
                        {

                                // error: failed to open the new file
                                this->GiveLock();
                                delete temp;
                                cout<<"2"<<endl;
                                cout<<FileExists(file.c_str())<<endl;
                                return false;
                        }
                        fd.close();
                        if (fd.is_open())
                        {
                       ;
                     secRef[subsystem][opCode] = seconds;
                        weekRef[subsystem][opCode] = week;
                                       // error: couldn't close the file
                                this->GiveLock();
                                delete temp;
                                cout<<"3"<<endl;
                                return false;
                        }
  */
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

                        this->unzipFile(filePath.c_str(), file.c_str(),
                                        zipSubSystem.c_str());
                        tempFile = filePath;
                        tempFile.append(file);
                        file = tempFile;
                }

                delete temp;

                fp = fopen(file.c_str(), "a");
                if (!fp){
                	cout<<"File not opened"<<endl;
                	this->GiveLock();
                	return false;
                }


                // open the file.
               /*
                *
                * fd.open(file.c_str(), ios::out | ios::app);
                if (!fd.is_open())
                {
                        // error: failed to open the new file
                        this->GiveLock();
                        return false;
                }
                */

                //Get time stamp, both week and seconds, cast to variable type data//
                size_t size = 0;

                if ((subsystem != SUBSYSTEM_PLD)
                                || (subsystem == SUBSYSTEM_PLD && opCode != PLD_DATA_SUCCESS))
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
                        size += (*it)->GetFlattenSize();
                        numParams += 1;
                }

                //buffer keeps pointer to the whole buffer, bufferPtr points to the next place to flattened data//
                uint8 *buffer = new uint8[size];
                uint8 *bufferPtr = buffer;

                if ((subsystem != SUBSYSTEM_PLD)
                                || (subsystem == SUBSYSTEM_PLD && opCode != PLD_DATA_SUCCESS))
                {
                        buffer[0] = (uint8) ((week >> 24) & 0xff);
                        buffer[1] = (uint8) ((week >> 16) & 0xff);
                        buffer[2] = (uint8) ((week >> 8) & 0xff);
                        buffer[3] = (uint8) (week & 0xff);
                        //TODO: add the RTC back
                        if (!(Phoenix::HAL::RTCGetTime(&seconds, NULL)))
                        {
                                delete buffer;
                                this->GiveLock();
                                return false;
                        }

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
                //fd.write((char *) buffer, (long int) size);
                if (fwrite((void *) buffer,1,(long int) size, fp) != (long int) size) cout<<"ADAM FALIURE TO WRITE"<<endl;


                if(fclose(fp)) cout<<"ADAM FILE DOESNT EXIST"<<endl;

                /*
                if (fd.bad())
                {
                        // error: failed to write correctly
                        fd.close();
                        this->GiveLock();
                        return false;
                }
                fd.close();
                if (fd.is_open())
                {
                        // error: couldn't close the file
                        this->GiveLock();
                        return false;
                }
                */

                this->GiveLock();
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

size_t removeSubstring(char * source, const char * toRemove,
                size_t sourceLength)
{
        //get the start position of the string we want to remove
        char * removeBeg = strstr(source, toRemove);
        if (removeBeg == NULL)
        {
                return 0;
        }

        char * removeEnd = removeBeg + strlen(toRemove);
        //move everything after the string to the start;
        char * end = source + sourceLength;
        //memmove(removeBeg, removeEnd, (end - removeEnd));
        for (; removeEnd != end; removeEnd++)
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

        FILE* fp;
        if (true == TakeLock(MAX_BLOCK_TIME))
        {
                fp = fopen(fileName, "r");
                if (!fp)
                {
                        this->GiveLock();
                        return false;
                }
                if (fclose(fp))
                {
                        // error: couldn't close the file
                        this->GiveLock();
                        return false;
                }
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


// known dependency: CMDServer subsystem loop
uint8 * FileHandler::ReadFile(const char * fileName, size_t * bufferSize)
{
        uint8 * buffer = NULL;
        //open file for read only
        fstream file;
        FILE * fp;
        uint8 result;
        if (true == TakeLock(MAX_BLOCK_TIME))
        {
                //file.open(fileName, ios::in);
                fp = fopen(fileName, "r");
                if (!fp)
                {
                        // error: failed to open the file
                        buffer = NULL;
                        this->GiveLock();
                        return buffer;
                }

                //length of the file
                *bufferSize = (size_t) this->fileSize(file);
				cout<<"Buffer Size: "<<*bufferSize<<endl;
				buffer = new uint8[((uint8) *bufferSize)];
                //file.read((char *) buffer, *bufferSize);
                result = fread(buffer,1,*bufferSize,fp);
                /*
                if (result != *bufferSize)
                {
                		fclose(fp);
                        delete buffer;
                        buffer = NULL;
                        this->GiveLock();
                        return buffer;
                }
                */
                fclose(fp);

                /*
                if (file.is_open())
                {
                        // error: couldn't close the file
                        this->GiveLock();
                }
                */
                this->GiveLock();
        }
        return buffer;
}

/*Believed to be useless. Remove at later time.*/
/*
uint32 FileHandler::FileOpens(const char * fileName)
{
        fstream file;
        if (true == TakeLock(MAX_BLOCK_TIME))
        {
                //FILE * fd = fopen(fileName, "r");
                file.open(fileName, ios::out);
                if (!file.is_open())
                {
                        // error: failed to open the file
                        this->GiveLock();
                        return false;
                }
                file.close();
                if (file.is_open())
                {
                        // error: couldn't close the file
                        this->GiveLock();
                        return false;
                }
                this->GiveLock();
                return true;
        }
        else
        {
                return false;
        }
}

*/



/*Believed to be usless. Remove at another time.*/
bool FileHandler::FileExists(const string fileName)
{
        fstream f;
        if (true == TakeLock(MAX_BLOCK_TIME))
        {
                //FILE * fd = fopen(fileName, "r");
                f.open(fileName.c_str(), ios::out | ios::app);
                if (!f.is_open())
                {
                        // error: failed to open the file
                        this->GiveLock();
                        return false;
                }
                f.close();
                if (f.is_open())
                {
                        // error: couldn't close the file
                        this->GiveLock();
                        return false;
                }
                this->GiveLock();
                return true;
        }
        else
        {
                return false;
        }
}


/*Believed to be useless. Remove at later time*/
// Used in FileHandler::LogAppend
bool FileHandler::FileExistsInTruDat(string fileToFind)
{
        ifstream f(truDat);
        for (string line; getline(f, line);)
        {
                if (line.substr(0, line.length() - 1) == fileToFind)
                {
                        DEBUG_COUT("found file match in tru.dat");
                        return true;
                }
        }

        return false;
}


uint32 FileHandler::FileWrite(char * fileName, char * buffer, size_t numBytes)
{
        uint32 rv = 0;
        //open file for write only
        fstream file;
        if (true == TakeLock(MAX_BLOCK_TIME))
        {
                file.open(fileName, ios::out | ios::app);
                if (!file.is_open())
                {
                        // error: failed to open the file
                        this->GiveLock();
                        return -1;
                }
                //length of the file
                file.write(buffer, numBytes);
                if (file.bad())
                {
                        file.close();
                        this->GiveLock();
                        return -2;
                }
                rv = (uint32) this->fileSize(file);
                file.close();
                if (file.is_open())
                {
                        // error: couldn't close the file
                        this->GiveLock();
                        return -3;
                }
                this->GiveLock();
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
        for (size_t i = 0; i < SYSTEM_MAX; i++)
        {
                for (size_t j = 0; j < MAX_OPCODES; j++)
                {
                        numDataPointsMax[i][j] = MAX_DATA_POINTS;
                        numDataPoints[i][j] = 0;
                }
        }
}

void FileHandler::FileSizePLDPicture(uint32 resolution, uint32 chunckSize)
{
        numDataPointsMax[SUBSYSTEM_PLD][PLD_PIC_CMD] = resolution / chunckSize;
        numDataPoints[SUBSYSTEM_PLD][PLD_PIC_CMD] = 0;
}

bool FileHandler::InitEpoch()
{
        fstream file;
        char fileName[] = "epoch.txt";
        if (true == TakeLock(MAX_BLOCK_TIME))
        {
                file.open(fileName, ios::out);
                if (!file.is_open())
                {
                        this->GiveLock();
                        return false;
                }

                char buffer[1];
                buffer[0] = 94;
                file.write(buffer, 1);
                if (file.bad())
                {
                        file.close();
                        this->GiveLock();
                        return false;
                }
                file.close();
                if (file.is_open())
                {
                        this->GiveLock();
                        return false;
                }
                this->GiveLock();
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
                if (!file.is_open())
                {
                        this->GiveLock();
                        return 0;
                }

                epochSize = (uint16) this->fileSize(file);
                file.close();
                if (file.is_open())
                {
                        this->GiveLock();
                        return 0;
                }
                this->GiveLock();
        }
        return epochSize;
}
/*
 bool FileHandler::KermitLock(void)
 {
 DEBUG_COUT("                           KermitLock: Called")
 if (true == TakeLock(MAX_BLOCK_TIME))
 {
 DEBUG_COUT("                           KermitLock: success")
 return true;
 }
 DEBUG_COUT("                           KermitLock: failed")
 return false;
 }

 void FileHandler::KermitUnlock(void)
 {
 DEBUG_COUT("                           KermitUnlock: Called")
 GiveLock( );
 }
*/

int FileHandler::fileSize(fstream & file)
{
        file.seekg(0, file.end);
        int length = file.tellg();
        file.seekg(0, file.beg);
        return length;
}

uint32 FileHandler::fileSize(char * fileName)
{
        uint32 rv = 0;
        //open file for read only
        fstream file;
        if (true == TakeLock(MAX_BLOCK_TIME))
        {
                file.open(fileName, ios::in);
                if (!file.is_open())
                {
                        // error: failed to open the file
                        this->GiveLock();
                        return -1;
                }
                //length of the file
                rv = (uint32) this->fileSize(file);
                file.close();
                if (file.is_open())
                {
                        // error: couldn't close the file
                        this->GiveLock();
                        return -2;
                }
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
		uint32 bufferSize;
		fstream file;
		uint8 * buffer;
		if (true == this->TakeLock(MAX_BLOCK_TIME))
				{
						file.open(fileName, ios::in);
						if (!file.is_open())
						{
								// error: failed to open the file
								crc = NULL;
								this->GiveLock();
								return crc;
						}
						bufferSize = (uint32) this->fileSize(file);
						buffer = new uint8[bufferSize];
						file.read((char *) buffer, bufferSize);

						//Calculate CRC
						for(uint8 i=0; i<bufferSize; i++){
							crc = generateCrc(buffer +  i,1,(unsigned int *) crcTable, (unsigned int)~crc);
						}

						if (file.bad())
						{
								file.close();
								delete buffer;
								buffer = NULL;
								this->GiveLock();
								return crc;
						}

						delete buffer;
						file.close();
						if (file.is_open())
						{
								// error: couldn't close the file
								this->GiveLock();
						}
						this->GiveLock();
				}
		return crc;
}

