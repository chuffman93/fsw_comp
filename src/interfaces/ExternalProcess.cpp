/*
 * ExternalProcess.cpp
 *
 *  Created on: Jan 8, 2018
 *      Author: chuffman93
 */
#include "util/Logger.h"
#include "interfaces/ExternalProcess.h"

using namespace std;

LogTags tags;

ExternalProcess::ExternalProcess(){
	tags += LogTag("Name", "ExternalProcess");
	child_status = -1;
	tpid = -1;
	child_pid = -1;

}

/*!
 * Used to initialize a process requiring only one line such as file transfer protocols
 * \param array of arguments to system call
 */
void ExternalProcess::launchProcess(char * argv[]){

	Logger::Stream(LEVEL_INFO,tags) << "Starting process: " << argv[0];
	child_pid = fork();
	if(child_pid == 0){
		execv(argv[0],argv);
		exit(0);
	}else{
		do{
			tpid = wait(&child_status);
		}while(tpid != child_pid);
		child_pid = -1;
	}
}

/*!
 * Used to initialize a process that needs to also be configured (e.g. slip)
 * \param array of arguments to first system call
 * \param array of arguments to second system call
 */
void ExternalProcess::launchProcess(char * argv[],char * argc[]){

	Logger::Stream(LEVEL_INFO,tags) << "Starting process: " << argv[0] << " and " << argc[0];
	child_pid = fork();
	if(child_pid == 0){
		execv(argv[0],argv);
		execv(argc[0],argc);exit(0);
	}else{

	}
}
 /*!
  * Kills the  process
  */
void ExternalProcess::closeProcess(){
	if(child_pid != -1){
		stringstream ss;
		ss << child_pid;
		string pd = ss.str();
		char * argv[] = {(char *)"/bin/kill",(char*)"-9",(char*)pd.c_str() ,NULL};
		Logger::Stream(LEVEL_INFO,tags) << "Killing process: " << pd;
		int f = fork();
		if(f == 0){
			execv(argv[0],argv);
			exit(0);
		}
		else{
			do{
				tpid = wait(&child_status);
			}
			while(tpid != child_pid);
			child_pid = -1;
		}
	}
	else
		return;
}
/*!
 * 	Simple "getter" function to get the PID of the
 * 	processes to tell whether or not it is initialized.
 */
int ExternalProcess::checkPID(){
	return child_pid;
}
/*!
 * Run closeProcess in deconstructor to ensure it is killed
 */
ExternalProcess::~ExternalProcess(){
	this->closeProcess();
}
