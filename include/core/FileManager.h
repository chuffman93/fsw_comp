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

#include <stdint.h>
#include <vector>

enum FileIOType {
	FIO_WRITE,
	FIO_READ,
	FIO_READDELETE,
	FIO_DELETE,
	FIO_EMPTY,
};

class FileManager {
public:
	void readFromFile(std::string filePath, std::vector<uint8_t>& buffer);
	void writeToFile(std::string filePath, std::vector<uint8_t>& buffer);
	void deleteFile(std::string filePath);
	bool checkExistance(std::string filePath);
	void renameFile(std::string filePath, std::string newfilePath);

	LogTags tags;
	Lock lock;
};

#endif /* INCLUDE_CORE_FILEMANAGER_H_ */
