/*
 * ACSHandlers.cpp
 *
 *  Created on: Apr 29, 2011
 *      Author: Michael
 *
 *  Modified by: Conrad Hougen
 *  Last modified: 5/31/12
 *
 *  Modified by: Caleb Lipscomb
 *  Last modified 10/17/12
 */

#include "servers/ACSHandlers.h"
#include "servers/ACSStdTasks.h"
#include "servers/ACSServer.h"
#include "servers/DispatchStdTasks.h"

#include "core/CommandMessage.h"
#include "core/ReturnMessage.h"
#include "core/ErrorMessage.h"
#include "core/Singleton.h"
#include "core/Factory.h"
#include "core/Dispatcher.h"
#include "core/StdTypes.h"
#include "core/ModeManager.h"

//#include "boards/backplane/dbg_led.h"

using namespace std;
using namespace Phoenix::Core;
using namespace Phoenix::Servers;

uint32 ACSPointGNDHandler::enumArray[] = {VAR_TYPE_ENUM_FLOAT, VAR_TYPE_ENUM_FLOAT, VAR_TYPE_ENUM_FLOAT, VAR_TYPE_ENUM_UNSIGNED_INT};
uint32 ACSPointMRPHandler::enumArray[] = {VAR_TYPE_ENUM_FLOAT, VAR_TYPE_ENUM_FLOAT, VAR_TYPE_ENUM_FLOAT};
// uint32 ACSSetTargetHandler::enumArray[] = {VAR_TYPE_ENUM_FLOAT, VAR_TYPE_ENUM_FLOAT, VAR_TYPE_ENUM_FLOAT, VAR_TYPE_ENUM_UNSIGNED_INT};
// uint32 ACSRangeHandler::enumArray[] = {VAR_TYPE_ENUM_UNSIGNED_INT};
uint32 ACSPointNadirHandler::enumArray[] = {VAR_TYPE_ENUM_UNSIGNED_INT};
//uint32 ACSFunctionalTestHandler::enumArray[] = {VAR_TYPE_ENUM_INT};
uint32 ACSGPSHandler::enumArray[] = {VAR_TYPE_ENUM_FLOAT, VAR_TYPE_ENUM_FLOAT, VAR_TYPE_ENUM_FLOAT, VAR_TYPE_ENUM_FLOAT, 
		VAR_TYPE_ENUM_FLOAT, VAR_TYPE_ENUM_FLOAT, VAR_TYPE_ENUM_FLOAT, VAR_TYPE_ENUM_UNSIGNED_INT};		
uint32 ACSControlOrientationsHandler::enumArray[] = {VAR_TYPE_ENUM_UNSIGNED_INT, VAR_TYPE_ENUM_FLOAT, VAR_TYPE_ENUM_FLOAT, VAR_TYPE_ENUM_FLOAT,
	VAR_TYPE_ENUM_FLOAT, VAR_TYPE_ENUM_FLOAT, VAR_TYPE_ENUM_FLOAT, VAR_TYPE_ENUM_FLOAT, VAR_TYPE_ENUM_FLOAT, VAR_TYPE_ENUM_FLOAT};
uint32 ACSMOIWheelsHandler::enumArray[] = {VAR_TYPE_ENUM_FLOAT, VAR_TYPE_ENUM_FLOAT, VAR_TYPE_ENUM_FLOAT};
// uint32 ACSMOISatelliteHandler::enumArray[] = {VAR_TYPE_ENUM_FLOAT, VAR_TYPE_ENUM_FLOAT, VAR_TYPE_ENUM_FLOAT, VAR_TYPE_ENUM_FLOAT,
// 		VAR_TYPE_ENUM_FLOAT, VAR_TYPE_ENUM_FLOAT};
uint32 ACSTorqCoeffHandler::enumArray[] = {VAR_TYPE_ENUM_UNSIGNED_INT, VAR_TYPE_ENUM_FLOAT, VAR_TYPE_ENUM_FLOAT, VAR_TYPE_ENUM_FLOAT,
		VAR_TYPE_ENUM_FLOAT, VAR_TYPE_ENUM_FLOAT, VAR_TYPE_ENUM_FLOAT, VAR_TYPE_ENUM_FLOAT};
