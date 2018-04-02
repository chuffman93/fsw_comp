/*
 * FileManager.cpp

 *
 *  Created on: Dec 22, 2017
 *      Author: alex and robo
 *      Edited on: January 9th 2018 by Alex
 */

#include "core/FileManager.h"
#include <string.h>
#include <iostream>
#include <sstream>
#include <iterator>
#include <sstream>
#include <fstream>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

using namespace std;

Lock FileManager::lock;
std::string FileManager::logMessageFP = "";
int FileManager::Reboot_num = updateRebootCount();
uint16_t FileManager::MAX_FILE_SIZE;
uint32_t FileManager::MAX_DOWN_SIZE;

FileManager::FileManager(){}

FileManager::~FileManager(){}


/*!
 * Reads a file byte by byte and returning a vector of those bytes
 * \param file path to the file that is to be read
 */
std::vector<uint8_t> FileManager::readFromFile(std::string filePath){

	LockGuard l(lock);
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
	Logger::Stream(LEVEL_INFO,tags) << "Opened " << filePath << " for reading";
	return buffer;


}

/*!
 * Writes to a file
 * \param file path to the file that is to be written to
 * \param vector of bytes that is to be written to a file
 */
void FileManager::writeToFile(std::string filePath, std::vector<uint8_t> &buffer){
	LockGuard l(lock);
	LogTags tags;
	tags += LogTag("Name", "FileManager");
	if (filePath == ""){
		Logger::Stream(LEVEL_ERROR,tags) << "Unable to write to file, empty file path";
		return;
	}
	else{
		Logger::Stream(LEVEL_INFO, tags) << "Writing " << buffer.size() << " bytes to \"" << filePath << "\"";
		ofstream f(filePath.c_str(),ofstream::binary | ofstream::app);

		for(std::vector<uint8_t>::const_iterator i = buffer.begin(); i != buffer.end(); ++i){
			f << *i;
		}
		f.close();
	}

}

/*!
 * Appends to a file
 * \param file path to file that is to be added to
 * \param vector of bytes that are to be added to a file
 */
