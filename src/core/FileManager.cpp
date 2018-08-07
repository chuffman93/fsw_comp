/*
 * FileManager.cpp

 *
 *  Created on: Dec 22, 2017
 *      Author: alex and robo
 *      Edited on: January 9th 2018 by Alex
 *
 *	-Ready for Code Review
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
#include "util/ByteStream.h"
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
		ofstream f(filePath.c_str(),ofstream::binary | ofstream::out);

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
 * Deletes a file or directory
 * \param path to either a file or a directory
 */
void FileManager::deleteFile(std::string filePath){
	if(filePath != NEW_SCH || filePath != SOT_PATH || filePath != IEF_PATH || filePath!=PPE_PATH)LockGuard l(lock);
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

/*!
 * Writes a string to a file
 * \param Path to file to be written to
 * \param string vector that is to be written
 */
void FileManager::writeToStringFile(std::string filePath, std::vector<std::string>& buffer){
	LockGuard l(lock);
	LogTags tags;
	tags += LogTag("Name", "FileManager");
	Logger::Stream(LEVEL_WARN,tags) << "Writing string to " << filePath;
	std::ofstream cmdFile(filePath.c_str(),std::ofstream::app);
	std::vector<std::string>::iterator it;
	for (it = buffer.begin(); it != buffer.end(); it++){
		cmdFile << *(it);
		Logger::Stream(LEVEL_DEBUG,tags) << "Wrote: " << *(it) << " to file";
	}
	cmdFile.close();
}


/*!
 * Checks the existance of a file
 * \param file path to file to be checked
 */
bool FileManager::checkExistance(std::string filePath){
	LockGuard l(lock);
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
	LogTag tags;
	Logger::Stream(LEVEL_INFO,tags) << "Copying " << filePath.c_str() << " to " << newfilePath.c_str();
	if (FileManager::checkExistance(filePath)){
		ExternalProcess cp;
		char*sh_cp[] = {(char*)"/bin/cp",(char*)filePath.c_str(),(char*)newfilePath.c_str(),NULL};
		cp.launchProcess(sh_cp);
		Logger::Stream(LEVEL_INFO,tags) << "Copied " << filePath.c_str() << " to " << newfilePath.c_str();
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
		//write boot count +1 to file
		std::ofstream out(COM_PASS_COUNT);
		out << ++ComPassCount;
		out.close();
		lock.unlock();
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
		ComPassCount = 0;
		Logger::Stream(LEVEL_ERROR,tags) << "No Com Pass File";
	}
	Logger::Stream(LEVEL_INFO,tags) << "Loaded ComPass Count: " << ComPassCount;
	return ComPassCount;
}

/*!
 * Creates a tarball with a given regex, not actually regex...
 * \param file path to the files to be brought together (e.g. /home/EPS_92 would collect all files with EPS_92 in the front of their name)
 * \param R indicates if it is to be: R = the regex itself, RB = before regex, and RA = after regex, which is indicated by the reboot count of the file name
 */
vector<std::string> FileManager::packageFiles(std::string dest, std::string R){
	LockGuard l(lock);
	LogTags tags;
	tags += LogTag("Name", "FileManager");

	FILE *fd = NULL;
	char sh_cmd[256];
	vector<std::string> Files;
	// manage fullfilepath + regex manipulation
	// if one is created .tar = filePath + regex.tar

	// Looking for the one epoch number
	if(R == "R"){
		// iterator location
		int i = dest.length()-1;
		std::string regex = "";
		// get the string to search for
		while(dest[i]!='/'){
			regex = dest[i]+regex;
			i--;
		}
		// set the a new string based on the regex used for finding the location
		const std::string ext(regex);
		// check to make sure the ext is not the dest
		if(dest != ext && dest.size() > ext.size() && (dest.substr((dest.size() - ext.size())) == regex)){
			// if not set the dest to the file path up to the "regex" string
			dest = dest.substr(0,dest.size()-ext.size());
		}
		//create a .tar location for collecting all files under the condition
		std::string newDest = dest + regex + ".tar";
		Logger::Stream(LEVEL_DEBUG,tags) << "Packaging " << dest.c_str() << " into " << newDest.c_str() << " with regex " << R.c_str();
		regex = regex+"_";
		// tar all files together for the first 50 files with the matching string
		sprintf(sh_cmd, "tar -czf %s -C %s `ls -lr %s | grep ^- | awk '{print $9}' | grep \"%s\" | head -%d`>/dev/null 2>&1",
				(char*)newDest.c_str(), (char*)dest.c_str(), (char*)dest.c_str(), (char*)regex.c_str(), 50);
		Logger::Stream(LEVEL_INFO,tags) << "Created tar file for regex: " << newDest.c_str();
		// used a pipe call to run the tarballing
		if(!(fd = popen(sh_cmd, "r"))){
			Logger::Stream(LEVEL_ERROR, tags) << "Tar creation failed.";
		}
		if(pclose(fd) == -1){
			Logger::Stream(LEVEL_ERROR, tags) << "Tar pipe could not be closed.";
		}else{
			// Check to make sure the tar file was created properly
			struct stat st;
			int success = lstat(newDest.c_str(), &st);
			if(success == -1){
				Logger::Stream(LEVEL_ERROR,tags) << "Stat of file could not be found.";
				return Files;
			}
			// check to make sure the file is under the max downlink size
			if (st.st_size > FileManager::MAX_DOWN_SIZE){
				// if not split it into smaller files
				Logger::Stream(LEVEL_INFO,tags) << "Tar size of: " << st.st_size << " is too large for a max downlink size of " << MAX_DOWN_SIZE << ", splitting into " << st.st_size/MAX_DOWN_SIZE << " archives";
				vector<std::string> tmp = FileManager::splitFile(newDest);
				// insert the names into the vector of files
				Files.insert(Files.end(),tmp.begin(),tmp.end());
			}else if(st.st_size < 0){
				Logger::Stream(LEVEL_ERROR,tags) << "Invalid file size for: " << newDest.c_str();
			}else{
				// If it is not invalid or too big add it to the new
				Files.push_back(newDest);
			}
		}

	// used for checking for specific files prior to an epoch
	}else if(R == "RB"){
		int i = dest.length()-1;
		std::string regex = "";
		std::string num = "";
		// first get the epic number
		while(dest[i]!='_'){
			num = dest[i]+num;
			i--;
		}
		// save it
		int EN = atoi(num.c_str());
		// save the ending string
		std::string end = "_"+num;
		i = dest.length()-1;
		// find the new file name
		while(dest[i]!='/'){
			regex = dest[i]+regex;
			i--;
		}
		// set the file name to a holder string
		std::string nRegex = regex.substr(0,regex.size() - end.size());
		// create the array to hold the file names of the file names from 1 -> the requested
		// epoch number
		std::string pastReg[EN];
		// get the names of the files ("name_num")
		for(int x = 1; x < EN ; x++){
			std::ostringstream oss;
			oss << x;
			pastReg[x-1] = nRegex+"_"+oss.str();
		}
		const std::string ext(regex);
		if(dest != ext && dest.size() > ext.size() && (dest.substr((dest.size() - ext.size())) == regex)){
			dest = dest.substr(0,dest.size()-ext.size());
		}
		// iterate through the array and repeat the process of tarballing the various collections and add them to the
		// vector to be passed on
		for(int y = 0; y < EN; y++){
			std::string newDest = dest + pastReg[y] + ".tar";
			pastReg[y] = pastReg[y] + "_";
			sprintf(sh_cmd, "tar -czf %s -C %s `ls -lr %s | grep ^- | awk '{print $9}' | grep \"%s\" | head -%d`>/dev/null 2>&1", (char*)newDest.c_str(), (char*)dest.c_str(), (char*)dest.c_str(), (char*)pastReg[y].c_str(), 50);
			if(!(fd = popen(sh_cmd, "r"))){
				Logger::Stream(LEVEL_ERROR, tags) << "Tar creation failed.";
				pclose(fd);
				continue;
			}
			if(pclose(fd) == -1){
				Logger::Stream(LEVEL_ERROR, tags) << "Tar pipe could not be closed.";
				continue;
			}else{
				struct stat st;
				int success = lstat((char*)newDest.c_str(), &st);
				if(success == -1){
					Logger::Stream(LEVEL_ERROR,tags) << "Stat of file could not be found.";
					continue;
				}
				if (st.st_size > FileManager::MAX_DOWN_SIZE){
					Logger::Stream(LEVEL_INFO,tags) << "Tar size of: " << st.st_size << " is too large for a max downlink size of " << MAX_DOWN_SIZE << ", splitting into " << st.st_size/MAX_DOWN_SIZE << " archives";
					vector<std::string> tmp = FileManager::splitFile(newDest);
					Files.insert(Files.end(),tmp.begin(),tmp.end());
				}else if(st.st_size < 0){
					Logger::Stream(LEVEL_ERROR,tags) << "Invalid file size for: " << newDest.c_str();
					continue;
				}else{
					Files.push_back(newDest);
				}
			}
			Logger::Stream(LEVEL_INFO,tags) << "Created tar file for regex: " << newDest;
		}
	// Used to find files after a give epoch number
	}else if(R == "RA"){
		int i = dest.length()-1;
		std::string regex = "";
		std::string num = "";
		// get the number passed in
		while(dest[i]!='_'){
			num = dest[i]+num;
			i--;
		}
		// save it
		int EN = atoi(num.c_str());
		std::string end = "_"+num;
		i = dest.length()-1;
		// get the name of the file to be searched for
		while(dest[i]!='/'){
			regex = dest[i]+regex;
			i--;
		}
		std::string nRegex = regex.substr(0,regex.size() - end.size());
		// get the rebootnumber count
		int intCount = Reboot_num;
		// calculate how many epochs have happened
		int itr = intCount - EN;
		// create an array to hold all of the file names
		std::string pastReg[itr+1];
		// run from the requested epoch to the current epoch
		for(int x = 0; x < itr+1; x++){
			std::ostringstream oss;
			oss << EN;
			pastReg[x] = nRegex+"_"+oss.str();
			EN++;
		}
		// get the file destination
		const std::string ext(regex);
		if(dest != ext && dest.size() > ext.size() && (dest.substr((dest.size() - ext.size())) == regex)){
			dest = dest.substr(0,dest.size()-ext.size());
		}
		// run through the array tarbaling all matching files+epoch number combinations
		// adding them to the vector to be passed on
		for(int y = 0; y < itr+1; y++){
			std::string newDest = dest + pastReg[y] + ".tar";
			pastReg[y] = pastReg[y] + "_";
			sprintf(sh_cmd, "tar -czf %s -C %s `ls -lr %s | grep ^- | awk '{print $9}' | grep \"%s\" | head -%d` >/dev/null 2>&1", (char*)newDest.c_str(), (char*)dest.c_str(), (char*)dest.c_str(), (char*)pastReg[y].c_str(), 50);
			if(!(fd = popen(sh_cmd, "r"))){
				Logger::Stream(LEVEL_ERROR, tags) << "Tar creation failed.";
				pclose(fd);
				continue;
			}
			if(pclose(fd) == -1){
				Logger::Stream(LEVEL_ERROR, tags) << "Tar pipe could not be closed.";
				continue;
			}else{
				struct stat st;
				int success = lstat((char*)newDest.c_str(), &st);
				if(success == -1){
					Logger::Stream(LEVEL_ERROR,tags) << "Stat of file could not be found.";
					continue;
				}
				if (st.st_size > FileManager::MAX_DOWN_SIZE){
					Logger::Stream(LEVEL_INFO,tags) << "Tar size of: " << st.st_size << " is too large for a max downlink size of " << MAX_DOWN_SIZE << ", splitting into " << st.st_size/MAX_DOWN_SIZE << " archives";
					vector<std::string> tmp = FileManager::splitFile(newDest);
					Files.insert(Files.end(),tmp.begin(),tmp.end());
				}else if(st.st_size < 0){
					Logger::Stream(LEVEL_ERROR,tags) << "Invalid file size for: " << newDest.c_str();
					continue;
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
	lock.lock();
	Logger::Stream(LEVEL_DEBUG,tags) << "Parsing directory: " << dir;
	// Open Directory
	dp = opendir(dir.c_str());
	// Error Check
	if(dp == NULL){
		Logger::Stream(LEVEL_ERROR, tags) << "Directory could not be found.";
		lock.unlock();
		return;
	}
	Logger::Stream(LEVEL_DEBUG,tags) << "Directory to get list from: " << dir.c_str();
	// add full path
	FILE * dwlkDFL = fopen(DFL_PATH,"a+");
	fwrite(dir.c_str(),strlen(dir.c_str()),1,dwlkDFL);
	fwrite("\n",strlen("\n"),1,dwlkDFL);
	// Read in contents of directory and write them to a file
	while((entry = readdir(dp))!= NULL){
		if ( entry->d_name[0] != '.'){
			fwrite(entry->d_name,strlen(entry->d_name),1,dwlkDFL);
			fwrite(",",strlen(","),1,dwlkDFL);
			fwrite("\n",strlen("\n"),1,dwlkDFL);
			Logger::Stream(LEVEL_DEBUG,tags) << "Generate file list entry: " << entry->d_name;
		}
	}
	// add an end line to the end of the file for a space between directorys
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
 *
 * Ran the same as the Package files function, with the exception of the rm command as opposed to the tar command
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
				pastReg[x] = nRegex+"_"+oss.str();
			}
			const std::string ext(regex);
			if(dest != ext && dest.size() > ext.size() && (dest.substr((dest.size() - ext.size())) == regex)){
				dest = dest.substr(0,dest.size()-ext.size());
			}
			lock.lock();
			for(int y = 0; y < EN; y++){
				sprintf(sh_cmd, "rm -rf `ls -l %s | grep ^- | awk '{print \"%s\" \"/\" $9}' | grep \"%s\"`",
						(char*)dest.c_str(), (char*)dest.c_str(), (char*)pastReg[y].c_str());;
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
				pastReg[x] = nRegex+"_"+oss.str();
				EN++;
			}
			const std::string ext(regex);
			if(dest != ext && dest.size() > ext.size() && (dest.substr((dest.size() - ext.size())) == regex)){
				dest = dest.substr(0,dest.size()-ext.size());
			}
			for(int y = 0; y < itr+1; y++){
				sprintf(sh_cmd, "rm -rf `ls -l %s | grep ^- | awk '{print \"%s\" \"/\" $9}' | grep \"%s\"`",
						(char*)dest.c_str(), (char*)dest.c_str(), (char*)pastReg[y].c_str());;
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
		ByteStream bs(buff);
		bs >> FileManager::MAX_FILE_SIZE >> FileManager::MAX_DOWN_SIZE;
		Logger::Stream(LEVEL_INFO,tags) << " Setting max file size to " << FileManager::MAX_FILE_SIZE << " Bytes" <<
				"Setting max downlink size to " << FileManager::MAX_DOWN_SIZE << " bytes.";
	}else{
		// default configs
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
		ByteStream bs(buff);
		bs >> FileManager::MAX_FILE_SIZE >> FileManager::MAX_DOWN_SIZE;
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
	Logger::Stream(LEVEL_DEBUG,tags) << "File Being Split: " << FilePath.c_str();

	ExternalProcess splt;

	char chunksize[15];
	sprintf(chunksize,"%d",FileManager::MAX_DOWN_SIZE);


	struct stat st;
	if(stat((char*)FilePath.c_str(), &st) == -1){
		Logger::Stream(LEVEL_ERROR,tags) << "Stat on the file: " << FilePath.c_str() << " was not successful";
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

