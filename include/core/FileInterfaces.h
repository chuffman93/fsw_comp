/*
 * FileInterfaces.h
 *
 *  Created on: Jan 2, 2018
 *      Author: alpa3119
 */

#ifndef INCLUDE_FILEINTERFACES_H_
#define INCLUDE_FILEINTERFACES_H_

#include <queue>
#include "FileManager.h"
#include "core/Lock.h"

class HealthStatusInterface {
public:
	HealthStatusInterface(std::string filePath);
	~HealthStatusInterface();
	void pushData(std::vector<uint8_t> buffer);
	InterfaceOperation getOperation();

private:
	std::string createFileName();

	std::string filePath;
	std::string fileName;
	std::queue<std::vector<uint8_t> > HealthStatusQueue;
	Lock lock;
};


struct ConfigData {
	std::vector<uint8_t> data;
	std::string filePath;
};

class ConfigInterface{
public:
	ConfigInterface();
	~ConfigInterface();
	std::vector<uint8_t> requestData();
	InterfaceOperation getOperation();

private:
	std::string filePath;
	std::vector<uint8_t> data;
	std::queue<ConfigData> ConfigQueue;
	Lock lock;
};


#endif /* INCLUDE_FILEINTERFACES_H_ */
