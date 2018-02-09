/*
 * ExternalProcess.h
 *
 *  Created on: Jan 8, 2018
 *      Author: chuffman93
 *
 */

#ifndef INCLUDE_INTERFACES_EXTERNALPROCESS_H_
#define INCLUDE_INTERFACES_EXTERNALPROCESS_H_
#include "util/Logger.h"
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <fstream>
#include <string.h>
#include <sstream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "core/Lock.h"


class ExternalProcess{
public:
	ExternalProcess();
	~ExternalProcess();
	void launchProcess(char * argv[]);
	void launchProcess(char * argv[], char * argc[]);
	void closeProcess();
	int checkPID();
private:
	pid_t tpid;
	pid_t child_pid;
	int child_status;
	Lock lock;

};



#endif /* INCLUDE_INTERFACES_EXTERNALPROCESS_H_ */