void FileManager::appendToFile(std::string filePath, std::vector<uint8_t>& buffer){
	LockGuard l(lock);
	LogTags tags;
	tags += LogTag("Name", "FileManager");
	if (filePath == ""){
		Logger::Stream(LEVEL_ERROR,tags) << "Unable to append to file, empty file path";
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
	Logger::Stream(LEVEL_INFO,tags) << "Appending " << buffer.size() << "bytes to " << filePath;
}

/*!
 * Deletes a file or directory
 * \param path to either a file or a directory
 */
void FileManager::deleteFile(std::string filePath){
	//remove(filePath.c_str());
	LockGuard l(lock);

	struct stat s;
	LogTags tags;
	tags += LogTag("Name", "FileManager");
	if (stat((char*)filePath.c_str(),&s) == 0){
		if( s.st_mode & S_IFDIR){
			Logger::Stream(LEVEL_INFO,tags) << "Deleting directory \"" << filePath << "\"";
			ExternalProcess rm;
			char * sh_cmd[] = {(char*)"/bin/rm",(char*)"-r",(char*)filePath.c_str(),NULL};
			rm.launchProcess(sh_cmd);
		}
		else{
			Logger::Stream(LEVEL_INFO,tags) << "Deleting file \"" << filePath << "\"";
			remove((char*)filePath.c_str());
		}
	}
	else{
		Logger::Stream(LEVEL_WARN,tags) << "Unable to delete file/directory \"" << filePath << "\"";
	}

}

// TODO: Do we need this?
void FileManager::appendToStringFile(std::string filePath, std::vector<std::string>& buffer){


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
	LockGuard l(lock);
	LogTag tags;
	if (access(filePath.c_str(), F_OK) == 0){
		ExternalProcess mv;
		char*sh_mv[] = {(char*)"/bin/mv",(char*)filePath.c_str(),(char*)newfilePath.c_str(),NULL};
		mv.launchProcess(sh_mv);
		Logger::Stream(LEVEL_INFO,tags) << "Moving " << filePath << " to " << newfilePath;
	}
}

/*!
 * Creates a new file name (full path) with the convention of ../nameOfFile_RebootCount_Time
 * \param base path including the Name of the file
 */
std::string FileManager::createFileName(std::string basePath){
	uint32_t currentTime = getCurrentTime();
	char filePath[50];
	if(basePath == RAD_FILE_PATH){
		sprintf(filePath,"%s_%d_%05d_",(char*)basePath.c_str(),Reboot_num,currentTime);
		return filePath;
	}
	sprintf(filePath,"%s_%d_%05d",(char*)basePath.c_str(),Reboot_num,currentTime);
	return filePath;
}

/*!
 * Copies a file into a new directory
 * \param file path to the fle to be copied
 * \param file path to the desired location
 */
void FileManager::copyFile(std::string filePath, std::string newfilePath){
	//TODO: dont use system call
	LogTag tags;
	Logger::Stream(LEVEL_INFO,tags) << "Copying " << filePath.c_str() << " to " << newfilePath.c_str();
	if (FileManager::checkExistance(filePath)){
		ExternalProcess cp;
		char*sh_cp[] = {(char*)"/bin/cp",(char*)filePath.c_str(),(char*)newfilePath.c_str(),NULL};
		cp.launchProcess(sh_cp);
		Logger::Stream(LEVEL_INFO,tags) << "Copied " << filePath.c_str() << " to " << newfilePath.c_str();

//		std::string command = "cp " + filePath+ " " + newfilePath;
//		system(command.c_str());
	}
}

/*!
 * Creates or updates the reboot count file.
 */
int FileManager::updateRebootCount(){
	LogTags tags;
	tags += LogTag("Name", "FileManager");
	int RebootCount;

	if (checkExistance(REBOOT_FILE)){
		lock.lock();
		//read boot number from file
		std::ifstream rebootFile(REBOOT_FILE);
		rebootFile >> RebootCount;
		rebootFile.close();
		lock.unlock();
		//write boot count +1 to file
		std::ofstream out(REBOOT_FILE);
		out << ++RebootCount;
		out.close();
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
	return RebootCount;
}


/*!
 * Creates or Updates the COM pass count
 */
int FileManager::updateComPassCount(){
	LogTags tags;
	tags += LogTag("Name", "FileManager");
	int ComPassCount;

	if (checkExistance(COM_PASS_COUNT)){
		lock.lock();
		//read boot number from file
		std::ifstream ComPassCountFile(COM_PASS_COUNT);
		ComPassCountFile >> ComPassCount;
		ComPassCountFile.close();
		lock.unlock();
		//write boot count +1 to file
		std::ofstream out(COM_PASS_COUNT);
		out << ++ComPassCount;
		out.close();
	}else {
		//this is the first time the boot count has been incremented, write initial boot count to file
		ComPassCount = 1;
		lock.lock();
		std::ofstream out(COM_PASS_COUNT);
		out << ComPassCount;
		out.close();
		lock.unlock();
	}
	Logger::Stream(LEVEL_INFO,tags) << "Updated ComPass Count to " << ComPassCount;
	return ComPassCount;
}

/*!
 * Used to get the compass count
 */
int FileManager::getComPassCount(){
	LogTags tags;
	tags += LogTag("Name", "FileManager");
	int ComPassCount;

	if (checkExistance(COM_PASS_COUNT)){
		lock.lock();
		//read boot number from file
		std::ifstream ComPassCountFile(COM_PASS_COUNT);
		ComPassCountFile >> ComPassCount;
		ComPassCountFile.close();
		lock.unlock();
	}else {
		//this is the first time the boot count has been incremented, write initial boot count to file
		ComPassCount = 0;
	}
	Logger::Stream(LEVEL_INFO,tags) << "Loaded ComPass Count: " << ComPassCount;
	return ComPassCount;
}

/*!
 * Creates a tarball with a given regex
 * \param file path to the files to be brought together (e.g. /home/EPS_92 would collect all files with EPS_92 in the front of their name)
 * \param R indicates if it is to be: R = the regex itself, RB = before regex, and RA = after regex, which is indicated by the reboot count of the file name
 */
vector<std::string> FileManager::packageFiles(std::string dest, std::string R){
	LockGuard l(lock);
	LogTags tags;
	tags += LogTag("Name", "FileManager");

	// TODO: size checks and error handling

	FILE *fd = NULL;
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
		Logger::Stream(LEVEL_INFO,tags) << "Created tar file for regex: " << newDest;
		if(!(fd = popen(sh_cmd, "r"))){
			Logger::Stream(LEVEL_ERROR, tags) << "Tar creation failed.";
		}
		if(pclose(fd) == -1){
			Logger::Stream(LEVEL_ERROR, tags) << "Tar pipe could not be closed.";
		}else{
			struct stat st;
			lstat(newDest.c_str(), &st);
			if (st.st_size > FileManager::MAX_DOWN_SIZE){
				Logger::Stream(LEVEL_INFO,tags) << "Tar size of: " << st.st_size << " is too large for a max downlink size of " << MAX_DOWN_SIZE << ", splitting into " << st.st_size/MAX_DOWN_SIZE << " archives";
				vector<std::string> tmp = FileManager::splitFile(newDest);
				Files.insert(Files.end(),tmp.begin(),tmp.end());
			}else{
				Files.push_back(newDest);
			}
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
			sprintf(sh_cmd, "tar -czf %s -C %s `ls -lr %s | grep ^- | awk '{print $9}' | grep \"%s\" | head -%d`>/dev/null 2>&1", (char*)newDest.c_str(), (char*)dest.c_str(), (char*)dest.c_str(), (char*)pastReg[y].c_str(), 50);
			if(!(fd = popen(sh_cmd, "r"))){
				Logger::Stream(LEVEL_ERROR, tags) << "Tar creation failed.";
				pclose(fd);
				break;
			}
			if(pclose(fd) == -1){
				Logger::Stream(LEVEL_ERROR, tags) << "Tar pipe could not be closed.";
				break;
			}else{
				struct stat st;
				stat((char*)newDest.c_str(), &st);
				if (st.st_size > FileManager::MAX_DOWN_SIZE){
					Logger::Stream(LEVEL_INFO,tags) << "Tar size of: " << st.st_size << " is too large for a max downlink size of " << MAX_DOWN_SIZE << ", splitting into " << st.st_size/MAX_DOWN_SIZE << " archives";
					vector<std::string> tmp = FileManager::splitFile(newDest);
					Files.insert(Files.end(),tmp.begin(),tmp.end());
				}else{
					Files.push_back(newDest);
				}
			}
			Logger::Stream(LEVEL_INFO,tags) << "Created tar file for regex: " << newDest;
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
		int intCount = Reboot_num;
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
				Logger::Stream(LEVEL_ERROR, tags) << "Tar creation failed.";
				pclose(fd);
				break;
			}
			if(pclose(fd) == -1){
				Logger::Stream(LEVEL_ERROR, tags) << "Tar pipe could not be closed.";
				break;
			}else{
				struct stat st;
				lstat((char*)newDest.c_str(), &st);
				if (st.st_size > FileManager::MAX_DOWN_SIZE){
					Logger::Stream(LEVEL_INFO,tags) << "Tar size of: " << st.st_size << " is too large for a max downlink size of " << MAX_DOWN_SIZE << ", splitting into " << st.st_size/MAX_DOWN_SIZE << " archives";
					vector<std::string> tmp = FileManager::splitFile(newDest);
					Files.insert(Files.end(),tmp.begin(),tmp.end());
				}else{
					Files.push_back(newDest);
				}
			}

			Logger::Stream(LEVEL_INFO,tags) << "Created tar file for regex: " << newDest;
		}
	}
	// handle multiple tar balls
	Logger::Stream(LEVEL_INFO, tags) << "Files " << R << " successfully packaged with the file name " << dest;
	return Files;
}


