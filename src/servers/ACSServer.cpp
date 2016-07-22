/*
* ACSServer.cpp
*
*  Created on: Feb 21, 2011
*      Author: Michael
*  Modified by: Conrad Hougen
*  Last modified: 6/4/12
*/

#include "servers/ACSStdTasks.h"
#include "servers/ACSServer.h"
#include "servers/ACSHandlers.h"
#include "servers/DispatchStdTasks.h"
#include "servers/CDHServer.h"

#include "core/CommandMessage.h"
#include "core/ReturnMessage.h"
#include "core/Singleton.h"
#include "core/Factory.h"
#include "core/WatchdogManager.h"
#include "core/ErrorMessage.h"
#include "core/StdTypes.h"
#include "core/DataMessage.h"
#include "core/Dispatcher.h"

#include "util/FileHandler.h"
#include "util/Logger.h"

#define HARDWARE

using namespace std;
using namespace Phoenix::Core;

namespace Phoenix{
namespace Servers{

// Message handler for a measurement request.
static ACSMeasurementHandler * acsHSHandler;
// Message handler for a current state command.
static ACSStateHandler * acsStateHandler;
// Message handler for a raw ADC command.
static ACSRawADCHandler * acsADCHandler;
// Message handler for an error in attitude command.
static ACSAttitudeErrorHandler * acsAttHandler;
// Message handler for an Point MRP vector command.
static ACSPointMRPHandler * acsPntMRPHandler;
// Message handler for a Point GND vector command.
static ACSPointGNDHandler * acsPntGndHandler;
// Message handler for an Point MRP vector request.
static ACSRequestMRPHandler * acsRqMRPHandler;
// Message handler for a Point GND vector request.
static ACSRequestGNDHandler * acsRqGNDHandler;
// Message handler for a sunsoak command.
static ACSSunsoakHandler * acsSunsoakHandler;
// Message handler for a point nadir command.
static ACSPointNadirHandler * acsNadirHandler;
// Message handler for a set target command.
//static ACSSetTargetHandler * acsSetHandler;
// Message handler for a range command.
//static ACSRangeHandler * acsRangeHandler;
// Message handler for a hold command.
static ACSHoldHandler * acsHoldHandler;
// Message handler for a disable command.
static ACSDisableHandler * acsDisHandler;
// Message handler for a reset command.
static ACSResetHandler * acsRstHandler;
// Message handler for a GPS command.
static ACSGPSHandler * acsGpsHandler;
static ACSStarCameraHSHandler * acsLastLockHandler;
static ACSPointComGndHandler * acsPointComGndHandler;
static ACSHelloWorldTestHandler * acsHelloWorldTestHandler;
//static ACSFunctionalTestHandler * acsFunctionalTestHandler;
//static ACSFunctionalTestCompleteHandler * acsFunctionalTestCompleteHandler;

static ACSControlOrientationsHandler * acsControlOrientHandler;
static ACSMOIWheelsHandler * acsMoiWheelsHandler;
//static ACSMOISatelliteHandler * acsMoiSatHandler;
static ACSTorqCoeffHandler * acsTorqHandler;
static ACSOrbitEphemHandler * acsOrbitHandler;
static ACSControllerGainsHandler * acsCntrlGainHandler;
static ACSContolFreqHandler * acsCntrlFreqHandler;
static ACSStarCameraSettingsHandler * acsStarCamSetHandler;
static ACSRotationModeHandler * acsRotModeHandler;
static ACSUpdateNotchFilterHandler * acsNotchFilterHandler;
static ACSUpdateLeapSecondHandler * acsLeapSecHandler;
static ACSGyroZRVHandler * acsGyroZRVHandler;
static ACSDeguassProfileHandler * acsDegaussHandler;
static ACSUpdateGNDCoordsHandler * acsGNDCoordsHandler;
static ACSUseGyroBiasHandler * acsGyroBiasHandler;

// Message handler for an error.
static ACSErrorHandler * acsErrorHandler;

// -------------------------------------- Necessary Methods --------------------------------------
ACSServer::ACSServer(string nameIn, LocationIDType idIn)
		: SubsystemServer(nameIn, idIn), Singleton(), arby(idIn)
{
	// Left Intentionally Blank
}

ACSServer::~ACSServer()
{
	// Left Intentionally Blank
}

ACSServer & ACSServer::operator=(const ACSServer & source)
{
	if (this == &source)
	{
		return *this;
	}

	SubsystemServer::operator =(source);

	return *this;
}

void ACSServer::Initialize(void)
{
	//Initialize handlers
	acsHSHandler = new ACSMeasurementHandler();
	acsStateHandler = new ACSStateHandler();
	acsADCHandler = new ACSRawADCHandler();
	acsAttHandler = new ACSAttitudeErrorHandler();
	acsPntMRPHandler = new ACSPointMRPHandler();
	acsPntGndHandler = new ACSPointGNDHandler();
	acsRqMRPHandler = new ACSRequestMRPHandler();
	acsRqGNDHandler = new ACSRequestGNDHandler();
	acsSunsoakHandler = new ACSSunsoakHandler();
	acsNadirHandler = new ACSPointNadirHandler();
	//acsSetHandler = new ACSSetTargetHandler();
	//acsRangeHandler = new ACSRangeHandler();
	acsHoldHandler = new ACSHoldHandler();
	acsDisHandler = new ACSDisableHandler();
	acsRstHandler = new ACSResetHandler();
	acsGpsHandler = new ACSGPSHandler();
	acsLastLockHandler = new ACSStarCameraHSHandler();
	acsPointComGndHandler = new ACSPointComGndHandler();
	acsHelloWorldTestHandler = new ACSHelloWorldTestHandler();
	//acsFunctionalTestHandler = new ACSFunctionalTestHandler();
	//acsFunctionalTestCompleteHandler = new ACSFunctionalTestCompleteHandler();
	acsControlOrientHandler = new ACSControlOrientationsHandler();
	acsMoiWheelsHandler = new ACSMOIWheelsHandler();
	//acsMoiSatHandler = new ACSMOISatelliteHandler();
	acsTorqHandler = new ACSTorqCoeffHandler();
	acsOrbitHandler = new ACSOrbitEphemHandler();
	acsCntrlGainHandler = new ACSControllerGainsHandler();
	acsCntrlFreqHandler = new ACSContolFreqHandler();
	acsStarCamSetHandler = new ACSStarCameraSettingsHandler();
	acsRotModeHandler = new ACSRotationModeHandler();
	acsNotchFilterHandler = new ACSUpdateNotchFilterHandler();
	acsLeapSecHandler = new ACSUpdateLeapSecondHandler;
	acsGyroZRVHandler = new ACSGyroZRVHandler();
	acsDegaussHandler = new ACSDeguassProfileHandler();
	acsGNDCoordsHandler = new ACSUpdateGNDCoordsHandler();
	acsGyroBiasHandler = new ACSUseGyroBiasHandler();
	acsErrorHandler = new ACSErrorHandler();
}
#ifdef TEST
void ACSServer::Destroy(void)
{
	//delete handlers
	delete acsHSHandler;
	delete acsStateHandler;
	delete acsADCHandler;
	delete acsAttHandler;
	delete acsPntMRPHandler;
	delete acsPntGndHandler;
	delete acsRqMRPHandler;
	delete acsRqGNDHandler;
	delete acsSunsoakHandler;
	delete acsNadirHandler;
	//delete acsSetHandler;
	//delete acsRangeHandler;
	delete acsHoldHandler;
	delete acsDisHandler;
	delete acsRstHandler;
	delete acsGpsHandler;
	delete acsLastLockHandler;
	delete acsPointComGndHandler;
	//delete acsFunctionalTestHandler;
	//delete acsFunctionalTestCompleteHandler;
	delete acsControlOrientHandler;
	delete acsMoiWheelsHandler;
	//delete acsMoiSatHandler;
	delete acsTorqHandler;
	delete acsOrbitHandler;
	delete acsCntrlGainHandler;
	delete acsCntrlFreqHandler;
	delete acsStarCamSetHandler;
	delete acsRotModeHandler;
	delete acsNotchFilterHandler;
	delete acsLeapSecHandler;
	delete acsGyroZRVHandler;
	delete acsDegaussHandler;
	delete acsGNDCoordsHandler;
	delete acsGyroBiasHandler;
	delete acsErrorHandler;
}
#endif
bool ACSServer::IsFullyInitialized(void)
{
	return Singleton::IsFullyInitialized();
}

void ACSServer::Update(SystemModeEnum mode)
{
	/*// Test code creates a packet that gets handled by acsSlewHandler
	ACSSlewHandler * acsSlewHandler = new ACSSlewHandler();
	CommandMessage pktCmd;
	FSWPacket * packet = new FSWPacket(NULL, SERVER_LOCATION_ACS, 1, 0, &pktCmd);
	acsSlewHandler->Handle(*packet);
	delete packet;
	delete acsSlewHandler;
	*/


}

bool ACSServer::RegisterHandlers()
{
	bool success = true;
	Dispatcher * dispatcher = dynamic_cast<Dispatcher *> (Factory::GetInstance(DISPATCHER_SINGLETON));

	// ACS Command OpCodes
	success &= reg.RegisterHandler(MessageIdentifierType(MESSAGE_TYPE_COMMAND, ACS_HS_CMD), acsHSHandler);
	success &= reg.RegisterHandler(MessageIdentifierType(MESSAGE_TYPE_COMMAND, ACS_HOLD_CMD), acsHoldHandler);
	success &= reg.RegisterHandler(MessageIdentifierType(MESSAGE_TYPE_COMMAND, ACS_DISABLE_CMD), acsDisHandler);
	success &= reg.RegisterHandler(MessageIdentifierType(MESSAGE_TYPE_COMMAND, ACS_STATE_CMD), acsStateHandler);
	success &= reg.RegisterHandler(MessageIdentifierType(MESSAGE_TYPE_COMMAND, ACS_RAW_ADC_CMD), acsADCHandler);
	success &= reg.RegisterHandler(MessageIdentifierType(MESSAGE_TYPE_COMMAND, ACS_ATTITUDE_ERROR_REQUEST_CMD), acsAttHandler);
	success &= reg.RegisterHandler(MessageIdentifierType(MESSAGE_TYPE_COMMAND, ACS_POINT_MRP_VECTOR_CMD), acsPntMRPHandler);
	success &= reg.RegisterHandler(MessageIdentifierType(MESSAGE_TYPE_COMMAND, ACS_POINT_GND_VECTOR_CMD), acsPntGndHandler);
	success &= reg.RegisterHandler(MessageIdentifierType(MESSAGE_TYPE_COMMAND, ACS_REQUEST_MRP_VECTOR_CMD), acsRqMRPHandler);
	success &= reg.RegisterHandler(MessageIdentifierType(MESSAGE_TYPE_COMMAND, ACS_REQUEST_GND_VECTOR_CMD), acsRqGNDHandler);
	success &= reg.RegisterHandler(MessageIdentifierType(MESSAGE_TYPE_COMMAND, ACS_SUNSOAK_CMD), acsSunsoakHandler);
	success &= reg.RegisterHandler(MessageIdentifierType(MESSAGE_TYPE_COMMAND, ACS_POINT_NADIR_CMD), acsNadirHandler);
// 			success &= reg.RegisterHandler(MessageIdentifierType(MESSAGE_TYPE_COMMAND, ACS_SET_TARGET_CMD), acsSetHandler);
// 			success &= reg.RegisterHandler(MessageIdentifierType(MESSAGE_TYPE_COMMAND, ACS_RANGE_TO_TARGET_CMD), acsRangeHandler);
	success &= reg.RegisterHandler(MessageIdentifierType(MESSAGE_TYPE_COMMAND, ACS_GPS_CMD), acsGpsHandler);
	success &= reg.RegisterHandler(MessageIdentifierType(MESSAGE_TYPE_COMMAND, ACS_RESET_CMD), acsRstHandler);
	success &= reg.RegisterHandler(MessageIdentifierType(MESSAGE_TYPE_COMMAND, ACS_STARCAMERA_HS_CMD), acsLastLockHandler);
	success &= reg.RegisterHandler(MessageIdentifierType(MESSAGE_TYPE_COMMAND, ACS_POINT_COM_GND_CMD), acsPointComGndHandler);
	success &= reg.RegisterHandler(MessageIdentifierType(MESSAGE_TYPE_COMMAND, ACS_HELLO_WORLD_TEST_CMD), acsHelloWorldTestHandler);
//			success &= reg.RegisterHandler(MessageIdentifierType(MESSAGE_TYPE_COMMAND, ACS_FUNCTIONAL_TEST_CMD), acsFunctionalTestHandler);
// 			success &= reg.RegisterHandler(MessageIdentifierType(MESSAGE_TYPE_COMMAND, ACS_FUNCTIONAL_TEST_COMPLETE_CMD), acsFunctionalTestCompleteHandler);

	success &= reg.RegisterHandler(MessageIdentifierType(MESSAGE_TYPE_CONFIG, ACS_CTRL_ORIENT_CONFIG), acsControlOrientHandler);
	success &= reg.RegisterHandler(MessageIdentifierType(MESSAGE_TYPE_CONFIG, ACS_MOI_WHEELS_CONFIG), acsMoiWheelsHandler);
//			success &= reg.RegisterHandler(MessageIdentifierType(MESSAGE_TYPE_CONFIG, ACS_MOI_SAT_CONFIG), acsMoiSatHandler);
	success &= reg.RegisterHandler(MessageIdentifierType(MESSAGE_TYPE_CONFIG, ACS_COEFF_TORQUER_CONFIG), acsTorqHandler);
	success &= reg.RegisterHandler(MessageIdentifierType(MESSAGE_TYPE_CONFIG, ACS_ORBIT_EPHEM_CONFIG), acsOrbitHandler);
	success &= reg.RegisterHandler(MessageIdentifierType(MESSAGE_TYPE_CONFIG, ACS_CTRL_LAW_GAINS_CONFIG), acsCntrlGainHandler);
	success &= reg.RegisterHandler(MessageIdentifierType(MESSAGE_TYPE_CONFIG, ACS_CTRL_LAW_FREQ_CONFIG), acsCntrlFreqHandler);
	success &= reg.RegisterHandler(MessageIdentifierType(MESSAGE_TYPE_CONFIG, ACS_STARCAMERA_SETTINGS_CONFIG), acsStarCamSetHandler);
	success &= reg.RegisterHandler(MessageIdentifierType(MESSAGE_TYPE_CONFIG, ACS_UPDATE_ROTATION_MODE_CONFIG), acsRotModeHandler);
	success &= reg.RegisterHandler(MessageIdentifierType(MESSAGE_TYPE_CONFIG, ACS_NOTCH_FILTER_CONFIG), acsNotchFilterHandler);
	success &= reg.RegisterHandler(MessageIdentifierType(MESSAGE_TYPE_CONFIG, ACS_GPS_LEAP_SEC_CONFIG), acsLeapSecHandler);
	success &= reg.RegisterHandler(MessageIdentifierType(MESSAGE_TYPE_CONFIG, ACS_GYRO_ZRV_CONFIG), acsGyroZRVHandler);
	success &= reg.RegisterHandler(MessageIdentifierType(MESSAGE_TYPE_CONFIG, ACS_DEGAUSS_CONFIG), acsDegaussHandler);
	success &= reg.RegisterHandler(MessageIdentifierType(MESSAGE_TYPE_CONFIG, ACS_UPDATE_GND_STATION_COORDS_CONFIG), acsGNDCoordsHandler);
	success &= reg.RegisterHandler(MessageIdentifierType(MESSAGE_TYPE_CONFIG, ACS_GYRO_BIAS_CONFIG), acsGyroBiasHandler);

	for(int opcode = ACS_CMD_MIN; opcode < ACS_CMD_MAX; opcode++)
	{
		success &= arby.ModifyPermission(MessageIdentifierType(MESSAGE_TYPE_COMMAND, opcode), true);
	}

	for(int opcode = ACS_CONFIG_MIN; opcode < ACS_CONFIG_MAX; opcode++)
	{
		success &= arby.ModifyPermission(MessageIdentifierType(MESSAGE_TYPE_CONFIG, opcode), true);
	}

	for(int opcode = ACS_SUB_ERROR_MIN; opcode < ACS_SUB_ERROR_MAX; opcode++)
	{
		success &= reg.RegisterHandler(MessageIdentifierType(MESSAGE_TYPE_ERROR, opcode), acsErrorHandler);
		success &= arby.ModifyPermission(MessageIdentifierType(MESSAGE_TYPE_COMMAND, opcode), true);
	}

	success &= dispatcher->AddRegistry(id, &reg, &arby);

	return success;
}

// -------------------------------------------- State Machine --------------------------------------------
void ACSServer::loopInit(){
	CDHServer * cdhServer = dynamic_cast<CDHServer *> (Factory::GetInstance(CDH_SERVER_SINGLETON));
	printf("------------------ Turning ACS on\n");
	cdhServer->subPowerOn(HARDWARE_LOCATION_ACS);
	currentState = ST_DISABLED;
}
void ACSServer::loopDisabled(){
	ModeManager * modeManager = dynamic_cast<ModeManager*>(Factory::GetInstance(MODE_MANAGER_SINGLETON));

	if(modeManager->GetMode() == MODE_COM)
		currentState = ST_GND_START;

	if(modeManager->GetMode() == MODE_PLD_PRIORITY)
		currentState = ST_PLD_START;
}
void ACSServer::loopGNDStart(){
	currentState = ST_GND_POINTING;
}
void ACSServer::loopGNDPointing(){
	ModeManager * modeManager = dynamic_cast<ModeManager*>(Factory::GetInstance(MODE_MANAGER_SINGLETON));

	if(modeManager->GetMode() != MODE_COM)
		currentState = ST_GND_STOP;
}
void ACSServer::loopGNDStop(){
	currentState = ST_DISABLED;
}
void ACSServer::loopPLDStart(){
	printf("-------------------- PLD Start\n");
	currentState = ST_PLD_POINTING;
}
void ACSServer::loopPLDPointing(){
	ModeManager * modeManager = dynamic_cast<ModeManager*>(Factory::GetInstance(MODE_MANAGER_SINGLETON));

	usleep(1000000);
	printf("-------------------- PLD pointing\n");
	DispatchPacket(ACSSendGPS());
//	FSWPacket * send = new FSWPacket(SERVER_LOCATION_ACS, HARDWARE_LOCATION_ACS, ACS_HS_CMD, true, false, MESSAGE_TYPE_COMMAND);
//	DispatchPacket(send);
	usleep(2000000);

	if(modeManager->GetMode() != MODE_PLD_PRIORITY)
		currentState = ST_PLD_STOP;
}
void ACSServer::loopPLDStop(){
	currentState = ST_DISABLED;
}

} // End Namespace servers
} // End Namespace Phoenix
