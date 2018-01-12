/*
 * ExternalProcess.cpp
 *
 *  Created on: Jan 8, 2018
 *      Author: chuffman93
 */
#include "interfaces/ExternalProcess.h"
#include "util/Logger.h"
using namespace std;



ExternalProcess::ExternalProcess(){
	pid = -1;
}

/*!
 * Used to initialize a process requiring only one line such as file transfer protocols
 */
void ExternalProcess::launchProcess(char * argv[]){
	int f = fork();
	if(f == 0){
		execv(argv[0],argv);
		exit(0);
	}
	this->pid = f;
}

/*!
 * Used to initialize a process that needs to also be configured (e.g. slip)
 */
void ExternalProcess::launchProcess(char * argv[],char * argc[]){
	int f = fork();
	if(f == 0){
		execv(argv[0],argv);
		execv(argc[0],argc);
		exit(0);
	}
	this->pid = f;
}
 /*!
  * Kills the  process
  */
void ExternalProcess::closeProcess(){
	if(pid != -1){
		stringstream ss;
		ss << pid;
		string pd = ss.str();
		char * argv[] = {(char *)"/bin/kill",(char*)"-9",(char*)pd.c_str() ,NULL};
		int f = fork();
		if(f == 0){
			execv(argv[0],argv);
			exit(0);
		}
		this->pid = -1;
	}
	else
		return;
}
/*!
 * 	Simple "getter" function to get the PID of the
 * 	processes to tell whether or not it is initialized.
 */
int ExternalProcess::checkPID(){
	return pid;
}
/*!
 * Run closeProcess in deconstructor to ensure it is killed
 */
ExternalProcess::~ExternalProcess(){
	this->closeProcess();
}
