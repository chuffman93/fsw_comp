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
		buffer.clear();
		return;
	}
	else if (buffer.size() == 0){
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
		buffer.clear();
		return;
	}
	if (buffer.size() == 0){
		return;
	}
	int fileID = open(filePath.c_str(),O_APPEND);
	if (fileID == -1){
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
	rename(filePath.c_str(),newfilePath.c_str());
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


}

int FileManager::packageFiles(const char* dest, const char* filePath, const char* regex){

	// TODO: size checks and error handling

	FILE *fd;
	char sh_cmd[256];
	sprintf(sh_cmd, "tar -czf %s -C %s `ls -lr %s | grep ^- | awk '{print $9}' | grep \"%s\" | head -%d`", dest, filePath, filePath, regex, 10);
	if(!(fd = popen(sh_cmd, "r"))){
		return -3;
	}

	if(pclose(fd) == -1){
		return -1;
	}
	return 0;
}
void FileManager::getFilesList(std::string dir){
	DIR *dp;
	struct dirent *entry;
	int count;
	dp = opendir(dir.c_str());
	if(dp == NULL){
		return;
	}

	count = 0;
	FILE * dwlkDFL = fopen(DFL_PATH,"a+");
	while((entry = readdir(dp))!= NULL){
		fwrite(", ",strlen(", "),1,dwlkDFL);
		fwrite(entry->d_name,strlen(entry->d_name),1,dwlkDFL);
	}
	closedir(dp);
	fclose(dwlkDFL);
}

