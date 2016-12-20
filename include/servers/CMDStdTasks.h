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
int packageFiles(string destination, string filePath, string regex, int maxFiles);

} // End of namespace Servers
} // End of namespace AllStar

#endif /* CMDSTDTASKS_H_ */
