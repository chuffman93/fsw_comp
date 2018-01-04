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

struct InterfaceOperation {
	InterfaceOperation(FileIOType type, std::string filePath, std::vector<uint8_t>& data);
	FileIOType type;
	std::string filePath;
	std::vector<uint8_t>& data;
};

class FileInterface {
public:
	virtual InterfaceOperation getOperation() = 0;
	virtual ~FileInterface() = 0;
};

class FileManager {
public:
	FileManager();
	~FileManager();
	void spinInterfaces();
	void registerInterface(FileInterface* interface);
	void handleFileDeletion();
private:
	void readFromFile(std::string filePath, std::vector<uint8_t>& buffer);
	void writeToFile(std::string filePath, std::vector<uint8_t>& buffer);
	void deleteFile(std::string filePath);

	LogTags tags;
	Lock lock;
	std::vector<FileInterface*> InterfaceList;
};

#endif /* INCLUDE_CORE_FILEMANAGER_H_ */