uint32 ACSOrbitEphemHandler::enumArray[] = {VAR_TYPE_ENUM_FLOAT, VAR_TYPE_ENUM_FLOAT, VAR_TYPE_ENUM_FLOAT, VAR_TYPE_ENUM_FLOAT,
	VAR_TYPE_ENUM_FLOAT};
uint32 ACSControllerGainsHandler::enumArray[] = {VAR_TYPE_ENUM_FLOAT, VAR_TYPE_ENUM_FLOAT, VAR_TYPE_ENUM_FLOAT, VAR_TYPE_ENUM_FLOAT,
	VAR_TYPE_ENUM_FLOAT, VAR_TYPE_ENUM_FLOAT, VAR_TYPE_ENUM_FLOAT, VAR_TYPE_ENUM_FLOAT, VAR_TYPE_ENUM_FLOAT, VAR_TYPE_ENUM_FLOAT,
	VAR_TYPE_ENUM_FLOAT};
uint32 ACSContolFreqHandler::enumArray[] = {VAR_TYPE_ENUM_FLOAT, VAR_TYPE_ENUM_FLOAT, VAR_TYPE_ENUM_FLOAT, VAR_TYPE_ENUM_FLOAT,
	VAR_TYPE_ENUM_FLOAT, VAR_TYPE_ENUM_FLOAT, VAR_TYPE_ENUM_FLOAT, VAR_TYPE_ENUM_UNSIGNED_INT, VAR_TYPE_ENUM_UNSIGNED_INT};
uint32 ACSStarCameraSettingsHandler::enumArray[] = {VAR_TYPE_ENUM_UNSIGNED_INT, VAR_TYPE_ENUM_UNSIGNED_INT, VAR_TYPE_ENUM_UNSIGNED_INT,
	VAR_TYPE_ENUM_UNSIGNED_INT, VAR_TYPE_ENUM_UNSIGNED_INT, VAR_TYPE_ENUM_FLOAT, VAR_TYPE_ENUM_FLOAT};
uint32 ACSRotationModeHandler::enumArray[] = {VAR_TYPE_ENUM_INT, VAR_TYPE_ENUM_INT, VAR_TYPE_ENUM_INT, VAR_TYPE_ENUM_INT};
uint32 ACSUpdateNotchFilterHandler::enumArray[] = {VAR_TYPE_ENUM_FLOAT, VAR_TYPE_ENUM_FLOAT, VAR_TYPE_ENUM_FLOAT, VAR_TYPE_ENUM_FLOAT};
uint32 ACSUpdateLeapSecondHandler::enumArray[] = {VAR_TYPE_ENUM_UNSIGNED_INT};
uint32 ACSGyroZRVHandler::enumArray[] = {VAR_TYPE_ENUM_FLOAT, VAR_TYPE_ENUM_FLOAT, VAR_TYPE_ENUM_FLOAT, VAR_TYPE_ENUM_FLOAT};
uint32 ACSDeguassProfileHandler::enumArray[] = {VAR_TYPE_ENUM_INT, VAR_TYPE_ENUM_INT, VAR_TYPE_ENUM_INT, VAR_TYPE_ENUM_INT, VAR_TYPE_ENUM_INT,
	VAR_TYPE_ENUM_INT, VAR_TYPE_ENUM_INT, VAR_TYPE_ENUM_INT, VAR_TYPE_ENUM_INT, VAR_TYPE_ENUM_INT, VAR_TYPE_ENUM_FLOAT};
uint32 ACSUpdateGNDCoordsHandler::enumArray[] = {VAR_TYPE_ENUM_FLOAT, VAR_TYPE_ENUM_FLOAT, VAR_TYPE_ENUM_FLOAT};
uint32 ACSUseGyroBiasHandler::enumArray[] = {VAR_TYPE_ENUM_UNSIGNED_INT};


ReturnMessage * ACSMeasurementHandler::Handle(const FSWPacket & packet)
{
	return (ACSHealthStatus());
}

ReturnMessage * ACSStateHandler::Handle(const FSWPacket & packet)
{
	return (ACSState());
}

ReturnMessage * ACSRawADCHandler::Handle(const FSWPacket & packet)
{
	return (ACSRawADC());
}