/*!
 * Generates a list of files in a directory
 * \param path to the directory
 */
void FileManager::generateFilesList(std::string dir){
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
	Logger::Stream(LEVEL_DEBUG,tags) << "Directory to get list from: " << dir.c_str();
	count = 0;
	// add full path
	FILE * dwlkDFL = fopen(DFL_PATH,"a+");
	fwrite(dir.c_str(),strlen(dir.c_str()),1,dwlkDFL);
	fwrite("\n",strlen("\n"),1,dwlkDFL);
	while((entry = readdir(dp))!= NULL){
		if ( entry->d_name[0] != '.'){
			fwrite(entry->d_name,strlen(entry->d_name),1,dwlkDFL);
			fwrite(",",strlen(","),1,dwlkDFL);
			fwrite("\n",strlen("\n"),1,dwlkDFL);
			Logger::Stream(LEVEL_DEBUG,tags) << "Generate file list entry: " << entry->d_name;
		}
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
			sprintf(sh_cmd, "rm -rf `ls -l %s | grep ^- | awk '{print \"%s\" \"/\" $9}' | grep \"%s\"`", (char*)dest.c_str(), (char*)dest.c_str(), (char*)regex.c_str());
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
				sprintf(sh_cmd, "rm -rf `ls -l %s | grep ^- | awk '{print \"%s\" \"/\" $9}' | grep \"%s\"`", (char*)dest.c_str(), (char*)dest.c_str(), (char*)pastReg[y].c_str());;
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
			int intCount = Reboot_num;
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
				sprintf(sh_cmd, "rm -rf `ls -l %s | grep ^- | awk '{print \"%s\" \"/\" $9}' | grep \"%s\"`", (char*)dest.c_str(), (char*)dest.c_str(), (char*)pastReg[y].c_str());;
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
		}
	//TODO: Error handling
	Logger::Stream(LEVEL_INFO, tags) << "Files deleted " << R << " with the file path including " << dest;
	return 0;
}

