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
bool checkForSOT(void);

} // End of namespace Servers
} // End of namespace AllStar

#endif /* CMDSTDTASKS_H_ */