ReturnMessage * ACSAttitudeErrorHandler::Handle(const FSWPacket & packet)
{
	return (ACSAttitudeError());
}

ReturnMessage * ACSPointGNDHandler::Handle(const FSWPacket & packet)
{
	void * outputArray[numParams] = {NULL};
	if(!ExtractParameters(packet, enumArray, numParams, outputArray))
	{
		ErrorMessage err(ACS_POINT_GND_VECTOR_FAILURE);
		ReturnMessage * ret = new ReturnMessage(&err, false);
		return ret;
	}
	
	float x = * (float *) outputArray[0];	
	float y = * (float *) outputArray[1];
	float z = * (float *) outputArray[2];
	uint32 system = * (uint32 *) outputArray[3];

	return (ACSPointGND(x, y, z, ((ACSSystemPointEnum)system)));
}

ReturnMessage * ACSPointMRPHandler::Handle(const FSWPacket & packet)
{
	void * outputArray[numParams] = {NULL};
	if(!ExtractParameters(packet, enumArray, numParams, outputArray))
	{
		ErrorMessage err(ACS_POINT_MRP_VECTOR_FAILURE);
		ReturnMessage * ret = new ReturnMessage(&err, false);
		return ret;
	}
	
	float x = * (float *) outputArray[0];
	float y = * (float *) outputArray[1];
	float z = * (float *) outputArray[2];

	return (ACSPointMRP(x, y, z));
}

ReturnMessage * ACSRequestGNDHandler::Handle(const FSWPacket & packet)
{
	return (ACSRequestGND());
}

ReturnMessage * ACSRequestMRPHandler::Handle(const FSWPacket & packet)
{
	return (ACSRequestMRP());
}

ReturnMessage * ACSSunsoakHandler::Handle(const FSWPacket & packet)
{
	return (ACSSunSoak());
}

ReturnMessage * ACSPointNadirHandler::Handle(const FSWPacket & packet)
{
	void * outputArray[numParams] = {NULL};
	if(!ExtractParameters(packet, enumArray, numParams, outputArray))
	{
		ErrorMessage err(ACS_POINT_NADIR_FAILURE);
		ReturnMessage * ret = new ReturnMessage(&err, false);
		return ret;
	}
	
	uint32 system = * (uint32 *) outputArray[0];
	
	switch(system)
	{
		case COM_RF_ANTENNA:
			return (ACSPointNadir(COM_RF_ANTENNA));
		case THEIA_CAMERA:
			return (ACSPointNadir(THEIA_CAMERA));
		default:
			ErrorMessage err(ACS_POINT_NADIR_FAILURE);
			ReturnMessage * ret = new ReturnMessage(&err, false);
			return ret;
	}	
	
}

/*
ReturnMessage * ACSSetTargetHandler::Handle(const FSWPacket & packet)
{
	void * outputArray[numParams] = {NULL};
	if(!ExtractParameters(packet, enumArray, numParams, outputArray))
	{
		ErrorMessage err(ACS_SET_TARGET_FAILURE);
		ReturnMessage * ret = new ReturnMessage(&err, false);
		return ret;
	}
	
	float x = * (float *) outputArray[0];
	float y = * (float *) outputArray[1];
	float z = * (float *) outputArray[2];
	uint32 targetNum = * (uint32 *) outputArray[3];
	
	switch(targetNum)
	{
		case MODE_TARGET:
			return (ACSSetTarget(x, y, z, MODE_TARGET));
		case EVENT_TARGET:
			return (ACSSetTarget(x, y, z, EVENT_TARGET));
		default:
			ErrorMessage err(ACS_SET_TARGET_FAILURE);
			ReturnMessage * ret = new ReturnMessage(&err, false);
			return ret;
	}
}

ReturnMessage * ACSRangeHandler::Handle(const FSWPacket & packet)
{
	void * outputArray[numParams] = {NULL};
	if(!ExtractParameters(packet, enumArray, numParams, outputArray))
	{
		ErrorMessage err(ACS_RANGE_TO_TARGET_FAILURE);
		ReturnMessage * ret = new ReturnMessage(&err, false);
		return ret;
	}
	
	uint32 targetNum = * (uint32 *) outputArray[0];

	switch(targetNum)
	{
		case GND_STATION:
			return (ACSRangeToTarget(GND_STATION));
		case MODE_TARGET:
			return (ACSRangeToTarget(MODE_TARGET));
		case EVENT_TARGET:
			return (ACSRangeToTarget(EVENT_TARGET));
		default:
			ErrorMessage err(ACS_RANGE_TO_TARGET_FAILURE);
			ReturnMessage * ret = new ReturnMessage(&err, false);
			return ret;
	}
}
*/

