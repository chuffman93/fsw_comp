/*
 * FileManager.cpp

 *
 *  Created on: Dec 22, 2017
 *      Author: alex and robo and NOT adam :)
 */

#include "core/FileManager.h"
#include <stdio.h>

FileManager::FileManager(){
	tags += LogTag("Name", "FileManager");
}

FileManager::~FileManager(){}

InterfaceOperation::InterfaceOperation(FileIOType type, std::string filePath, std::vector<uint8_t>& data)
: type(type), filePath(filePath), data(data)
{}

FileInterface::~FileInterface(){}

void FileManager::spinInterfaces(){
	for (std::vector<FileInterface*>::iterator i = InterfaceList.begin(); i != InterfaceList.end(); i++){
		InterfaceOperation op = (*i)->getOperation();
		switch (op.type){
		case FIO_WRITE:
			writeToFile(op.filePath, op.data);
			break;
		case FIO_READ:
			readFromFile(op.filePath, op.data);
			break;
		case FIO_READDELETE:
			readFromFile(op.filePath, op.data);
			deleteFile(op.filePath);
			break;
		case FIO_DELETE:
			deleteFile(op.filePath);
			break;
		case FIO_EMPTY:
			break;
		}
	}

}

void FileManager::registerInterface(FileInterface* interface){
	InterfaceList.push_back(interface);
}

void FileManager::handleFileDeletion(){

}

void FileManager::readFromFile(std::string filePath, std::vector<uint8_t>& buffer){
	FILE * fileID = fopen(filePath.c_str(), "r");

	if (fileID == NULL){
		buffer.clear();
		return;
	}

	//get size of file
	fseek(fileID,0,SEEK_END);
	size_t size = ftell(fileID);
	fseek(fileID, 0, SEEK_SET);

	//resize buffer
	buffer.clear();
	buffer.resize(size);

	//read file and place data into buffer
	fread(&buffer[0],1,size,fileID);

	//close file
	fclose(fileID);


}

void FileManager::writeToFile(std::string filePath, std::vector<uint8_t>& buffer){

}

void FileManager::deleteFile(std::string filePath){

}

