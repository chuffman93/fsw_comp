/*
 * ExternalProcess.h
 *
 *  Created on: Jan 8, 2018
 *      Author: chuffman93
 *
 */

#ifndef INCLUDE_INTERFACES_EXTERNALPROCESS_H_
#define INCLUDE_INTERFACES_EXTERNALPROCESS_H_

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <fstream>
#include <string.h>
#include <sstream>


class ExternalProcess{
public:
	ExternalProcess();
	~ExternalProcess();
	void launchProcess(char * argv[]);
	void launchProcess(char * argv[], char * argc[]);
	void closeProcess();
	int checkPID();
private:
	int pid;



};



#endif /* INCLUDE_INTERFACES_EXTERNALPROCESS_H_ */
