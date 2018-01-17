/*
 * FileManager.cpp

 *
 *  Created on: Dec 22, 2017
 *      Author: alex and robo
 *      Edited on: January 9th 2018 by Alex
 */

#include "core/FileManager.h"
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <sstream>
#include <iterator>
#include <fstream>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

using namespace std;

FileManager::FileManager(){
	tags += LogTag("Name", "FileManager");
}

FileManager::~FileManager(){}



std::vector<uint8_t> FileManager::readFromFile(std::string filePath){

	std::vector<uint8_t> buffer;

	int fileID = open(filePath.c_str(),O_RDONLY);
	if (fileID == -1){
		Logger::Stream(LEVEL_ERROR,tags) << "Unable to open file for reading: " << filePath;
		return buffer;
	}
	uint8_t byte;
	while(read(fileID, &byte, 1) == 1){
		buffer.push_back(byte);
	}
	close(fileID);
	return buffer;


}

void FileManager::writeToFile(std::string filePath, std::vector<uint8_t> &buffer){
	if (filePath == ""){
		Logger::Stream(LEVEL_ERROR,tags) << "Unable to write to file, empty file path";
		buffer.clear();
		return;
	}
	else if (buffer.size() == 0){
		Logger::Stream(LEVEL_ERROR,tags) << "Unable to write to file, empty buffer: " << filePath;
		return;
	}
	else{
		ofstream f(filePath.c_str(),ofstream::out | ofstream::binary);

		for(std::vector<uint8_t>::const_iterator i = buffer.begin(); i != buffer.end(); ++i){
			f << *i;
		}
		f.close();
	}

}

void FileManager::appendToFile(std::string filePath, std::vector<uint8_t>& buffer){
	if (filePath == ""){
		Logger::Stream(LEVEL_ERROR,tags) << "Unable to append to file, empty file path";
		buffer.clear();
		return;
	}
	if (buffer.size() == 0){
		Logger::Stream(LEVEL_ERROR,tags) << "Unable to append to file, empty buffer: " << filePath;
		return;
	}
	int fileID = open(filePath.c_str(),O_APPEND);
	if (fileID == -1){
		Logger::Stream(LEVEL_ERROR,tags) << "Unable to append to file, file path does not exist: " << filePath;
		return;
	}
	write(fileID, &buffer, buffer.size());
	close(fileID);
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
	if (access(filePath.c_str(), F_OK) == 0){
		rename(filePath.c_str(),newfilePath.c_str());
	}
}

std::string FileManager::createFileName(std::string basePath){

	//get number of reboots
	std::ifstream rebootFile(REBOOT_FILE);
	int intCount;
	rebootFile >> intCount;
	rebootFile.close();

	//convert boot count to string
	stringstream strCount;
	strCount << intCount;
	std:: string RebootCount = strCount.str();

	//get current time
	uint32_t currentTime = getCurrentTime();
	stringstream ss;
	ss << currentTime;
	std::string time = ss.str();

	std::string filePath = basePath + "_" + RebootCount + "_" + time;


	return filePath;

}

void FileManager::copyFile(std::string filePath, std::string newfilePath){
	if (access(filePath.c_str(), F_OK) == 0){
		std::string command = "cp " + filePath + " " + newfilePath;
		system(command.c_str());
	}
}

void FileManager::updateRebootCount(){
	int RebootCount;
	if (checkExistance(REBOOT_FILE)){
		//read boot number from file
		std::ifstream rebootFile(REBOOT_FILE);
		rebootFile >> RebootCount;
		rebootFile.close();

		//write boot count +1 to file
		std::ofstream out(REBOOT_FILE);
		out << ++RebootCount;
		out.close();
	}else {
		//this is the first time the boot count has been incremented, write initial boot count to file
		RebootCount = 1;
		std::ofstream out(REBOOT_FILE);
		out << RebootCount;
		out.close();
	}
	Logger::Stream(LEVEL_INFO,tags) << "Updated Reboot Count to " << RebootCount;
}

void FileManager::parseIEF(){

	GroundCommunication ground;

	char *line = NULL;
	size_t len;
	ssize_t bytesRead;

	char * type;
	uint8_t lineNumber;

	FILE * file = fopen(IEF_PATH, "r");

	while ((bytesRead = getline(&line, &len, file)) != -1){
		lineNumber++;
		type = strtok(line,",");
		if (type == NULL){
			continue;
		}else if (strcmp(type,"CMD")){
			ground.parseCommandRequest(line);
		}else if (strcmp(type,"DWL")){
			ground.parseDownlinkRequest(line);
		}else if (strcmp(type,"DLT")){
			ground.parseDeletionRequest(line);
		}else if (strcmp(type, "DFL")){
			ground.parseFileListRequest(line);
		}
	}

	FileManager::deleteFile(IEF_PATH);
}

void FileManager::parsePPE(){
	GroundCommunication ground;

	char * line;
	size_t len;
	ssize_t bytesRead;

	char * type;
	uint8_t lineNumber;

	FILE * file = fopen(PPE_PATH, "r");

	if (file == NULL){
		Logger::Stream(LEVEL_INFO, tags) << "No PPE File";
		return;
	}

	while ((bytesRead = getline(&line, &len, file)) != -1){
		lineNumber++;
		type = strtok(line,",");
		if (type == NULL){
			continue;
		}else if (strcmp(type,"CMD")){
			ground.parseCommandRequest(line);
		}else if (strcmp(type,"DLT")){
			ground.parseDeletionRequest(line);
		}
	}
	FileManager::deleteFile(PPE_PATH);
}




