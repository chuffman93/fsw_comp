/*
 * FileManager.cpp

 *
 *  Created on: Dec 22, 2017
 *      Author: alex and robo and NOT adam :)
 */

#include "core/FileManager.h"
#include <stdio.h>
#include <iostream>

using namespace std;

FileManager::FileManager(){
	tags += LogTag("Name", "FileManager");
}

FileManager::~FileManager(){}



std::vector<uint8_t> FileManager::readFromFile(std::string filePath){
	std::vector<uint8_t> buffer;
	FILE * fileID = fopen(filePath.c_str(), "r");
	if (fileID == NULL){
		return buffer;
	}

	//get size of file
	fseek(fileID,0,SEEK_END);
	size_t size = ftell(fileID);
	fseek(fileID, 0, SEEK_SET);

	//resize buffer
	buffer.resize(size);

	//read file and place data into buffer
	fread(&buffer[0],1,size,fileID);

	//close file
	fclose(fileID);

	return buffer;
}

void FileManager::writeToFile(std::string filePath, std::vector<uint8_t>& buffer){
	if (filePath == ""){
		buffer.clear();
		return;
	}
	if (buffer.size() == 0){
		return;
	}
	FILE * fileID = fopen(filePath.c_str(), "a");
	fwrite(&buffer, 1,buffer.size(), fileID);

	fclose(fileID);
}

void FileManager::deleteFile(std::string filePath){
	remove(filePath.c_str());

}

bool FileManager::checkExistance(std::string filePath){
	if (access(filePath.c_str(), F_OK) == 0){
		return true;
	}
	return false;
}

void FileManager::moveFile(std::string filePath, std::string newfilePath){
	rename(filePath.c_str(),newfilePath.c_str());
}