ReturnMessage * ACSHoldHandler::Handle(const FSWPacket & packet)
{
	return (ACSHold());
}

ReturnMessage * ACSDisableHandler::Handle(const FSWPacket & packet)
{
	return (ACSDisable());
}

ReturnMessage * ACSResetHandler::Handle(const FSWPacket & packet)
{
	return (ACSReset());
}

ReturnMessage * ACSGPSHandler::Handle(const FSWPacket & packet)
{
	void * outputArray[numParams] = {NULL};
	if(!ExtractParameters(packet, enumArray, numParams, outputArray))
	{
		ErrorMessage err(ACS_GPS_FAILURE);
		ReturnMessage * ret = new ReturnMessage(&err, false);
		return ret;
	}

	float posX = * (float *) outputArray[0];
	float posY = * (float *) outputArray[1];
	float posZ = * (float *) outputArray[2];
	float velX = * (float *) outputArray[3];
	float velY = * (float *) outputArray[4];
	float velZ = * (float *) outputArray[5];
	float sec = * (float *) outputArray[6];
	uint32 week = * (uint32 *) outputArray[7];

	return (ACSSendGPS(posX, posY, posZ, velX, velY, velZ, sec, week));
}

ReturnMessage * ACSStarCameraHSHandler::Handle(const FSWPacket & packet)
{

	return (ACSStarCameraHS());
}


ReturnMessage * ACSPointComGndHandler::Handle(const FSWPacket & packet)
{
	return (ACSPointComGnd());
}


ReturnMessage * ACSHelloWorldTestHandler::Handle(const FSWPacket & packet)
{

	return (ACSHelloworld(packet));
}

// ReturnMessage * ACSFunctionalTestHandler::Handle(const FSWPacket & packet)
// {
// 	void * outputArray[numParams] = {NULL};
// 	if(!ExtractParameters(packet, enumArray, numParams, outputArray))
// 	{
// 		ErrorMessage err(ACS_FUNCTIONAL_TEST_FAILURE);
// 		ReturnMessage * ret = new ReturnMessage(&err, false);
// 		return ret;
// 	}
// 
// 	int32 testNum = * (int32 *) outputArray[0];
// 	
// 	return (ACSFunctionalTest(testNum));
// }
// 
// ReturnMessage * ACSFunctionalTestCompleteHandler::Handle(const FSWPacket & packet)
// {
// 	return (ACSFunctionalTestComplete());
// }


ReturnMessage * ACSControlOrientationsHandler::Handle(const FSWPacket & packet)
{
	void * outputArray[numParams] = {NULL};
	if(!ExtractParameters(packet, enumArray, numParams, outputArray))
	{
		ErrorMessage err(ACS_CTRL_ORIENT_FAILURE);
		ReturnMessage * ret = new ReturnMessage(&err, false);
		return ret;
	}

	uint32 selection = * (uint32 *) outputArray[0];
	float x1 = * (float *) outputArray[1];
	float x2 = * (float *) outputArray[2];
	float x3 = * (float *) outputArray[3];
	float y1 = * (float *) outputArray[4];
	float y2 = * (float *) outputArray[5];
	float y3 = * (float *) outputArray[6];
	float z1 = * (float *) outputArray[7];
	float z2 = * (float *) outputArray[8];
	float z3 = * (float *) outputArray[9];
	
	return(ACSControlOrientations(selection, x1, x2, x3, y1, y2, y3, z1, z2, z3));
}

