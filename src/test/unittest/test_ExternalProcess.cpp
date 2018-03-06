/*
 * test_ExternalProcess.cpp
 *
 *  Created on: Jan 8, 2018
 *      Author: chuffman93
 */

#include "test/catch.hpp"
#include "interfaces/ExternalProcess.h"

#include<iostream>
using namespace std;



TEST_CASE("Test that ExternalProcess launches the process","[.][externalprocess]"){
	ExternalProcess slattach;
	char * argv[] = {(char *)"/sbin/slattach",(char *)"-L",(char *)"-m",(char *)"-s",(char *)"115200",(char *)"-p",(char *)"slip",(char *)"/dev/tty52", NULL};
	char * argc[] = {(char *)"/sbin/ifconfig",(char *)"sl0",(char *)"1.1.1.2",(char *)"pointopoint",(char *)"1.1.1.1",(char *)"mtu",(char *)"256",(char *)"multicast",(char *)"up",NULL};

	slattach.launchProcess(argv,argc);
	REQUIRE(slattach.checkPID() != -1);

	slattach.closeProcess();
	REQUIRE(slattach.checkPID() == -1);

}
