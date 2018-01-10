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

class FileManager {
public:
	FileManager();
	~FileManager();

	std::vector<uint8_t> readFromFile(std::string filePath);
	void writeToFile(std::string filePath, std::vector<uint8_t>& buffer);
	void deleteFile(std::string filePath);
	bool checkExistance(std::string filePath);
	void moveFile(std::string filePath, std::string newfilePath);
	std::string createFileName(std::string basePath);
	void updateRebootCount();

	LogTags tags;
	Lock lock;

};

#endif /* INCLUDE_CORE_FILEMANAGER_H_ */
