/*
 * FileInterfaces.h
 *
 *  Created on: Jan 2, 2018
 *      Author: alpa3119
 */

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