/*!
 * Function to parse through a given ground file
 * \param file path to the ground file
 */
std::vector<std::string> FileManager::parseGroundFile(std::string filePath){
	LogTags tags;

	char *line = NULL;
	size_t len;
	ssize_t bytesRead;

	Logger::Stream(LEVEL_INFO,tags) << "Parsing ground file: " << filePath;

	std::vector<std::string> groundRequests;

	FILE * file = fopen(filePath.c_str(), "r");

	while ((bytesRead = getline(&line, &len, file)) != -1){
		groundRequests.push_back(line);
	}

	fclose(file);
	FileManager::deleteFile(IEF_PATH);

	return groundRequests;

}

/*!
 * Function used to write logger messages to a file
 * \param tags used in the logger message
 * \param the message being recoreded
 */
void FileManager::writeLog(std::string tags,std::string message){

	if(logMessageFP == ""){
		logMessageFP = FileManager::createFileName(LOG_MESSAGES) + ".txt";
	}else{
		struct stat st;
		stat(logMessageFP.c_str(), &st);
		if(st.st_size > MAX_FILE_SIZE){
			logMessageFP = FileManager::createFileName(LOG_MESSAGES) + ".txt";
		}
	}
	ofstream f(logMessageFP.c_str(),ofstream::out | ofstream::app);
	f << tags << " " << message << '\n';
	f.close();

}

/*!
 * Returns the reboot number
 */
int FileManager::GetReboot(){
	return Reboot_num;
}

