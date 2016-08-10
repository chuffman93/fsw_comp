/*
 * COMServer.cpp
 *
 *  Created on: Feb 21, 2011
 *      Author: Michael
 */

#include "servers/COMServer.h"
#include "servers/COMHandlers.h"
#include "servers/COMStdTasks.h"
#include "servers/DispatchStdTasks.h"

#include "core/CommandMessage.h"
#include "core/ReturnMessage.h"
#include "core/Singleton.h"
#include "core/Factory.h"
#include "core/WatchdogManager.h"
#include "core/ErrorMessage.h"
#include "core/StdTypes.h"
#include "core/ModeManager.h"

#include "util/FileHandler.h"
#include "util/Logger.h"

//#include "boards/backplane/dbg_led.h"

/*extern "C"
{
	#include "util/eKermit.h"
	#include "util/CircularBuffer.h"
	#include "util/cdefs.h"
}*/

using namespace std;
using namespace AllStar::Core;

namespace AllStar
{
	namespace Servers
	{	
		/*! \brief COM message handlers. */
		static COMHSHandler * comHSHandler;
		static COMBeaconHandler * comBeacHandler;
		static COMDataSendHandler * comDataSendHandler;
		static COMLoginHandler * comLoginHandler;
		static COMLogoutHandler * comLogoutHandler;
		static COMDataReceiveHandler * comDataRecHandler;
		static COMResetHandler * comRstHandler;
		static COMErrorHandler * comErrorHandler;
		
		// -------------------------------------- Necessary Methods --------------------------------------
		COMServer::COMServer(string nameIn, LocationIDType idIn)
				: SubsystemServer(nameIn, idIn), Singleton(), arby(idIn), COMCurrentState(BEACON_STATE), parseFlag(false), kermitRunFlag(false), beaconData()
		{			
			/*fileInputBuffer = new char[IBUFLEN + 8];
			fileOutputBuffer = new char[OBUFLEN + 8];
			inputBuffer = CreateCircularBuffer(COM_INPUT_BUFFER_LENGTH);*/
			//SetState(BEACON_STATE);
		}

		COMServer::~COMServer()
		{
			delete fileInputBuffer;
			delete fileOutputBuffer;
			//DestroyCircularBuffer(inputBuffer);
		}

		COMServer & COMServer::operator=(const COMServer & source)
		{
			if (this == &source)
			{
				return *this;
			}

			SubsystemServer::operator =(source);

			return *this;
		}

		void COMServer::Initialize(void)
		{
			//Initialize handlers
#ifdef WIN32
			if(comHSHandler == NULL)
				comHSHandler = new COMHSHandler();
			if(comBeacHandler == NULL)
				comBeacHandler = new COMBeaconHandler();
			if(comDataSendHandler == NULL)
				comDataSendHandler = new COMDataSendHandler();
			if(comLoginHandler == NULL)
				comLoginHandler = new COMLoginHandler();
			if(comLogoutHandler == NULL)
				comLogoutHandler = new COMLogoutHandler();
			if(comDataRecHandler == NULL)
				comDataRecHandler = new COMDataReceiveHandler();
			if(comRstHandler == NULL)
				comRstHandler = new COMResetHandler();
			if(comErrorHandler == NULL)
				comErrorHandler = new COMErrorHandler();
#else
			comHSHandler = new COMHSHandler();
			comBeacHandler = new COMBeaconHandler();
			comDataSendHandler = new COMDataSendHandler();
			comLoginHandler = new COMLoginHandler();
			comLogoutHandler = new COMLogoutHandler();
			comDataRecHandler = new COMDataReceiveHandler();
			comRstHandler = new COMResetHandler();
			comErrorHandler = new COMErrorHandler();
#endif
		}
#ifdef TEST
		void COMServer::Destroy(void)
		{
			//delete handlers
			delete comHSHandler;
			delete comBeacHandler;
			delete comDataSendHandler;
			delete comLoginHandler;
			delete comLogoutHandler;
			delete comDataRecHandler;
			delete comRstHandler;
			delete comErrorHandler;
		}
#endif
		bool COMServer::IsFullyInitialized(void)
		{
			return(Singleton::IsFullyInitialized());
		}

