#include "subsystem/SubsystemBase.h"


void HealthFileStruct::recordBytes(std::vector<uint8_t> message){
	fileSize += message.size();
	if (fileSize >= MAX_FILE_SIZE){
		currentFile = FileManager::createFileName(basePath);
		fileSize = message.size();
	}
	FileManager::writeToFile(currentFile, message);
}
