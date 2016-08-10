/*
* CMDServer.cpp
*
*  Created on: Feb 12, 2013
*      Author: Caitlyn
*  Modified by:
*/

#include "servers/CMDHandlers.h"
#include "servers/CMDServer.h"
#include "servers/CMDStdTasks.h"
#include "servers/DispatchStdTasks.h"

#include "core/CommandMessage.h"
#include "core/ReturnMessage.h"
#include "core/Singleton.h"
#include "core/Factory.h"
#include "core/ErrorMessage.h"
#include "core/StdTypes.h"
#include "util/Logger.h"
#include "POSIX.h"

#include <iostream>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>

#include "core/WatchdogManager.h"
#include "util/FileHandler.h"

//#include "boards/backplane/dbg_led.h"

using namespace std;
using namespace Phoenix::Core;

namespace Phoenix{
namespace Servers{

//Message Handler for ACP Protocol Switches
static CMDSwitchProtocolHandler * cmdSwitchProtocolHandler;
int CMDServer::subsystem_acp_protocol[HARDWARE_LOCATION_MAX];

CMDServer::CMDServer(string nameIn, LocationIDType idIn)
		: SubsystemServer(nameIn, idIn), Singleton(), arby(idIn)
{
	for(int i = HARDWARE_LOCATION_MIN; i < HARDWARE_LOCATION_MAX; i++){
		subsystem_acp_protocol[i] = ACP_PROTOCOL_SPI;
	}
}

CMDServer::~CMDServer()
{
	// Left Intentionally Blank
}

void CMDServer::Initialize(void)
{
	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
	cmdSwitchProtocolHandler = new CMDSwitchProtocolHandler();
	char cmdString[100];

	//------------------------SERIAL PORT------------------------------
	char portname[20] = "/dev/ttyS2";
	struct termios port;
	int portfd = open(portname, O_RDWR | O_NOCTTY | O_NDELAY);
	if(portfd == -1){
		logger->Log("CMDServer: Failed to open serial port!", LOGGER_LEVEL_FATAL);
	}

	if(tcgetattr(portfd, &port) < 0) {
		logger->Log("CMDServer: Problem with initial port attributes.", LOGGER_LEVEL_ERROR);
	}

	port.c_iflag &= ~(IGNBRK | BRKINT | ICRNL | INLCR | PARMRK | INPCK | ISTRIP | IXON);
	port.c_cflag |= CLOCAL | CREAD | CS8;
	port.c_cflag &= ~(CSIZE | PARENB | CSTOPB);
	port.c_lflag &= ~(ECHO | ECHONL | ISIG);
	port.c_lflag |= ICANON;

	if(cfsetispeed(&port, B115200) < 0 || cfsetospeed(&port, B115200) < 0){
		logger->Log("GPSServer: Problem setting the baud rate!", LOGGER_LEVEL_FATAL);
	}

	if(tcsetattr(portfd, TCSANOW, &port) < 0){
		logger->Log("GPSServer: Problem setting port attributes!", LOGGER_LEVEL_ERROR);
	}

	//---------------------------SLIP------------------------------------
	sprintf(cmdString, "slattach -L -m -p slip \"/dev/ttyS2\" &");
	system(cmdString);
	printf("%s\n", cmdString);

	//--------------------------UFTP--------------------------------------
	/*
	 * -E     Only  allow incoming sessions if encryption is enabled.  Default
		      is to allow both encrypted and unencrypted sessions.
	   -T temp_dir
			  Temp directory in	 which	files  are  received,  then  moved  to
			  dest_dir	when  the  session is complete.	 If omitted, files are
			  received directly	 into  dest_dir.   Must	 reside	 on  the  same
			  filesystem as the destination directory.

			  The  -T option MUST be specified to allow the client to save the
			  state of failed file transfers that can be resumed later.

			  Not compatible -A or -t.	Not compatible with -D	when  multiple
			  destination  directories are specified.  Also, no incoming files
			  with an absolute path will be accepted if this option is	speci-
			  fied.

			  IMPORTANT: When full directories are received, the entire direc-
			  tory is moved at once to the destination directory, removing any
			  existing	file/directory.	 This means that if an existing direc-
			  tory in dest_dir is the same name as a directory	received  into
			  temp_dir,	 all  files  under the existing directory are deleted.
			  The -i option prevents by moving all files individually.
	   -I interface[,interface...]
			  Lists one or more interfaces to listen to multicast traffic  on.
			  Interfaces  can  be specified either by interface name, by host-
			  name, or by  IP.	 When  receiving  a  closed  group  membership
			  request,	the client will participate if any of these interfaces
			  matches an IP in the announcement.  When receiving an open group
			  membership  request,  the	 first interface listed is the one the
			  client will report back to the server.  This may not necessarily
			  be the interface that the ANNOUNCE was received on.  The default
			  is to listen on all active non-loopback interfaces.  NOTE: Since
			  Windows  doesn't	have  named  interfaces (not in the sense that
			  UNIX-like systems	 do),  only  hostnames	or  IP	addresses  are
			  accepted	on Windows.  If specifying by hostname or IP, may be a
			  mixture of IPv4 and IPv6 addresses, except on systems that don't
			  support dual mode sockets such as Windows XP.
	   -D dest_dir[,dest_dir...]
			  Destination directories for all received files.  When an	incom-
			  ing  file	 specifies  an absolute path, it must match one of the
			  destination directories, otherwise the file  will	 be  rejected.
			  Incoming	files  that  don't  specify  an	 absolute path will be
			  received into the	 first	destination  directory	in  the	 list.
			  Default is /tmp for UNIX-like systems, C:\temp for Windows.
	*/
	//TODO add encryption
	char tmpDir[20] = "~/tmp";
	char fileDir[20] = "~/dow";
	char prvKey[20] = "~/rsakey.pem";
	sprintf(cmdString, "./uftpd -T %s -I sl0 -D %s -k %s", tmpDir, fileDir, prvKey);
	system(cmdString);
}

#ifdef TEST
void CMDServer::Destroy(void)
{
	delete cmdSwitchProtocolHandler;
}
#endif

bool CMDServer::IsFullyInitialized(void)
{
	return(Singleton::IsFullyInitialized());
}

CMDServer & CMDServer::operator=(const CMDServer & source)
{
	if (this == &source)
	{
		return *this;
	}

	SubsystemServer::operator =(source);

	return *this;
}

void CMDServer::Update(SystemModeEnum mode)
{
	// Called by mode manager each time mode changes
	//* Ex: Needs to do things to close mode 1, enter mode 2
	//* Setup and tear down between modes



}

bool CMDServer::RegisterHandlers()
{
	bool success = true;

	Dispatcher * dispatcher = dynamic_cast<Dispatcher *> (Factory::GetInstance(DISPATCHER_SINGLETON));

	// CMD Command Opcode
	success &= reg.RegisterHandler(MessageIdentifierType(MESSAGE_TYPE_COMMAND, CMD_ACP_SWITCH), cmdSwitchProtocolHandler);
	success &= arby.ModifyPermission(MessageIdentifierType(MESSAGE_TYPE_COMMAND, CMD_ACP_SWITCH), true);

	/*
	for(int opcode = EPS_SUB_ERROR_MIN; opcode < EPS_SUB_ERROR_MAX; opcode++)
	{
		success &= reg.RegisterHandler(MessageIdentifierType(MESSAGE_TYPE_ERROR, opcode), epsErrorHandler);
		success &= arby.ModifyPermission(MessageIdentifierType(MESSAGE_TYPE_COMMAND, opcode), true);
	}
	*/


	success &= dispatcher->AddRegistry(id, &reg, &arby);

	return success;
}

void CMDServer::SubsystemLoop(void)
{
	FileHandler * fileHandler = dynamic_cast<FileHandler *> (Factory::GetInstance(FILE_HANDLER_SINGLETON));
	ModeManager * modeManager = dynamic_cast<ModeManager *> (Factory::GetInstance(MODE_MANAGER_SINGLETON));
	Dispatcher * dispatcher = dynamic_cast<Dispatcher *> (Factory::GetInstance(DISPATCHER_SINGLETON));
	//WatchdogManager * wdm = dynamic_cast<WatchdogManager *> (Factory::GetInstance(WATCHDOG_MANAGER_SINGLETON));

	SystemModeEnum mode;

	uint8 * readBuffer;
	size_t readSize = 0;

	CMDFiles[0] = (char *)"CMD_List_1.dat";	// ACS or ALL
	CMDFiles[1] = (char *)"CMD_List_2.dat"; // COM
	CMDFiles[2] = (char *)"CMD_List_3.dat"; // EPS
	CMDFiles[3] = (char *)"CMD_List_4.dat"; // GPS
	CMDFiles[4] = (char *)"CMD_List_5.dat"; // PLD

	//VariableTypeData str_hold("test_schedule.dat");
	//ReturnMessage * ret = DispatchPacket(SERVER_LOCATION_CMD, SERVER_LOCATION_SCH, 0, 1, MESSAGE_TYPE_COMMAND, SCH_BUILD_SCHEDULE_CMD, str_hold);

	while(1)
	{
		printf("Here");
		while(dispatcher->Listen(id));
		uint64_t LastTimeTick = getTimeInMillis();



		waitUntil(LastTimeTick, 1000);

	}
}

} // End Namespace Servers
} // End Namespace Phoenix
