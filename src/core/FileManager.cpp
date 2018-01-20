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

Lock FileManager::lock;

FileManager::FileManager(){}

FileManager::~FileManager(){}


/*!
 * Reads a file byte by byte and returning a vector of those bytes
 * \param file path to the file that is to be read
 */
std::vector<uint8_t> FileManager::readFromFile(std::string filePath){


	LogTags tags;
	tags += LogTag("Name", "FileManager");

	std::vector<uint8_t> buffer;

	lock.lock();
	int fileID = open(filePath.c_str(),O_RDONLY);
	if (fileID == -1){
		Logger::Stream(LEVEL_ERROR,tags) << "Unable to open file for reading: " << filePath;
		lock.unlock();
		return buffer;
	}
	uint8_t byte;
	while(read(fileID, &byte, 1) == 1){
		buffer.push_back(byte);
	}
	close(fileID);
	lock.unlock();
	return buffer;


}

/*!
 * Writes to a file
 * \param file path to the file that is to be written to
 * \param vector of bytes that is to be written to a file
 */
void FileManager::writeToFile(std::string filePath, std::vector<uint8_t> &buffer){

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
		lock.lock();
		ofstream f(filePath.c_str(),ofstream::out | ofstream::binary);

		for(std::vector<uint8_t>::const_iterator i = buffer.begin(); i != buffer.end(); ++i){
			f << *i;
		}
		f.close();
		lock.unlock();
	}

}

/*!
 * Appends to a file
 * \param file path to file that is to be added to
 * \param vector of bytes that are to be added to a file
 */
void FileManager::appendToFile(std::string filePath, std::vector<uint8_t>& buffer){

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
	lock.lock();
	int fileID = open(filePath.c_str(),O_APPEND);
	if (fileID == -1){
		Logger::Stream(LEVEL_ERROR,tags) << "Unable to append to file, file path does not exist: " << filePath;
		lock.unlock();
		return;
	}
	write(fileID, &buffer, buffer.size());
	close(fileID);
	lock.unlock();
}

/*!
 * Deletes a file or directory
 * \param path to either a file or a directory
 */
void FileManager::deleteFile(std::string filePath){
	struct stat s;
	LogTags tags;
	tags += LogTag("Name", "FileManager");
	if (stat((char*)filePath.c_str(),&s) == 0){
		if( s.st_mode & S_IFDIR){
			ExternalProcess rm;
			char * sh_cmd[] = {(char*)"/bin/rm",(char*)"-r",(char*)filePath.c_str(),NULL};
			rm.launchProcess(sh_cmd);
		}
		else
			lock.lock();
			remove((char*)filePath.c_str());
			lock.unlock();
	}
	else
		Logger::Stream(LEVEL_WARN,tags) << "No file or directory to be deleted.";
		return;

}

/*!
 * Checks the existance of a file
 * \param file path to file to be checked
 */
bool FileManager::checkExistance(std::string filePath){
	if (access(filePath.c_str(), F_OK) == 0){
		return true;
	}
	return false;
}

/*!
 * Moves a file to a new path
 * \param file path to file that is to be moved
 * \param new file path that the file is being moved to
 */
void FileManager::moveFile(std::string filePath, std::string newfilePath){
	if (access(filePath.c_str(), F_OK) == 0){
		lock.lock();
		rename(filePath.c_str(),newfilePath.c_str());
		lock.unlock();
	}
}

/*!
 * Creates a new file name (full path) with the convention of ../nameOfFile_RebootCount_Time
 * \param base path including the Name of the file
 */
