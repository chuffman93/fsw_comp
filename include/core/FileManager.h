/*
 * FileManager.h
 *
 *  Created on: Dec 22, 2017
 *      Author: adam
 */

#ifndef INCLUDE_CORE_FILEMANAGER_H_
#define INCLUDE_CORE_FILEMANAGER_H_

#include "FileSystem.h"
#include "util/Logger.h"
#include "core/Lock.h"
#include "util/TimeKeeper.h"
#include "interfaces/ExternalProcess.h"



#include <vector>
#include <unistd.h>
#include <dirent.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define CONFIG_FMG_SIZE 6

class FileManager {
public:
	FileManager();
	~FileManager();

	static std::vector<uint8_t> readFromFile(std::string filePath);
	//for files that always need to be rewritten to (aka reboot file) or new files
	static void writeToFile(std::string filePath, std::vector<uint8_t>& buffer);
	//for file that need to be appended to or new files
	static void appendToFile(std::string filePath, std::vector<uint8_t>& buffer);

	static void writeToStringFile(std::string filePath, std::vector<std::string>& buffer);

	static void deleteFile(std::string filePath);
	static bool checkExistance(std::string filePath);
	static void moveFile(std::string filePath, std::string newfilePath);
	//appends reboot count and time since epoch to given base path
	static std::string createFileName(std::string basePath);

	static void copyFile(std::string filePath, std::string newfilePath);

	//updates reboot count, this will only be called once in main.cpp
	static int updateRebootCount();
	static int updateComPassCount();
	static int getComPassCount();


	static std::vector<std::string> packageFiles(std::string filePath,std::string R);
	static void generateFilesList(std::string dir);
	static int regexDelete(std::string filePath,std::string R);

	static std::vector<std::string> parseGroundFile(std::string filePath);
	static void writeLog(std::string tags,std::string message);
	static int GetReboot();

	static void handleConfig();
	static void updateConfig();

	static std::vector<std::string> splitFile(std::string filePath);

	static uint16_t MAX_FILE_SIZE;
	//static FileManager fm;

private:
	static Lock lock;
	static std::string logMessageFP;
	static int Reboot_num;
	static uint32_t MAX_DOWN_SIZE;
};

#endif /* INCLUDE_CORE_FILEMANAGER_H_ */
