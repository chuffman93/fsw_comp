/*
 * CMDStdTasks.cpp
 *
 *  Created on: May 21, 2013
 *      Author: Brian Campuzano
 */

#include "servers/CMDServer.h"
#include "servers/SCHServer.h"
#include "servers/CMDStdTasks.h"
#include "servers/DispatchStdTasks.h"
#include "core/StdTypes.h"
#include "core/Singleton.h"
#include "core/Factory.h"
#include "core/WatchdogManager.h"
#include "util/FileHandler.h"
#include "util/Logger.h"
#include "util/Diagnostics.h"
#include <termios.h>
#include <string>
#include <string.h>

using namespace AllStar::Core;
using namespace std;

namespace AllStar{
namespace Servers{

#define NUM_DIAGNOSTIC_TESTS 4
#define SIZE_THRESHOLD 1000

void portSetup(void){
	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));

	char portname[20] = "/dev/ttyS2";
	struct termios port;
	int portfd = open(portname, O_RDWR | O_NOCTTY | O_NDELAY);

	if(portfd == -1){
		logger->Log(LOGGER_LEVEL_FATAL, "CMDServer: Failed to open serial port!");
	}

	if(tcgetattr(portfd, &port) < 0) {
		logger->Log(LOGGER_LEVEL_ERROR, "CMDServer: Problem with initial port attributes.");
	}

	port.c_iflag &= ~IXON;

	if(cfsetispeed(&port, B115200) < 0 || cfsetospeed(&port, B115200) < 0){
		logger->Log(LOGGER_LEVEL_FATAL, "GPSServer: Problem setting the baud rate!");
	}

	if(tcsetattr(portfd, TCSANOW, &port) < 0){
		logger->Log(LOGGER_LEVEL_ERROR, "GPSServer: Problem setting port attributes!");
	}
}

void uftpSetup(void){
	char cmdString[100];

	//---------------------------SLIP------------------------------------
	sprintf(cmdString, "slattach -L -m -p slip \"/dev/ttyS2\" &");
	printf("%s\n", cmdString);
	system(cmdString);

	sprintf(cmdString, "ifconfig sl0 1.1.1.1 pointopoint 1.1.1.2 up");
	printf("%s\n", cmdString);
	system(cmdString);

	//---------------------------UFTP-----------------------------------
	//TODO add encryption
	char tmpDir[20] = "~/tmp";
	char fileDir[20] = "~/dow";
	//char prvKey[20] = "~/rsakey.pem";
	//sprintf(cmdString, "./uftpd -T %s -I sl0 -D %s -k %s -E", tmpDir, fileDir, prvKey);
	sprintf(cmdString, "./uftpd -T %s -I sl0 -D %s", tmpDir, fileDir);
	printf("%s\n", cmdString);
	system(cmdString);
}

void runDiagnostic(void){
	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
	logger->Log(LOGGER_LEVEL_INFO, "CMDServer: running diagnostics tests");
	FILE * fp = fopen("diagnostics.txt", "r");

	if(fp == NULL){
		logger->Log(LOGGER_LEVEL_WARN, "Error opening diagnostic tests file");

		// TODO: should we run a pre-specified test?
		FILE * fpr = fopen("diagnostic_results.txt", "a+");
		char results[] = "No tests to run\n";
		fwrite(results, 1, sizeof(results), fpr);
		fclose(fpr);
	}else{
		uint8 i = 0;
		bool isActive[NUM_DIAGNOSTIC_TESTS] = {false};
		char c;
		while(i < NUM_DIAGNOSTIC_TESTS){
			c = fgetc(fp);
			if(c == EOF){
				logger->Log(LOGGER_LEVEL_WARN, "Diagnostic tests file too short!");
				break;
			}
			isActive[i] = (c == '1') ? true : false;
			i++;
		}

		FILE * fpr = fopen("diagnostic_results.txt", "a+");
		fprintf(fpr, "Diagnostics Test @ %lld\n", getTimeInMillis());
		fclose(fpr);

		// Conditionally run tests here TODO: rewrite with function pointers
		BusLoadTest(isActive[0]);
		SPIStats(isActive[1]);
		SampleTest2(isActive[2]);
		SampleTest3(isActive[3]);

		fclose(fp);

		// FIXME: decide on deletion/transfer scheme
		//remove("diagnostics.txt");
	}
}

void parseDRF(void){
	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
	FILE * fp;
	char * line = NULL;
	size_t len = 0;
	ssize_t read;

	// open the files to downlink file
	fp = fopen(DRF_PATH, "r");
	if (fp == NULL){
		logger->Log(LOGGER_LEVEL_ERROR, "CMDStdTasks: error opening DRF");
		return;
	}

	// parse the file line by line
	char * archive;
	char * dir;
	char * num;
	int numFiles;
	char * regex;
	while ((read = getline(&line, &len, fp)) != -1) {
		// ---- Parse line
		archive = strtok(line,",");
		if(archive == NULL){
			logger->Log(LOGGER_LEVEL_WARN, "Incomplete DRF line at archive");
			continue; // skip to the next line
		}

		dir = strtok(NULL,",");
		if(dir == NULL){
			logger->Log(LOGGER_LEVEL_WARN, "Incomplete DRF line at directory");
			continue; // skip to the next line
		}

		num = strtok(NULL,",");
		if(num == NULL){
			logger->Log(LOGGER_LEVEL_WARN, "Incomplete DRF line at number");
			continue; // skip to the next line
		}
		numFiles = atoi(num);
		if(numFiles < 1){
			logger->Log(LOGGER_LEVEL_WARN, "Invalid DRF line at number");
			continue; // skip to the next line
		}

		regex = strtok(NULL,",");
		if(regex == NULL){
			logger->Log(LOGGER_LEVEL_WARN, "Incomplete DRF line at regex");
			continue; // skip to the next line
		}

		// ---- Call the function to gather and compress
		string lsResults[numFiles];
		if(strcmp(regex,"X\n") == 0 || strcmp(regex,"X") == 0){
			logger->Log(LOGGER_LEVEL_DEBUG, "No regex");
			regex = "";
		}else{
			logger->Log(LOGGER_LEVEL_DEBUG, "regex");
		}

		packageFiles(archive, dir, regex, numFiles); // TODO: error check this?
	}

	fclose(fp);

	logger->Log(LOGGER_LEVEL_INFO, "Finished DRF");
}

