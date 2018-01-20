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


#include <stdint.h>
#include <vector>
#include <unistd.h>
#include <dirent.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

class FileManager {
public:
	FileManager();
	~FileManager();

	static std::vector<uint8_t> readFromFile(std::string filePath);
	//for files that always need to be rewritten to (aka reboot file) or new files
	static void writeToFile(std::string filePath, std::vector<uint8_t>& buffer);
	//for file that need to be appended to or new files
	static void appendToFile(std::string filePath, std::vector<uint8_t>& buffer);

	static void deleteFile(std::string filePath);
	static bool checkExistance(std::string filePath);
	static void moveFile(std::string filePath, std::string newfilePath);
	//appends reboot count and time since epoch to given base path
	static std::string createFileName(std::string basePath);

	static void copyFile(std::string filePath, std::string newfilePath);

	//updates reboot count, this will only be called once in main.cpp
	static void updateRebootCount();


	static int packageFiles(const char* dest, const char* filePath, const char* regex);
	static void getFilesList(std::string dir);
	static int regexDelete(const char* filePath, const char * regex);

	static std::vector<char*> parseGroundFile(std::string filePath);


	//static FileManager fm;

private:

};

#endif /* INCLUDE_CORE_FILEMANAGER_H_ */
