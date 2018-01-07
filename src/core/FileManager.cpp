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

FileInterface::~FileInterface(){}


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
	if (filePath == ""){
		buffer.clear();
		std::cout << "test no file path" << endl;
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
	if (access(filePath.c_str(), F_OK)){
		return true;
	}
}