ReturnMessage * ACSMOIWheelsHandler::Handle(const FSWPacket & packet)
{
	void * outputArray[numParams] = {NULL};
	if(!ExtractParameters(packet, enumArray, numParams, outputArray))
	{
		ErrorMessage err(ACS_MOI_WHEELS_FAILURE);
		ReturnMessage * ret = new ReturnMessage(&err, false);
		return ret;
	}

	float motorX = * (float *) outputArray[0];
	float motorY = * (float *) outputArray[1];
	float motorZ = * (float *) outputArray[2];
	
	return(ACSMOIWheels(motorX, motorY, motorZ));
}

// ReturnMessage * ACSMOISatelliteHandler::Handle(const FSWPacket & packet)
// {
// 	void * outputArray[numParams] = {NULL};
// 	if(!ExtractParameters(packet, enumArray, numParams, outputArray))
// 	{
// 		ErrorMessage err(ACS_MOI_SAT_FAILURE);
// 		ReturnMessage * ret = new ReturnMessage(&err, false);
// 		return ret;
// 	}
// 
// 	float I_11 = * (float *) outputArray[0];
// 	float I_12 = * (float *) outputArray[1];
// 	float I_13 = * (float *) outputArray[2];
// 	float I_22 = * (float *) outputArray[3];
// 	float I_23 = * (float *) outputArray[4];
// 	float I_33 = * (float *) outputArray[5];
// 	
// 	return(ACSMOISatellite(I_11, I_12, I_13, I_22, I_23, I_33));
// }

ReturnMessage * ACSTorqCoeffHandler::Handle(const FSWPacket & packet)
{
	void * outputArray[numParams] = {NULL};
	if(!ExtractParameters(packet, enumArray, numParams, outputArray))
	{
		ErrorMessage err(ACS_COEFF_TORQUER_FAILURE);
		ReturnMessage * ret = new ReturnMessage(&err, false);
		return ret;
	}
	
	uint32 rodNum = * (uint32 *) outputArray[0];
	float p1 = * (float *) outputArray[1];
	float p2 = * (float *) outputArray[2];
	float p3 = * (float *) outputArray[3];
	float p4 = * (float *) outputArray[4];
	float p5 = * (float *) outputArray[5];
	float p6 = * (float *) outputArray[6];
	float p7 = * (float *) outputArray[7];
	
	return(ACSTorqCoeff(rodNum, p1, p2, p3, p4, p5, p6, p7));
}

ReturnMessage * ACSOrbitEphemHandler::Handle(const FSWPacket & packet)
{
	void * outputArray[numParams] = {NULL};
	if(!ExtractParameters(packet, enumArray, numParams, outputArray))
	{
		ErrorMessage err(ACS_ORBIT_EPHEM_FAILURE);
		ReturnMessage * ret = new ReturnMessage(&err, false);
		return ret;
	}
	
	float a = * (float *) outputArray[0];
	float e = * (float *) outputArray[1];
	float i = * (float *) outputArray[2];
	float Omega = * (float *) outputArray[3];
	float omega = * (float *) outputArray[4];
	
	return(ACSOrbitEphem(a, e, i, Omega, omega));
}

ReturnMessage * ACSControllerGainsHandler::Handle(const FSWPacket & packet)
{
	void * outputArray[numParams] = {NULL};
	if(!ExtractParameters(packet, enumArray, numParams, outputArray))
	{
		ErrorMessage err(ACS_CTRL_LAW_GAINS_FAILURE);
		ReturnMessage * ret = new ReturnMessage(&err, false);
		return ret;
	}
	
	float K = * (float *) outputArray[0];
	float Px = * (float *) outputArray[1];
	float Py = * (float *) outputArray[2];
	float Pz = * (float *) outputArray[3];
	float Kix = * (float *) outputArray[4];
	float Kiy = * (float *) outputArray[5];
	float Kiz = * (float *) outputArray[6];
	float Kmx = * (float *) outputArray[7];
	float Kmy = * (float *) outputArray[8];
	float Kmz = * (float *) outputArray[9];
	float Kw = * (float *) outputArray[10];
	
	return(ACSControllerGains(K, Px, Py, Pz, Kix, Kiy, Kiz, Kmx, Kmy, Kmz, Kw));
}

