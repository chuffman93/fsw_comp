/*
 * CMDStdTasks.h
 *
 *  Created on: May 21, 2013
 *      Author: Brian Campuzano
 */

#ifndef CMDSTDTASKS_H_
#define CMDSTDTASKS_H_

#include <string>
#include "core/StdTypes.h"
#include "servers/EPSServer.h"
#include "servers/ACSServer.h"
#include "servers/CDHServer.h"
#include "servers/PLDServer.h"

using namespace std;
namespace AllStar{
namespace Servers{

void uftpSetup(void);
bool openIEF(FILE ** fp, char ** line, size_t * len);
bool parseIEFLine(FILE * fp, char ** line, size_t * len, uint8 lineNum);
void parseDRF(void);
void parseDLT(void);
void parsePPE(void);
void parseDFL(void);
void processUplinkFiles(void);
string trimNewline(string buf);
void prepRADDownlink(char * line, uint8 lineNum);
void prepDataDownlink(char * line, uint8 lineNum);
const long getFileSize(const char * filePath, const char * regex, const int maxFiles);
const int packageFiles(const char * destination, const char * filePath, const char * regex, const int maxFiles, uint8 lineNum);
int deleteOldest(char * filePath, int numFiles, uint8 lineNum);
int deleteRegex(char * filePath, char * regex, uint8 lineNum);
int getNumFiles(char * dir);
string getDownlinkFile(int fileNum);
void executeFSWCommand(int command, uint8 lineNum);
int checkForSOT(void);
void getFilesList(string dir);

} // End of namespace Servers
} // End of namespace AllStar

#endif /* CMDSTDTASKS_H_ */
