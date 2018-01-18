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

FileManager::FileManager(){}

FileManager::~FileManager(){}



std::vector<uint8_t> FileManager::readFromFile(std::string filePath){
	Lock lock;
	LockGuard lock_guard(lock);

	LogTags tags;
	tags += LogTag("Name", "FileManager");

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
	Lock lock;
	LockGuard lock_guard(lock);
	LogTags tags;
	tags += LogTag("Name", "FileManager");
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
	Lock lock;
	LockGuard lock_guard(lock);
	LogTags tags;
	tags += LogTag("Name", "FileManager");
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
	// Test again

	Lock lock;
	LockGuard lock_guard(lock);


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
	Lock lock;
	LockGuard lock_guard(lock);
	LogTags tags;
	tags += LogTag("Name", "FileManager");
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
	LogTags tags;
	tags += LogTag("Name", "FileManager");

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


vector<std::string> FileManager::packageFiles(std::string dest, std::string R){
	// do regex delete too!!!!
	// R containing
	// RB before
	// RA after

	Lock lock;
	LockGuard lock_guard(lock);
	LogTags tags;
	tags += LogTag("Name", "FileManager");

	// TODO: size checks and error handling

	FILE *fd;
	char sh_cmd[256];
	vector<std::string> Files;
	// manage fullfilepath + regex manipulation
	// if one is created .tar = filePath + regex.tar


	if(R == "R"){
		int i = dest.length()-1;
		std::string regex = "";
		while(dest[i]!='/'){
			regex = dest[i]+regex;
			i--;
		}
		const std::string ext(regex);
		if(dest != ext && dest.size() > ext.size() && (dest.substr((dest.size() - ext.size())) == regex)){
			dest = dest.substr(0,dest.size()-ext.size());
		}
		std::string newDest = dest + regex + ".tar";
		regex = regex+"_";
		sprintf(sh_cmd, "tar -czf %s -C %s `ls -lr %s | grep ^- | awk '{print $9}' | grep \"%s\" | head -%d`>/dev/null 2>&1", (char*)newDest.c_str(), (char*)dest.c_str(), (char*)dest.c_str(), (char*)regex.c_str(), 50);
		Files.push_back(newDest);
		if(!(fd = popen(sh_cmd, "r"))){
			cout << " NEED LOGGER STATEMENTS FOR FAILURE TO CREATE TAR " << endl;
		}
		if(pclose(fd) == -1){
			cout << " NEED LOGGER STATEMETS FOR FAILURE TO CLOSE PIPE WHEN CREATING TAR " << endl;
		}


	}else if(R == "RB"){
		int i = dest.length()-1;
		std::string regex = "";
		std::string num = "";
		while(dest[i]!='_'){
			num = dest[i]+num;
			i--;
		}
		int EN = atoi(num.c_str());
		std::string end = "_"+num;
		i = dest.length()-1;
		while(dest[i]!='/'){
			regex = dest[i]+regex;
			i--;
		}
		std::string nRegex = regex.substr(0,regex.size() - end.size());
		std::string pastReg[EN];
		for(int x = 0; x < EN ; x++){
			std::ostringstream oss;
			oss << x;
			pastReg[x] = nRegex+"_"+oss.str();
		}
		const std::string ext(regex);
		if(dest != ext && dest.size() > ext.size() && (dest.substr((dest.size() - ext.size())) == regex)){
			dest = dest.substr(0,dest.size()-ext.size());
		}
		for(int y = 0; y < EN; y++){
			std::string newDest = dest + pastReg[y] + ".tar";
			pastReg[y] = pastReg[y] + "_";
			sprintf(sh_cmd, "tar -czf %s -C %s `ls -lr %s | grep ^- | awk '{print $9}' | grep \"%s\" | head -%d` >/dev/null 2>&1", (char*)newDest.c_str(), (char*)dest.c_str(), (char*)dest.c_str(), (char*)pastReg[y].c_str(), 50);

			if(!(fd = popen(sh_cmd, "r"))){
				cout << " NEED LOGGER STATEMENTS FOR FAILURE TO CREATE TAR " << endl;
				pclose(fd);
				break;
			}
			if(pclose(fd) == -1){
				cout << " NEED LOGGER STATEMETS FOR FAILURE TO CLOSE PIPE WHEN CREATING TAR " << endl;
				break;
			}
			Files.push_back(newDest);
		}
	}else if(R == "RA"){
		int i = dest.length()-1;
		std::string regex = "";
		std::string num = "";
		while(dest[i]!='_'){
			num = dest[i]+num;
			i--;
		}
		int EN = atoi(num.c_str());
		std::string end = "_"+num;
		i = dest.length()-1;
		while(dest[i]!='/'){
			regex = dest[i]+regex;
			i--;
		}
		std::string nRegex = regex.substr(0,regex.size() - end.size());


		std::ifstream rebootFile(REBOOT_FILE);
		int intCount;
		rebootFile >> intCount;
		rebootFile.close();
		int itr = intCount - EN;
		std::string pastReg[itr+1];
		for(int x = 0; x < itr+1; x++){
			std::ostringstream oss;
			oss << EN;
			pastReg[x] = nRegex+"_"+oss.str();
			EN++;
		}
		const std::string ext(regex);
		if(dest != ext && dest.size() > ext.size() && (dest.substr((dest.size() - ext.size())) == regex)){
			dest = dest.substr(0,dest.size()-ext.size());
		}
		for(int y = 0; y < itr+1; y++){
			std::string newDest = dest + pastReg[y] + ".tar";
			pastReg[y] = pastReg[y] + "_";
			sprintf(sh_cmd, "tar -czf %s -C %s `ls -lr %s | grep ^- | awk '{print $9}' | grep \"%s\" | head -%d` >/dev/null 2>&1", (char*)newDest.c_str(), (char*)dest.c_str(), (char*)dest.c_str(), (char*)pastReg[y].c_str(), 50);

			if(!(fd = popen(sh_cmd, "r"))){
				cout << " NEED LOGGER STATEMENTS FOR FAILURE TO CREATE TAR " << endl;
				pclose(fd);
				break;
			}
			if(pclose(fd) == -1){
				cout << " NEED LOGGER STATEMETS FOR FAILURE TO CLOSE PIPE WHEN CREATING TAR " << endl;
				break;
			}
			Files.push_back(newDest);
		}
	}
	// handle multiple tar balls
	return Files;
}



void FileManager::getFilesList(std::string dir){
	Lock lock;
	LockGuard lock_guard(lock);
	LogTags tags;
	tags += LogTag("Name", "FileManager");
	// TODO: Error handling, decide on more cases

	DIR *dp;
	struct dirent *entry;
	int count;
	dp = opendir(dir.c_str());
	if(dp == NULL){
		// add logger message
		return;
	}

	count = 0;
	// add full path
	FILE * dwlkDFL = fopen(DFL_PATH,"a+");
	while((entry = readdir(dp))!= NULL){
		fwrite(entry->d_name,strlen(entry->d_name),1,dwlkDFL);
		fwrite(", ",strlen(", "),1,dwlkDFL);
	}
	fwrite("\n",strlen("\n"),1,dwlkDFL);
	closedir(dp);
	fclose(dwlkDFL);
}


int FileManager::regexDelete(std::string dest,std::string R){
	Lock lock;
	LockGuard lock_guard(lock);

	LogTags tags;
	tags += LogTag("Name", "FileManager");
	FILE *fd;

	char sh_cmd[265];

	// check folder deletion

	if(R == "R"){
			int i = dest.length()-1;
			std::string regex = "";
			while(dest[i]!='/'){
				regex = dest[i]+regex;
				i--;
			}
			const std::string ext(regex);
			if(dest != ext && dest.size() > ext.size() && (dest.substr((dest.size() - ext.size())) == regex)){
				dest = dest.substr(0,dest.size()-ext.size());
			}
			regex = regex + "_";
			sprintf(sh_cmd, "rm -r `ls -l %s | grep ^- | awk '{print \"%s\" \"/\" $9}' | grep \"%s\"`", (char*)dest.c_str(), (char*)dest.c_str(), (char*)regex.c_str());
			if(!(fd = popen(sh_cmd, "r"))){
				cout << " NEED LOGGER STATEMENTS FOR FAILURE TO CREATE TAR " << endl;
				return -3;
			}
			if(pclose(fd) == -1){
				cout << " NEED LOGGER STATEMETS FOR FAILURE TO CLOSE PIPE WHEN CREATING TAR " << endl;
				return -1;
			}


		}else if(R == "RB"){
			int i = dest.length()-1;
			std::string regex = "";
			std::string num = "";
			while(dest[i]!='_'){
				num = dest[i]+num;
				i--;
			}
			int EN = atoi(num.c_str());
			std::string end = "_"+num;
			i = dest.length()-1;
			while(dest[i]!='/'){
				regex = dest[i]+regex;
				i--;
			}
			std::string nRegex = regex.substr(0,regex.size() - end.size());
			std::string pastReg[EN];
			for(int x = 0; x < EN ; x++){
				std::ostringstream oss;
				oss << x;
				pastReg[x] = nRegex+"_"+oss.str()+"_";
			}
			const std::string ext(regex);
			if(dest != ext && dest.size() > ext.size() && (dest.substr((dest.size() - ext.size())) == regex)){
				dest = dest.substr(0,dest.size()-ext.size());
			}
			for(int y = 0; y < EN; y++){
				sprintf(sh_cmd, "rm -r `ls -l %s | grep ^- | awk '{print \"%s\" \"/\" $9}' | grep \"%s\"`", (char*)dest.c_str(), (char*)dest.c_str(), (char*)pastReg[y].c_str());;

				if(!(fd = popen(sh_cmd, "r"))){
					cout << " NEED LOGGER STATEMENTS FOR FAILURE TO CREATE TAR " << endl;
					pclose(fd);
					return -3;
					break;
				}
				if(pclose(fd) == -1){
					cout << " NEED LOGGER STATEMETS FOR FAILURE TO CLOSE PIPE WHEN CREATING TAR " << endl;
					return -1;
					break;
				}
			}
		}else if(R == "RA"){
			int i = dest.length()-1;
			std::string regex = "";
			std::string num = "";
			while(dest[i]!='_'){
				num = dest[i]+num;
				i--;
			}
			int EN = atoi(num.c_str());
			std::string end = "_"+num;
			i = dest.length()-1;
			while(dest[i]!='/'){
				regex = dest[i]+regex;
				i--;
			}
			std::string nRegex = regex.substr(0,regex.size() - end.size());


			std::ifstream rebootFile(REBOOT_FILE);
			int intCount;
			rebootFile >> intCount;
			rebootFile.close();
			int itr = intCount - EN;
			std::string pastReg[itr+1];
			for(int x = 0; x < itr+1; x++){
				std::ostringstream oss;
				oss << EN;
				pastReg[x] = nRegex+"_"+oss.str()+"_";
				EN++;
			}
			const std::string ext(regex);
			if(dest != ext && dest.size() > ext.size() && (dest.substr((dest.size() - ext.size())) == regex)){
				dest = dest.substr(0,dest.size()-ext.size());
			}
			for(int y = 0; y < itr+1; y++){
				sprintf(sh_cmd, "rm -r `ls -l %s | grep ^- | awk '{print \"%s\" \"/\" $9}' | grep \"%s\"`", (char*)dest.c_str(), (char*)dest.c_str(), (char*)pastReg[y].c_str());;

				if(!(fd = popen(sh_cmd, "r"))){
					cout << " NEED LOGGER STATEMENTS FOR FAILURE TO CREATE TAR " << endl;
					pclose(fd);
					return -3;
					break;
				}
				if(pclose(fd) == -1){
					cout << " NEED LOGGER STATEMETS FOR FAILURE TO CLOSE PIPE WHEN CREATING TAR " << endl;
					return -1;
					break;
				}
			}
		}
	//TODO: Error handling
	return 0;
}



