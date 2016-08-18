/*
 * WatchdogStdTasks.h
 *
 *  Created on: August 16, 2016
 *      Author: Alex St. Clair
 */

#ifndef _WATCHDOGSTDTASKS_H
#define _WATCHDOGSTDTASKS_H

#include "servers/SubsystemServer.h"
#include <string>

using namespace AllStar::Servers;
using namespace std;

namespace AllStar{
namespace Core{

void * runServer(void * args);

}
}

#endif // _WATCHDOGSTDTASKS_H
