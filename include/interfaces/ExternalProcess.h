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
	void launchSlattach();
	void launchTftp(char * argv[]);
	void launchUftp(char * argv[]);
	void lauchUftpd(char * argv[]);
	int checkPID(std::string process);
	void closeSlattach();
	void closeTftp();
	void closeUftp();
	void closeUftpd();

private:
	int uftp_pid;
	int uftpd_pid;
	int tftp_pid;
	int slattach_pid;


};



#endif /* INCLUDE_INTERFACES_EXTERNALPROCESS_H_ */
