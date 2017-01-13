/*
 * CMDStdTasks.h
 *
 *  Created on: May 21, 2013
 *      Author: Brian Campuzano
 */

#ifndef CMDSTDTASKS_H_
#define CMDSTDTASKS_H_

#include <string>

using namespace std;
namespace AllStar{
namespace Servers{

typedef enum FSWCommandTypes{
	FSW_CMD_EPS_REBOOT = 1,
	FSW_CMD_CDH_REBOOT,
	FSW_CMD_TX_SILENCE,
	FSW_CMD_MAX = FSW_CMD_TX_SILENCE
}FSWCommand;

void uftpSetup(void);
void runDiagnostic(void);
void parseDRF(void);
void parseDLT(void);
void parsePPE(void);
void processUplinkFiles(void);
string trimNewline(string buf);
const long getFileSize(const char * filePath, const char * regex, const int maxFiles);
const int packageFiles(const char * destination, const char * filePath, const char * regex, const int maxFiles);
int deleteOldest(char * filePath, int numFiles);
int deleteRegex(char * filePath, char * regex);
int getNumFiles(char * dir);
string getDownlinkFile(int fileNum);
void executeFSWCommand(int command);

} // End of namespace Servers
} // End of namespace AllStar

#endif /* CMDSTDTASKS_H_ */
