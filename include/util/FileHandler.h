/*! \file FileHandler.h
 *  \brief Header File for the FileHandler Class.
 *
 *  This class provides the functions to create, append, and close data files for writing to the SD card
 *  For both data files and error files
 *
 */

#ifndef _FILEHANDLER_H
#define _FILEHANDLER_H

#include <iostream>
#include <fstream>
#include <string>

#include "core/StdTypes.h"
#include "core/Singleton.h"
#include "core/Factory.h"
#include "core/MultiDataMessage.h"

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus
//#include "FreeRTOS/FreeRTOS.h"
//#include "FreeRTOS/queue.h"
//#include "FreeRTOS/semphr.h"
//#include "FreeRTOS/task.h"
#ifdef __cplusplus
}
#endif // __cplusplus

using namespace std;

/*! \brief Maximum number of opcodes per subsystem */
const size_t MAX_OPCODES = 256;

const size_t MAXFILESIZE = 500;

/*! \brief Maximum number of data points per log file */
const size_t MAX_DATA_POINTS = 1000;

/*! \brief Maximum number of data points per error log file */
const size_t MAX_ERROR_DATA_POINTS = 100;

const uint8 epochNum = 0;

class FileHandler: public Phoenix::Core::Singleton
{
        /*! \brief Declare Factory a friend class
         *
         *      This allows factory to call FileHandler's private constructor
         */
        friend class Phoenix::Core::Factory;
public:

        /*! \brief Append a buffer to the Error or Mode Log
         *
         * Adds a timestamp (week and seconds) and two bytes of
         * data to either the Error or Mode Log. Creates the log
         * and appends it to tru.dat if it does not already exist
         *
         * \param logType which log to append the buffer to
         * \param dataOne first byte to be appended
         * (after week and seconds)
         * \param dataTwo second byte to be appended
         * \return true if the operation was successful
         * and false otherwise
         *
         */
        bool LogAppend(FileHandlerIDEnum logType, MessageCodeType dataOne, MessageCodeType dataTwo);

        /*! \brief Adds the buffer to the file.
         *
         *      Looks up the most recent file based on the
         *      subsystem and opcode. If the file is full create
         *      a new file. Appends the message to the file based
         *      on the length provided.
         *
         *  \param subsystem Subsystem to find the file to append to.
         *  \param opCode Opcode to find the file to append to.
         *  \param message Message to append.
         *  \param length Length of the message.
         *  \return true if the operation was successful and
         *  false otherwise.
         */
        bool Append(FileHandlerIDEnum subsystem, MessageCodeType opCode,
                        const Phoenix::Core::MultiDataMessage & message);

        /*! \brief Deletes the file from the SD card.
         *
         *      Deletes the file given by the fileName.
         *
         *  \param fileName Name of the file to delete.
         *  \return true if the operation was successful and
         *  false otherwise.
         */
        bool DeleteFile(const char * fileName);

        /*! \brief Reads the file from the SD card.
         *
         *      Opens the file based on the file name.
         *      Reads the entire file into a buffer, sets
         *      the buffer size based on the size of the read.
         *
         *  \param fileName Name of the file to read.
         *  \param bufferSize Location to store the size of the
         *      buffer read.
         *  \return If the read was successful returns a pointer
         *  to the information read, otherwise returns NULL.
         */
        uint8 * ReadFile(const char * fileName, size_t * bufferSize);

        /*! \brief Checks file's size on the SD card.
         *
         *      Opens the file based on the file name.
         *      Checks that the fd is valid.
         *      Reads the size of the file and returns it.
         *
         *  \param fileName Name of the file to check.
         *  \return the size of the file, -1 if it doesn't
         *  exist.
         */
        uint32 FileOpens(const char * fileName);

        /*! \brief Checks if the file exists on the SD card.
         *
         *      Opens the file based on the file name.
         *      Checks that the fd is valid.
         *               *
         *  \param fileName Name of the file to check.
         *  \return true if the file exists; false otherwise
         */
        bool FileExists(const string fileName);

        /*! \brief Searches tru.dat for filename
         *
         * Opens the tru.dat file and checks each
         * line to see if it is equal to the file name
         *
         * \param fileName Name of the file to look for
         *
         */
        bool FileExistsInTruDat(string fileToFind);

        /*! \brief Writes the buffer to the file.
         *
         *      Opens the file based on the file name.
         *      Checks that the fd is valid.
         *      Writes the number of bytes specified
         *      into the file and returns file size.
         *
         *  \param fileName Name of the file to delete.
         *      \param buffer buffer to write into the file.
         *      \param numBytes number of bytes to write.
         *  \return the size of the file, -1 if it doesn't
         *  exist.
         */
        uint32 FileWrite(const char * fileName, char * buffer, size_t numBytes);

        void FileSizePLDPicture(uint32 resolution, uint32 chunckSize);

        // Epoch information
        bool InitEpoch(void);
        uint16 GetEpoch(void);

        /**************************WARNING****************************
         *                                                                                                                       *
         *      The following functions are to be used by eKermit only!  *
         *      Using these functions outside of the eKermit context     *
         *  may result in the permanent locking of the SD card!      *
         *                                                                                                               *
         **************************WARNING***************************/

        bool KermitLock(void);
        void KermitUnlock(void);

        int fileSize(fstream & file);

        uint32 fileSize(const char * fileName);

        void zipFiles(const char * path, const char * zipName);

        void unzipFiles(const char * path, const char * zipName);

        void unzipFile(const char * path, const char * file, const char * zipName);

        unsigned int folderSize(const char * path);

        void makeCrcTable(uint32 crcTable[]);

        unsigned int generateCrc(uint8 *p, size_t n, unsigned int crcTable[], unsigned int crc);

        uint32 crcCheck(const char * filename);

private:
        /*! \brief Initialize the File Handler Class
         *
         *  Initializes the operating system constructs needed for
         *  File Handler to work properly.
         */
        static void Initialize(void);

        /*! \brief Static Destructor for File Handler
         *
         *  Frees all internal memory use, frees all operating system
         *  objects used, and deletes the singleton instance pointer.
         */
#ifdef WIN32
        static void Destroy(void);
#endif

        /*! \brief Checks if File Handler Class is initialized
         *
         *  \return true if the initialization was successful and
         *  false otherwise.
         */
        bool IsFullyInitialized(void);

        /*! \brief Creates the file size reference table create function,
         *      based on the maximum number of opcodes per subsystem, and the
         *      maximum number of data points per opcode and error opcode.
         */
        void FileSizeReferenceCreate(void);

        //Comment on how it works
        uint8_t FetchFileName(FileHandlerIDEnum subsystem, MessageCodeType opCode, string file, int week);

        // Subsystem file designator reference tables
        // Allows quick reference to most recent file.
        uint32 secRef[SYSTEM_MAX][MAX_OPCODES];
        uint32 weekRef[SYSTEM_MAX][MAX_OPCODES];

        // Subsystem file data size reference tables
        uint32 numDataPoints[SYSTEM_MAX][MAX_OPCODES];
        uint32 numDataPointsMax[SYSTEM_MAX][MAX_OPCODES];

        /*! \brief Constructor for File Handler */
        FileHandler(void);

        /*! \brief Deconstructor for File Handler */
        ~FileHandler(void);

        /*! \brief Assignment operator for File Handler */
        FileHandler & operator=(const FileHandler & source);
        const static char * errLog;
        const static char * modeLog;
};

#endif  //_FILEHANDLER_H