ReturnMessage * ACSContolFreqHandler::Handle(const FSWPacket & packet)
{
	void * outputArray[numParams] = {NULL};
	if(!ExtractParameters(packet, enumArray, numParams, outputArray))
	{
		ErrorMessage err(ACS_CTRL_LAW_FREQ_FAILURE);
		ReturnMessage * ret = new ReturnMessage(&err, false);
		return ret;
	}
	
	float motor = * (float *) outputArray[0];
	float gyro = * (float *) outputArray[1];
	float magnetorquer = * (float *) outputArray[2];
	float starCamera = * (float *) outputArray[3];
	float magnetometerRate = * (float *) outputArray[4];
	float magnetometerSamples = * (float *) outputArray[5];
	float loopFreq = * (float *) outputArray[6];
	uint32 maxPWMTorq = * (uint32 *) outputArray[7];
	uint32 maxPWMWheels = * (uint32 *) outputArray[8];
	
	return(ACSContolFreq(motor, gyro, magnetorquer, starCamera, magnetometerRate, magnetometerSamples, loopFreq, maxPWMTorq, maxPWMWheels));
}

ReturnMessage * ACSStarCameraSettingsHandler::Handle(const FSWPacket & packet)
{
	void * outputArray[numParams] = {NULL};
	if(!ExtractParameters(packet, enumArray, numParams, outputArray))
	{
		ErrorMessage err(ACS_STARCAMERA_SETTINGS_FAILURE);
		ReturnMessage * ret = new ReturnMessage(&err, false);
		return ret;
	}

	uint32 digGain = * (uint32 *) outputArray[0];
	uint32 angGain = * (uint32 *) outputArray[1];
	uint32 exp = * (uint32 *) outputArray[2];
	uint32 thres = * (uint32 *) outputArray[3];
	uint32 maxBright = * (uint32 *) outputArray[4];
	float normalErr = * (float *) outputArray[5];
	float largeAngErr = * (float *) outputArray[6];
	
	return(ACSStarCameraSettings(digGain, angGain, exp, thres, maxBright, normalErr, largeAngErr));
}

ReturnMessage * ACSRotationModeHandler::Handle(const FSWPacket & packet)
{
	void * outputArray[numParams] = {NULL};
	if(!ExtractParameters(packet, enumArray, numParams, outputArray))
	{
		ErrorMessage err(ACS_UPDATE_ROTATION_MODE_FAILURE);
		ReturnMessage * ret = new ReturnMessage(&err, false);
		return ret;
	}

	int32 rot180 = * (int32 *) outputArray[0];
	int32 rot45 = * (int32 *) outputArray[1];
	int32 rotNeg90 = * (int32 *) outputArray[2];
	int32 rot90 = * (int32 *) outputArray[3];
	
	return(ACSSetRot(rot180, rot45, rotNeg90, rot90));
}

ReturnMessage * ACSUpdateNotchFilterHandler::Handle(const FSWPacket & packet)
{
	void * outputArray[numParams] = {NULL};
	if(!ExtractParameters(packet, enumArray, numParams, outputArray))
	{
		ErrorMessage err(ACS_NOTCH_FILTER_FAILURE);
		ReturnMessage * ret = new ReturnMessage(&err, false);
		return ret;
	}

	float magSampleRate = * (float *) outputArray[0];
	float bandwidth1 = * (float *) outputArray[1];
	float bandwidth2 = * (float *) outputArray[2];
	float bandwidth3 = * (float *) outputArray[3];
	
	return(ACSUpdateNotch(magSampleRate, bandwidth1, bandwidth2, bandwidth3));
}

ReturnMessage * ACSUpdateLeapSecondHandler::Handle(const FSWPacket & packet)
{
	void * outputArray[numParams] = {NULL};
	if(!ExtractParameters(packet, enumArray, numParams, outputArray))
	{
		ErrorMessage err(ACS_GPS_LEAP_SEC_FAILURE);
		ReturnMessage * ret = new ReturnMessage(&err, false);
		return ret;
	}

	uint32 leapSec = * (uint32 *) outputArray[0];
	
	return(ACSUpdateLeap(leapSec));
}