		void COMServer::Update(SystemModeEnum mode)
		{
			//update mode stuff!
		}

        bool COMServer::RegisterHandlers()
        {
        	volatile bool success = true;
        	Dispatcher * dispatcher = dynamic_cast<Dispatcher *> (Factory::GetInstance(DISPATCHER_SINGLETON));	

        	// COM Command OpCodes
			success &= reg.RegisterHandler(MessageIdentifierType(MESSAGE_TYPE_COMMAND, COM_HS_CMD), comHSHandler);
        	success &= reg.RegisterHandler(MessageIdentifierType(MESSAGE_TYPE_COMMAND, COM_BEACON_CMD), comBeacHandler);
        	success &= reg.RegisterHandler(MessageIdentifierType(MESSAGE_TYPE_COMMAND, COM_DATA_SEND_CMD), comDataSendHandler);
			success &= reg.RegisterHandler(MessageIdentifierType(MESSAGE_TYPE_COMMAND, COM_LOGOUT_CMD), comLogoutHandler);
        	success &= reg.RegisterHandler(MessageIdentifierType(MESSAGE_TYPE_COMMAND, COM_RESET_CMD), comRstHandler);
        	
			success &= reg.RegisterHandler(MessageIdentifierType(MESSAGE_TYPE_COMMAND, COM_LOGIN_CMD), comLoginHandler);
			success &= reg.RegisterHandler(MessageIdentifierType(MESSAGE_TYPE_COMMAND, COM_GND_LOGGED_OUT_CMD), comLogoutHandler);
			success &= reg.RegisterHandler(MessageIdentifierType(MESSAGE_TYPE_ERROR, COM_CONNECTION_LOST_CMD), comLogoutHandler);
			success &= reg.RegisterHandler(MessageIdentifierType(MESSAGE_TYPE_DATA, COM_DATA_RECEIVE_CMD), comDataRecHandler);

        	success &= reg.RegisterHandler(MessageIdentifierType(MESSAGE_TYPE_ERROR, COM_PASSWORD_REFUSED_CMD), comErrorHandler);

        	// COM Command OpCodes permissions
			for(int opcode = COM_CMD_MIN; opcode < COM_CMD_MAX; opcode++)
			{
				success &= arby.ModifyPermission(MessageIdentifierType(MESSAGE_TYPE_COMMAND, opcode), true);
				success &= arby.ModifyPermission(MessageIdentifierType(MESSAGE_TYPE_DATA, opcode), true);
				success &= arby.ModifyPermission(MessageIdentifierType(MESSAGE_TYPE_ERROR, opcode), true);
			}
			
        	success &= arby.ModifyPermission(MessageIdentifierType(MESSAGE_TYPE_ERROR, COM_PASSWORD_REFUSED_CMD), true);

        	success &= dispatcher->AddRegistry(id, &reg, &arby);

        	return success;
        }

        // -------------------------------------------- State Machine ---------------------------------------------
		void COMServer::loopInit(){
			currentState = ST_IDLE;
		}

		void COMServer::loopIdle(){
			ModeManager * modeManager = dynamic_cast<ModeManager *>(Factory::GetInstance(MODE_MANAGER_SINGLETON));

			if(modeManager->GetMode() == MODE_COM){
				currentState = ST_COM_START;
			}
		}

		void COMServer::loopCOMStart(){
			currentState = ST_COM;
		}
		
		void COMServer::loopCOM(){
			ModeManager * modeManager = dynamic_cast<ModeManager *>(Factory::GetInstance(MODE_MANAGER_SINGLETON));

			if(modeManager->GetMode() != MODE_COM){
				currentState = ST_COM_STOP;
			}
		}

		void COMServer::loopCOMStop(){
			currentState = ST_IDLE;
		}
	}
}


