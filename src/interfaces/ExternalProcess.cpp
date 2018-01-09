/*
 * ExternalProcess.cpp
 *
 *  Created on: Jan 8, 2018
 *      Author: chuffman93
 */
#include "interfaces/ExternalProcess.h"
#include "util/Logger.h"
#include <unistd.h>
using namespace std;



ExternalProcess::ExternalProcess(){
	slattach_pid = -1;
	uftpd_pid = -1;
	tftp_pid = -1;
	uftp_pid = -1;

}

/*!
 * Launch slattach to given port (adjust for flight).
 * 	Also uses ifconfig to configure the port.
 */
void ExternalProcess::launchSlattach(){

	char * argvS[] = {(char *)"/sbin/slattach",(char *)"-L",(char *)"-m",(char *)"-s",(char *)"115200",(char *)"-p",(char *)"slip",(char *)"/dev/tty52", NULL};
	int f = fork();
	if(f == 0){
		if(execv(argvS[0],argvS) == -1){
			// TODO: Handle Error
		}

		exit(0);
	}
	this->slattach_pid = f;
	// sleep to allow time for slattach to occur
	sleep(5);
	char * argvC[] = {(char *)"/sbin/ifconfig",(char *)"sl0",(char *)"1.1.1.2",(char *)"pointopoint",(char *)"1.1.1.1",(char *)"mtu",(char *)"256",(char *)"multicast",(char *)"up",NULL};
	f = fork();
	if (f==0){
		execv(argvC[0],argvC);
		exit(0);
	}
}
 /*!
  * Kills the slattach process
  */
void ExternalProcess::closeSlattach(){
	if(slattach_pid != -1){
		stringstream ss;
		ss << slattach_pid;
		string pid = ss.str();
		char * argv[] = {(char *)"/bin/kill",(char*)"-9",(char*)pid.c_str() ,NULL};
		int f = fork();
		if(f == 0){
			execv(argv[0],argv);
			exit(0);
		}
		this->slattach_pid = -1;
	}
	else
		return;
}
/*!
 *  Launches tftp
 */
void ExternalProcess::launchTftp(char * argv[]){
	int f = fork();
	if(f==0){
		execv(argv[0],argv);
		exit(0);
	}
	this->tftp_pid = f;
}
/*!
 * 	Closes tftp
 */
void ExternalProcess::closeTftp(){
	if(tftp_pid != -1){
			stringstream ss;
			ss << tftp_pid;
			string pid = ss.str();
			char * argv[] = {(char *)"/bin/kill",(char*)"-9",(char*)pid.c_str() ,NULL};
			int f = fork();
			if(f == 0){
				execv(argv[0],argv);
				exit(0);
			}
			this->slattach_pid = -1;
		}
		else
			return;
}
/*!
 * 	Launches uftpd
 */
void ExternalProcess::lauchUftpd(char * argv[]){
	int f = fork();
	if(f==0){
		execv(argv[0],argv);
		exit(0);
	}
	this->tftp_pid = f;
}

/*!
 * Closes uftpd
 */
void ExternalProcess::closeUftpd(){
	if(uftpd_pid != -1){
			stringstream ss;
			ss << uftpd_pid;
			string pid = ss.str();
			char * argv[] = {(char *)"/bin/kill",(char*)"-9",(char*)pid.c_str() ,NULL};
			int f = fork();
			if(f == 0){
				execv(argv[0],argv);
				exit(0);
			}
			this->slattach_pid = -1;
		}
		else
			return;
}
/*!
 * Launches uftp
 */
void ExternalProcess::launchUftp(char * argv[]){
	int f = fork();
	if(f==0){
		execv(argv[0],argv);
		exit(0);
	}
	this->tftp_pid = f;
}
/*!
 * Closes uftp
 */
void ExternalProcess::closeUftp(){
	if(uftp_pid != -1){
		stringstream ss;
		ss << uftp_pid;
		string pid = ss.str();
		char * argv[] = {(char *)"/bin/kill",(char*)"-9",(char*)pid.c_str() ,NULL};
		int f = fork();
		if(f == 0){
			execv(argv[0],argv);
			exit(0);
		}
		this->slattach_pid = -1;
	}
	else
		return;
}


/*!
 * 	Simple "getter" function to get the PID of the different
 * 	processes to tell whether or not the are initialized.
 */
int ExternalProcess::checkPID(std::string process){
	if (process == "slattach")
		return slattach_pid;
	else if(process == "uftpd")
		return uftpd_pid;
	else if(process == "uftp")
		return uftp_pid;
	else if(process == "tftp")
		return tftp_pid;
	else
		return -1;
}

ExternalProcess::~ExternalProcess(){
	this->closeSlattach();
	this->closeTftp();
	this->closeUftp();
	this->closeUftpd();
}