void parseDLT(void){
	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
	FILE * fp;
	char * line = NULL;
	size_t len = 0;
	ssize_t read;

	// open the files to downlink file
	fp = fopen(DLT_PATH, "r");
	if (fp == NULL){
		logger->Log(LOGGER_LEVEL_ERROR, "CMDStdTasks: error opening DLT\n");
		return;
	}

	// parse the file line by line
	char * dir;
	char * num;
	int numFiles;
	char * regex;
	while ((read = getline(&line, &len, fp)) != -1) {
		// ---- Parse line
		dir = strtok(line,",");
		if(dir == NULL){
			logger->Log(LOGGER_LEVEL_WARN, "Incomplete DLT line at dir");
			continue; // skip to the next line
		}

		num = strtok(NULL,",");
		if(num == NULL){
			logger->Log(LOGGER_LEVEL_WARN, "Incomplete DLT line at number");
			continue; // skip to the next line
		}
		numFiles = atoi(num);
		if(numFiles < -1 || numFiles == 0){
			logger->Log(LOGGER_LEVEL_WARN, "Invalid DLT line at number");
			continue;
		}

		regex = strtok(NULL,",");
		if(regex == NULL){
			logger->Log(LOGGER_LEVEL_WARN, "Incomplete DLT line at regex");
			continue; // skip to the next line
		}

		// ---- find deletion scheme and delete
		if((strcmp(regex,"X\n") == 0 || strcmp(regex,"X") == 0) && numFiles != -1){
			logger->Log(LOGGER_LEVEL_WARN, "---- No regex");
		}else if(!(strcmp(regex,"X\n") == 0 || strcmp(regex,"X") == 0) && numFiles == -1){
			logger->Log(LOGGER_LEVEL_WARN, "---- Regex");
		}else{
			logger->Log(LOGGER_LEVEL_WARN, "---- Invalid combination");
		}
	}

	fclose(fp);

	logger->Log(LOGGER_LEVEL_INFO, "Finished DLT");
}

void postPassExecution(void){
	// parse and execute post-pass execution script
}

void processUplinkFiles(void){
	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));

	// Move new schedule from the uplink directory to its final directory
	if(access(SCHEDULE_PATH, F_OK) != -1){
		rename(SCHEDULE_PATH, SCH_SCHEDULE_FILE);
	}else{
		logger->Log(LOGGER_LEVEL_WARN, "ProcessUplinkFiles(): No new schedule file!");
	}
}

string trimNewline(string buf){
  // Remove the newline at the end of a string
  size_t len = buf.length();
  if (len && (buf[len-1] == '\n')) {
    buf[len-1] = '\0';
  }
  return buf;
}

int getFileSize(string filePath, string regex, int maxFiles){
    Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
	FILE * fd;
	int size;

	// sh command to get the total size of all the files we want to compress
	char sh_cmd[248];
	sprintf(sh_cmd, "tot=0; for num in `ls -lr %s | grep ^- | grep %s | awk '{print $5}' | head -%d`; do tot=$(($tot+$num)); done; echo $tot", filePath, regex, maxFiles);

	// Execute an sh script and pipe the results back to the file descriptor fd
	if(!(fd = popen(sh_cmd, "r"))){
	    logger->Log(LOGGER_LEVEL_ERROR, "GetFileSize: could not verify file sizes");
	    return -1;
	}

	// Parse ls command into array
	int loop_count = 0;
	char buff[512];
	while(fgets(buff, sizeof(buff), fd)!=NULL){
		if (loop_count > 0) {
			logger->Log(LOGGER_LEVEL_ERROR, "GetFileSize: sh command produced too many results");
			return -2;
		}
	    size = atoi(buff);
	    loop_count++;
	}

	if (pclose(fd) == -1){
		logger->Log(LOGGER_LEVEL_WARN, "GetFileSize: Error closing file stream");
	}

	return size;
}

int packageFiles(string destination, string filePath, string regex, int maxFiles){
    Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
	FILE * fd;

	int size = getFileSize(filePath, regex, maxFiles);

	if (size < 0) {
		logger->Log(LOGGER_LEVEL_ERROR, "PackageFiles: Error detected, aborting packaging");
		return -1;
	}
	else if (size > SIZE_THRESHOLD){
		logger->Log(LOGGER_LEVEL_ERROR, "PackageFiles: Total file size is too great to package");
		return -2;
	}

	// sh command to tar the files to package
	char sh_cmd[248];
	sprintf(sh_cmd, "tar -cf %s `ls -lr %s | grep ^- | awk '{print $9}' | grep %s | head -%s`", destination, filePath, regex, maxFiles);

	// Execute a shell script and pipe the results back to the file descriptor fd
	if(!(fd = popen(sh_cmd, "r"))){
	    logger->Log(LOGGER_LEVEL_ERROR, "PackageFiles: Error packaging files");
	    return -3;
	}

	if (pclose(fd) == -1){
		logger->Log(LOGGER_LEVEL_WARN, "PackageFiles: Error closing file stream");
	}

	return 0;
}

} // End of namespace Servers
} // End of namespace AllStar
