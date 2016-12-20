/*
 * CMDStdTasks.h
 *
 *  Created on: May 21, 2013
 *      Author: Brian Campuzano
 */

#ifndef CMDSTDTASKS_H_
#define CMDSTDTASKS_H_

namespace AllStar{
namespace Servers{

void portSetup(void);
void uftpSetup(void);
void runDiagnostic(void);
void parseDRF(void);
void parseDLT(void);
void postPassExecution(void);
void processUplinkFiles(void);
void trimNewline(string buf);
int getFileSize(string filePath, string regex, int maxFiles);
int compressFiles(string destination, string filePath, string regex, int maxFiles, int size_threshold);

} // End of namespace Servers
} // End of namespace AllStar

#endif /* CMDSTDTASKS_H_ */