ReturnMessage * ACSGyroZRVHandler::Handle(const FSWPacket & packet)
{
	void * outputArray[numParams] = {NULL};
	if(!ExtractParameters(packet, enumArray, numParams, outputArray))
	{
		ErrorMessage err(ACS_GYRO_ZRV_FAILURE);
		ReturnMessage * ret = new ReturnMessage(&err, false);
		return ret;
	}

	float x = * (float *) outputArray[0];
	float y = * (float *) outputArray[1];
	float z = * (float *) outputArray[2];
	float noise = * (float *) outputArray[3];
	
	return(ACSGyroZRV(x, y, z, noise));
}

ReturnMessage * ACSDeguassProfileHandler::Handle(const FSWPacket & packet)
{
	void * outputArray[numParams] = {NULL};
	if(!ExtractParameters(packet, enumArray, numParams, outputArray))
	{
		ErrorMessage err(ACS_DEGAUSS_FAILURE);
		ReturnMessage * ret = new ReturnMessage(&err, false);
		return ret;
	}

	int32 x1 = * (int32 *) outputArray[0];
	int32 x2 = * (int32 *) outputArray[1];
	int32 x3 = * (int32 *) outputArray[2];
	int32 x4 = * (int32 *) outputArray[3];
	int32 x5 = * (int32 *) outputArray[4];
	int32 x6 = * (int32 *) outputArray[5];
	int32 x7 = * (int32 *) outputArray[6];
	int32 x8 = * (int32 *) outputArray[7];
	int32 x9 = * (int32 *) outputArray[8];
	int32 x10 = * (int32 *) outputArray[9];
	float ms = * (float *) outputArray[10];
	
	return(ACSDeguassProfile(x1, x2, x3, x4, x5, x6, x7, x8, x9, x10, ms));
}

ReturnMessage * ACSUpdateGNDCoordsHandler::Handle(const FSWPacket & packet)
{
	void * outputArray[numParams] = {NULL};
	if(!ExtractParameters(packet, enumArray, numParams, outputArray))
	{
		ErrorMessage err(ACS_UPDATE_GND_STATION_COORDS_FAILURE);
		ReturnMessage * ret = new ReturnMessage(&err, false);
		return ret;
	}

	float x = * (float *) outputArray[0];
	float y = * (float *) outputArray[1];
	float z = * (float *) outputArray[2];
	
	return(ACSUpdateGNDCorrds(x, y, z));
}

ReturnMessage * ACSUseGyroBiasHandler::Handle(const FSWPacket & packet)
{
	void * outputArray[numParams] = {NULL};
	if(!ExtractParameters(packet, enumArray, numParams, outputArray))
	{
		ErrorMessage err(ACS_GYRO_BIAS_FAILURE);
		ReturnMessage * ret = new ReturnMessage(&err, false);
		return ret;
	}

	uint32 bias = * (uint32 *) outputArray[0];
	
	return(ACSUseGyroBias(bias));
}

ReturnMessage * ACSErrorHandler::Handle(const FSWPacket & packet)
{
	//grab dispatcher instance, if it fails return DISPATCHER_NO_INSTANCE
	Dispatcher * dispatcher = dynamic_cast<Dispatcher *> (Factory::GetInstance(DISPATCHER_SINGLETON));

	FSWPacket * forward = new FSWPacket(packet);

	//forward error message to Error Octopus
	forward->SetDestination(SERVER_LOCATION_ERR);
	forward->SetSource(SERVER_LOCATION_ACS);

	//Dispatch packet, if it fails return DISPATCH_FAILED
	if(!dispatcher->Dispatch(*forward))
	{
			ErrorMessage err(DISPATCH_FAILED);
			ReturnMessage * eRet = new ReturnMessage(&err, false);
			delete forward;
			return eRet;
	}

	ReturnMessage retMsg;
	DispatcherStatusEnum stat;
	//Wait for return message, if it fails return status response from dispatcher
	if(DISPATCHER_STATUS_OK != (stat = dispatcher->WaitForDispatchResponse(*forward, retMsg)))
	{
			ErrorMessage err(DISPATCHER_STATUS_ERR);
			ReturnMessage * eRet = new ReturnMessage(&err, false);
			delete forward;
			return eRet;
	}

	delete forward;
	//Send server response message back to caller
	ReturnMessage * ret = new ReturnMessage(retMsg);
	//caller responsible for deleting the return message.
	return ret;
}

