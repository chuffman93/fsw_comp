/*
 * FileInterfaces.cpp
 *
 *  Created on: Jan 2, 2018
 *      Author: alpa3119
 */

#include "core/FileInterfaces.h"

//-----------------------------HEALTH AND STATUS ----------------------------

HealthStatusInterface::HealthStatusInterface(std::string filePath)
: filePath(filePath), fileName("")
{}

HealthStatusInterface::~HealthStatusInterface(){}

void HealthStatusInterface::pushData(std::vector<uint8_t> buffer){
	HealthStatusQueue.push(buffer);
}

InterfaceOperation HealthStatusInterface::getOperation(){
	std::vector<uint8_t> buff;
	if (fileName == ""){
		fileName = createFileName();
	}

	std::string fileLocation = filePath + fileName;

	std::vector<uint8_t> tempBuff = HealthStatusQueue.front();
	while (buff.size() + tempBuff.size() + 4 < MAX_FILE_SIZE){
		//TODO: prepend time since last boot up, 4 byte number containing number of seconds
		HealthStatusQueue.pop();
		buff.insert(buff.end(), tempBuff.begin(), tempBuff.end());
		tempBuff.clear();
		tempBuff = HealthStatusQueue.front();
	}

	InterfaceOperation HealthOp(FIO_WRITE, fileLocation, buff);

	return HealthOp;
}


std::string HealthStatusInterface:: createFileName(){
	//TODO: create file name with number of resets and time since boot up
	return "TempFileName";
}
/*

//-------------------------------CONFIG----------------------------------------

ConfigInterface::ConfigInterface(std::string filePath)
: filePath(filePath)
{}

ConfigInterface::~ConfigInterface(){}

std::vector<uint8_t> ConfigInterface::pullData(){

}

InterfaceOperation ConfigInterface::getOperation(){
	std::vector<uint8_t> buff;
	InterfaceOperation ConfigOp(FIO_READ, filePath, buff);
	return ConfigOp;
}



*/