/*!
 * Sets the configs for the FileManager
 */
void FileManager::handleConfig(){
	LogTags tags;
	tags += LogTag("Name", "FileManager");
	if(FileManager::checkExistance(FMG_CONFIG)){
		std::vector<uint8_t> buff = FileManager::readFromFile(FMG_CONFIG);
		FileManager::MAX_FILE_SIZE = (uint16_t)buff.at(1) << 8 |
				buff.at(0);
		FileManager::MAX_DOWN_SIZE = (uint32_t)buff.at(5) << 24 |
				(uint32_t)buff.at(4) << 16 |
				(uint32_t)buff.at(3) << 8 |
				buff.at(2);
		Logger::Stream(LEVEL_INFO,tags) << " Setting max file size to " << FileManager::MAX_FILE_SIZE << " Bytes" <<
				"Setting max downlink size to " << FileManager::MAX_DOWN_SIZE << " bytes.";
	}else{
		FileManager::MAX_FILE_SIZE = 5000;
		FileManager::MAX_DOWN_SIZE = 100000;
		Logger::Stream(LEVEL_WARN,tags) << "No File Manager configs found";

	}
}

/*!
 * Handles updating the configs if an update is uploaded
 */
void FileManager::updateConfig(){
	LogTags tags;
	tags += LogTag("Name", "FileManager");
	if(FileManager::checkExistance(FMG_CONFIG_UP)){
		std::vector<uint8_t> buff = FileManager::readFromFile(FMG_CONFIG_UP);
		if(buff.size() != CONFIG_FMG_SIZE){
			Logger::Stream(LEVEL_ERROR,tags) << "Incorrect Size for config";
			return;
		}
		FileManager::MAX_FILE_SIZE = (uint16_t)buff.at(1) << 8|
				buff.at(0);
		FileManager::MAX_DOWN_SIZE = (uint32_t)buff.at(5) << 24 |
				(uint32_t)buff.at(4) << 16 |
				(uint32_t)buff.at(3) << 8 |
				buff.at(2);
		Logger::Stream(LEVEL_INFO,tags) << " Setting max file size to " << FileManager::MAX_FILE_SIZE << " Bytes" <<
				"Setting max downlink size to " << FileManager::MAX_DOWN_SIZE << " bytes.";
		FileManager::moveFile(FMG_CONFIG_UP,FMG_CONFIG);
	}
	else{
		Logger::Stream(LEVEL_WARN,tags) << "There are no FMG config updates";
	}
}

/*!
 * Used to split a file that exceeds the max downlink limit (used primarily for tar archives)
 * \param Path to the file needing to be split
 */
std::vector<std::string> FileManager::splitFile(std::string FilePath){
	LogTags tags;
	tags += LogTag("Name", "FileManager");

	ExternalProcess splt;

	char chunksize[15];
	sprintf(chunksize,"%d",FileManager::MAX_DOWN_SIZE);

	struct stat st;
	if(stat((char*)FilePath.c_str(), &st) == -1){
		Logger::Stream(LEVEL_ERROR,tags) << "Stat on the file: " << FilePath << " was not successful";
	}
	int n_splits = st.st_size/FileManager::MAX_DOWN_SIZE;
	char * sh_cmd[] = {(char *)"/usr/bin/split", (char*) "-b", (char*)chunksize, (char*) "-d",(char*) "-a", (char*)"3",(char*)FilePath.c_str(),(char*)FilePath.c_str(),NULL};
	splt.launchProcess(sh_cmd);
	vector<std::string> tmp;
	char temp[100];
	for(int i = 0; i <= n_splits; i++){
		sprintf(temp,"%s%03d",(char*)FilePath.c_str(),i);
		std::string buff(temp,sizeof(temp));
		Logger::Stream(LEVEL_DEBUG,tags) << "Split File #" << i << " "<< buff.c_str();
		tmp.push_back(buff);
	}
	return tmp;
}