std::string FileManager::createFileName(std::string basePath){

	//get number of reboots
	// Test again

	lock.lock();
	std::ifstream rebootFile(REBOOT_FILE);
	int intCount;
	rebootFile >> intCount;
	rebootFile.close();
	lock.unlock();

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

/*!
 * Copies a file into a new directory
 * \param file path to the fle to be copied
 * \param file path to the desired location
 */
void FileManager::copyFile(std::string filePath, std::string newfilePath){
	if (access(filePath.c_str(), F_OK) == 0){
		std::string command = "cp " + filePath + " " + newfilePath;
		// TODO: get away from system command
		system(command.c_str());
	}
}

/*!
 * Creates or updates the reboot count file.
 */
void FileManager::updateRebootCount(){

	LogTags tags;
	tags += LogTag("Name", "FileManager");
	int RebootCount;

	if (checkExistance(REBOOT_FILE)){
		//read boot number from file
		lock.lock();
		std::ifstream rebootFile(REBOOT_FILE);
		rebootFile >> RebootCount;
		rebootFile.close();

		//write boot count +1 to file
		std::ofstream out(REBOOT_FILE);
		out << ++RebootCount;
		out.close();
		lock.unlock();
	}else {
		//this is the first time the boot count has been incremented, write initial boot count to file
		RebootCount = 1;
		lock.lock();
		std::ofstream out(REBOOT_FILE);
		out << RebootCount;
		out.close();
		lock.unlock();
	}
	Logger::Stream(LEVEL_INFO,tags) << "Updated Reboot Count to " << RebootCount;
}


/*!
 * Parses the IEF files once recieved
 */
void FileManager::parseIEF(){

	GroundCommunication ground;

	char *line = NULL;
	size_t len;
	ssize_t bytesRead;

	char * type;
	uint8_t lineNumber;

	lock.lock();
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
	lock.unlock();

	FileManager::deleteFile(IEF_PATH);
}

/*!
 * Parses the PPE file onces recieved
 */
void FileManager::parsePPE(){
	GroundCommunication ground;
	LogTags tags;
	tags += LogTag("Name", "FileManager");

	char * line;
	size_t len;
	ssize_t bytesRead;

	char * type;
	uint8_t lineNumber;

	lock.lock();
	FILE * file = fopen(PPE_PATH, "r");

	if (file == NULL){
		Logger::Stream(LEVEL_INFO, tags) << "No PPE File";
		lock.unlock();
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
	lock.unlock();
	FileManager::deleteFile(PPE_PATH);
}

/*!
 * Creates a tarball with a given regex
 * \param file path to the files to be delete (e.g. /home/EPS_92 would collect all files with EPS_92 in the front of their name)
 * \param R indicates if it is to be: R = the regex itself, RB = before regex, and RA = after regex, which is indicated by the reboot count of the file name
 */
vector<std::string> FileManager::packageFiles(std::string dest, std::string R){

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
		lock.lock();
		if(!(fd = popen(sh_cmd, "r"))){
			Logger::Stream(LEVEL_ERROR, tags) << "Tar creation failed.";
		}
		if(pclose(fd) == -1){
			Logger::Stream(LEVEL_ERROR, tags) << "Tar pipe could not be closed.";
		}
		lock.unlock();
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
		lock.lock();
		for(int y = 0; y < EN; y++){
			std::string newDest = dest + pastReg[y] + ".tar";
			pastReg[y] = pastReg[y] + "_";
			sprintf(sh_cmd, "tar -czf %s -C %s `ls -lr %s | grep ^- | awk '{print $9}' | grep \"%s\" | head -%d` >/dev/null 2>&1", (char*)newDest.c_str(), (char*)dest.c_str(), (char*)dest.c_str(), (char*)pastReg[y].c_str(), 50);
			if(!(fd = popen(sh_cmd, "r"))){
				Logger::Stream(LEVEL_ERROR, tags) << "Tar creation failed.";
				pclose(fd);
				break;
			}
			if(pclose(fd) == -1){
				Logger::Stream(LEVEL_ERROR, tags) << "Tar pipe could not be closed.";
				break;
			}
			Files.push_back(newDest);
		}
		lock.unlock();
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
		lock.lock();;
		std::ifstream rebootFile(REBOOT_FILE);
		int intCount;
		rebootFile >> intCount;
		rebootFile.close();
		lock.unlock();
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
		lock.lock();
		for(int y = 0; y < itr+1; y++){
			std::string newDest = dest + pastReg[y] + ".tar";
			pastReg[y] = pastReg[y] + "_";
			sprintf(sh_cmd, "tar -czf %s -C %s `ls -lr %s | grep ^- | awk '{print $9}' | grep \"%s\" | head -%d` >/dev/null 2>&1", (char*)newDest.c_str(), (char*)dest.c_str(), (char*)dest.c_str(), (char*)pastReg[y].c_str(), 50);
			if(!(fd = popen(sh_cmd, "r"))){
				Logger::Stream(LEVEL_ERROR, tags) << "Tar creation failed.";
				pclose(fd);
				break;
			}
			if(pclose(fd) == -1){
				Logger::Stream(LEVEL_ERROR, tags) << "Tar pipe could not be closed.";
				break;
			}
			Files.push_back(newDest);
		}
		lock.unlock();
	}
	// handle multiple tar balls
	Logger::Stream(LEVEL_INFO, tags) << "Files " << R << " successfully packaged with the file name " << dest;
	return Files;
}



void FileManager::getFilesList(std::string dir){

	LogTags tags;
	tags += LogTag("Name", "FileManager");
	// TODO: Error handling, decide on more cases
	DIR *dp;
	struct dirent *entry;
	int count;
	lock.lock();
	dp = opendir(dir.c_str());
	if(dp == NULL){
		Logger::Stream(LEVEL_ERROR, tags) << "Directory could not be found.";
		lock.unlock();
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
	lock.unlock();
	Logger::Stream(LEVEL_INFO, tags) << "File list successfully collected.";
}

/*!
 * Deletes a group of files with a given regex
 * \param file path to the files to be delete (e.g. /home/EPS_92 would collect all files with EPS_92 in the front of their name)
 * \param R indicates if it is to be: R = the regex itself, RB = before regex, and RA = after regex, which is indicated by the reboot count of the file name
 */
int FileManager::regexDelete(std::string dest,std::string R){

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
			lock.lock();
			if(!(fd = popen(sh_cmd, "r"))){
				Logger::Stream(LEVEL_ERROR, tags) << "Regex files could not be deleted.";
				lock.unlock();
				return -3;
			}
			if(pclose(fd) == -1){
				Logger::Stream(LEVEL_ERROR, tags) << "Shell command for removing files could not be closed.";
				lock.unlock();
				return -1;
			}
			lock.unlock();
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
			lock.lock();
			for(int y = 0; y < EN; y++){
				sprintf(sh_cmd, "rm -r `ls -l %s | grep ^- | awk '{print \"%s\" \"/\" $9}' | grep \"%s\"`", (char*)dest.c_str(), (char*)dest.c_str(), (char*)pastReg[y].c_str());;
				if(!(fd = popen(sh_cmd, "r"))){
					Logger::Stream(LEVEL_ERROR, tags) << "Regex files could not be deleted.";
					pclose(fd);
					lock.unlock();
					return -3;
				}
				if(pclose(fd) == -1){
					Logger::Stream(LEVEL_ERROR, tags) << "Shell command for removing files could not be closed.";
					lock.unlock();
					return -1;
				}
			}
			lock.unlock();
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
			lock.lock();
			std::ifstream rebootFile(REBOOT_FILE);
			int intCount;
			rebootFile >> intCount;
			rebootFile.close();
			lock.unlock();
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
			lock.lock();
			for(int y = 0; y < itr+1; y++){
				sprintf(sh_cmd, "rm -r `ls -l %s | grep ^- | awk '{print \"%s\" \"/\" $9}' | grep \"%s\"`", (char*)dest.c_str(), (char*)dest.c_str(), (char*)pastReg[y].c_str());;
				if(!(fd = popen(sh_cmd, "r"))){
					Logger::Stream(LEVEL_ERROR, tags) << "Regex files could not be deleted.";
					pclose(fd);
					lock.unlock();
					return -3;
				}
				if(pclose(fd) == -1){
					Logger::Stream(LEVEL_ERROR, tags) << "Shell command for removing files could not be closed.";
					lock.unlock();
					return -1;
				}
			}
			lock.unlock();
		}
	//TODO: Error handling
	Logger::Stream(LEVEL_INFO, tags) << "Files deleted " << R << " with the file path including " << dest;
	return 0;
}



