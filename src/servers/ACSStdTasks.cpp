/*
 * ACSStdTasks.cpp
 * Written by: Conrad Hougen
 * Created: 7/10/12
 */

#include "servers/ACSStdTasks.h"
#include "servers/DispatchStdTasks.h"
#include "servers/ACSServer.h"

#include "core/ModeManager.h"
#include "core/Singleton.h"
#include "core/Factory.h"
#include "core/DataMessage.h"
#include "core/Dispatcher.h"

#include "util/FileHandler.h"

//#include "boards/backplane/dbg_led.h"

using namespace std;
using namespace Phoenix::Core;

namespace Phoenix
{
	namespace Servers
	{
		ReturnMessage * ACSHealthStatus(void)
		{
			return(DispatchPacket(SERVER_LOCATION_ACS, HARDWARE_LOCATION_ACS, 1, 0, MESSAGE_TYPE_COMMAND, ACS_HS_CMD));
		}

		ReturnMessage * ACSState(void)
		{
			return(DispatchPacket(SERVER_LOCATION_ACS, HARDWARE_LOCATION_ACS, 1, 0, MESSAGE_TYPE_COMMAND, ACS_STATE_CMD));
		}

		ReturnMessage * ACSRawADC(void)
		{
			return(DispatchPacket(SERVER_LOCATION_ACS, HARDWARE_LOCATION_ACS, 1, 0, MESSAGE_TYPE_COMMAND, ACS_RAW_ADC_CMD));
		}
		
		ReturnMessage * ACSAttitudeError(void)
		{
			return(DispatchPacket(SERVER_LOCATION_ACS, HARDWARE_LOCATION_ACS, 1, 0, MESSAGE_TYPE_COMMAND, ACS_ATTITUDE_ERROR_REQUEST_CMD));
		}

		ReturnMessage * ACSPointMRP(const float & x, const float & y, const float & z)
		{
			VariableTypeData x_hold(x);
			VariableTypeData y_hold(y);
			VariableTypeData z_hold(z);

			list<VariableTypeData *> params;
			params.push_back(&x_hold);
			params.push_back(&y_hold);
			params.push_back(&z_hold);
			
			return(DispatchPacket(SERVER_LOCATION_ACS, HARDWARE_LOCATION_ACS, 1, 0, MESSAGE_TYPE_COMMAND, ACS_POINT_MRP_VECTOR_CMD, params));
		}

		ReturnMessage * ACSPointGND(const float & x, const float & y, const float & z, const ACSSystemPointEnum & system)
		{
			VariableTypeData x_hold(x);
			VariableTypeData y_hold(y);
			VariableTypeData z_hold(z);
			VariableTypeData sys_hold(((uint32) system));

			list<VariableTypeData *> params;
			params.push_back(&x_hold);
			params.push_back(&y_hold);
			params.push_back(&z_hold);
			params.push_back(&sys_hold);

			return(DispatchPacket(SERVER_LOCATION_ACS, HARDWARE_LOCATION_ACS, 1, 0, MESSAGE_TYPE_COMMAND, ACS_POINT_GND_VECTOR_CMD, params));
		}

		ReturnMessage * ACSRequestMRP(void)
		{
			return(DispatchPacket(SERVER_LOCATION_ACS, HARDWARE_LOCATION_ACS, 1, 0, MESSAGE_TYPE_COMMAND, ACS_REQUEST_MRP_VECTOR_CMD));
		}

		ReturnMessage * ACSRequestGND(void)
		{
			return(DispatchPacket(SERVER_LOCATION_ACS, HARDWARE_LOCATION_ACS, 1, 0, MESSAGE_TYPE_COMMAND, ACS_REQUEST_GND_VECTOR_CMD));
		}

		ReturnMessage * ACSSunSoak(void)
		{
			return(DispatchPacket(SERVER_LOCATION_ACS, HARDWARE_LOCATION_ACS, 1, 0, MESSAGE_TYPE_COMMAND, ACS_SUNSOAK_CMD));
		}

		ReturnMessage * ACSPointNadir(const ACSSystemPointEnum & system)
		{
			VariableTypeData sys_hold(static_cast<uint32> (system));
			return(DispatchPacket(SERVER_LOCATION_ACS, HARDWARE_LOCATION_ACS, 1, 0, MESSAGE_TYPE_COMMAND, ACS_POINT_NADIR_CMD, sys_hold));
		}

// 		ReturnMessage * ACSSetTarget(const float & x, const float & y, const float & z, const SchedulerTargetEnum & targetNum)
// 		{
// 			//Target ranges will now be taken care of by CDH not ACS, due to ACS being shut off to conserver power
//
// 			VariableTypeData x_hold(x);
// 			VariableTypeData y_hold(y);
// 			VariableTypeData z_hold(z);
// 			VariableTypeData num_hold(static_cast<uint32> (targetNum));
//
// 			list<VariableTypeData *> params;
// 			params.push_back(&x_hold);
// 			params.push_back(&y_hold);
// 			params.push_back(&z_hold);
// 			params.push_back(&num_hold);
//
// 			return(DispatchPacket(SERVER_LOCATION_ACS, HARDWARE_LOCATION_ACS, 1, 0, MESSAGE_TYPE_COMMAND, ACS_SET_TARGET_CMD, params));
//
// 			ACSServer * acsServer = dynamic_cast<ACSServer *> (Factory::GetInstance(ACS_SERVER_SINGLETON));
// 			if(!acsServer->SetTargetPostion(targetNum, x, y, z))
// 			{
// 				ErrorMessage err(ACS_SET_TARGET_FAILURE);
// 				ReturnMessage * ret = new ReturnMessage(&err, false);
// 				return ret;
// 			}
// 			DataMessage dat(ACS_SET_TARGET_SUCCESS);
// 			ReturnMessage * ret = new ReturnMessage(&dat, false);
// 			return ret;
// 		}

// 		ReturnMessage * ACSRangeToTarget(const SchedulerTargetEnum & targetNum)
// 		{
// 			//Target ranges will now be taken care of by CDH not ACS, due to ACS being shut off to conserver power
// 			/*
// 			VariableTypeData num_hold(static_cast<uint32> (targetNum));
// 			return(DispatchPacket(SERVER_LOCATION_ACS, HARDWARE_LOCATION_ACS, 1, 0, MESSAGE_TYPE_COMMAND, ACS_RANGE_TO_TARGET_CMD, num_hold));
// 			*/
// 			ACSServer * acsServer = dynamic_cast<ACSServer *> (Factory::GetInstance(ACS_SERVER_SINGLETON));
// 			float * position = acsServer->GetTargetPostion(targetNum);
// 			if((position[0] == 0) && (position[1] == 0) && (position[2] == 0))
// 			{
// 				ErrorMessage err(ACS_RANGE_TO_TARGET_FAILURE);
// 				ReturnMessage * ret = new ReturnMessage(&err, false);
// 				return ret;
// 			}
// 			VariableTypeData posX_hold(position[0]);
// 			VariableTypeData posY_hold(position[1]);
// 			VariableTypeData posZ_hold(position[2]);
//
// 			list<VariableTypeData *> params;
// 			params.push_back(&posX_hold);
// 			params.push_back(&posY_hold);
// 			params.push_back(&posZ_hold);
//
// 			DataMessage dat(ACS_RANGE_TO_TARGET_SUCCESS, params);
// 			ReturnMessage * ret = new ReturnMessage(&dat, false);
// 			return ret;
// 		}

		ReturnMessage * ACSHold(void)
		{
			return(DispatchPacket(SERVER_LOCATION_ACS, HARDWARE_LOCATION_ACS, 1, 0, MESSAGE_TYPE_COMMAND, ACS_HOLD_CMD));
		}

		ReturnMessage * ACSDisable(void)
		{
			return(DispatchPacket(SERVER_LOCATION_ACS, HARDWARE_LOCATION_ACS, 1, 0, MESSAGE_TYPE_COMMAND, ACS_DISABLE_CMD));
		}

		ReturnMessage * ACSReset(void)
		{
			return(DispatchPacket(SERVER_LOCATION_ACS, HARDWARE_LOCATION_ACS, 1, 0, MESSAGE_TYPE_COMMAND, ACS_RESET_CMD));
		}

		ReturnMessage * ACSSendGPS(const float & posX, const float & posY, const float & posZ, const float & velX, const float & velY, const float & velZ,
					const float & seconds, const uint32 & week)
		{
			VariableTypeData posX_hold(posX);
			VariableTypeData posY_hold(posY);
			VariableTypeData posZ_hold(posZ);
			VariableTypeData velX_hold(velX);
			VariableTypeData velY_hold(velY);
			VariableTypeData velZ_hold(velZ);
			VariableTypeData sec_hold(seconds);
			VariableTypeData week_hold(week);

			list<VariableTypeData *> params;
			params.push_back(&posX_hold);
			params.push_back(&posY_hold);
			params.push_back(&posZ_hold);
			params.push_back(&velX_hold);
			params.push_back(&velY_hold);
			params.push_back(&velZ_hold);
			params.push_back(&sec_hold);
			params.push_back(&week_hold);

			return(DispatchPacket(SERVER_LOCATION_ACS, HARDWARE_LOCATION_ACS, 1, 0, MESSAGE_TYPE_COMMAND, ACS_GPS_CMD, params));
		}
		
		ReturnMessage * ACSStarCameraHS(void)
		{
			return(DispatchPacket(SERVER_LOCATION_ACS, HARDWARE_LOCATION_ACS, 1, 0, MESSAGE_TYPE_COMMAND, ACS_STARCAMERA_HS_CMD));
		}
		
		ReturnMessage * ACSPointComGnd(void)
		{
			return(DispatchPacket(SERVER_LOCATION_ACS, HARDWARE_LOCATION_ACS, 1, 0, MESSAGE_TYPE_COMMAND, ACS_POINT_COM_GND_CMD));
		}
		
		ReturnMessage * ACSHelloworld(const Phoenix::Core::FSWPacket & packet)
		{
			//grab dispatcher instance, if it fails return DISPATCHER_NO_INSTANCE
			Dispatcher * dispatcher = dynamic_cast<Dispatcher *> (Factory::GetInstance(DISPATCHER_SINGLETON));

			FSWPacket * forward = new FSWPacket(packet);

			//forward error message to Error Octopus
			forward->SetDestination(HARDWARE_LOCATION_ACS);
			forward->SetSource(SERVER_LOCATION_ACS);

			//Dispatch packet, if it fails return DISPATCH_FAILED
			if(!dispatcher->Dispatch(*forward))
			{
					ErrorMessage err(DISPATCH_FAILED);
					ReturnMessage * eRet = new ReturnMessage(&err, false);
					delete forward;
					return eRet;
			}
			printf("\r\nACS Helloworld message sent\n\r");
		}


 		ReturnMessage * ACSFunctionalTest(const int32 & testNum)
 		{
 			VariableTypeData test_hold(testNum);
 			return(DispatchPacket(SERVER_LOCATION_ACS, HARDWARE_LOCATION_ACS, 1, 0, MESSAGE_TYPE_COMMAND, ACS_FUNCTIONAL_TEST_CMD, test_hold));
 		}

// 		ReturnMessage * ACSFunctionalTestComplete(void)
// 		{
// 			ACSServer * acsServer = dynamic_cast<ACSServer *>(Factory::GetInstance(ACS_SERVER_SINGLETON));
// 			acsServer->SetRunTest(false);
// 			DataMessage dat(ACS_FUNCTIONAL_TEST_COMPLETE_SUCCESS);
// 			ReturnMessage * ret = new ReturnMessage(&dat, true);
// 			usleep(5000000);
// 			return(ret);
// 		}
		
		ReturnMessage * ACSControlOrientations(const uint32 & selection, const float & x1, const float & x2, const float & x3,
			const float & y1, const float & y2, const float & y3, const float & z1, const float & z2, const float & z3)
		{
			VariableTypeData sel_hold(selection);
			VariableTypeData x1_hold(x1);
			VariableTypeData x2_hold(x2);
			VariableTypeData x3_hold(x3);
			VariableTypeData y1_hold(y1);
			VariableTypeData y2_hold(y2);
			VariableTypeData y3_hold(y3);
			VariableTypeData z1_hold(z1);
			VariableTypeData z2_hold(z2);
			VariableTypeData z3_hold(z3);

			list<VariableTypeData *> params;
			params.push_back(&sel_hold);
			params.push_back(&x1_hold);
			params.push_back(&x2_hold);
			params.push_back(&x3_hold);
			params.push_back(&y1_hold);
			params.push_back(&y2_hold);
			params.push_back(&y3_hold);
			params.push_back(&z1_hold);
			params.push_back(&z2_hold);
			params.push_back(&z3_hold);
				
			return(DispatchPacket(SERVER_LOCATION_ACS, HARDWARE_LOCATION_ACS, 1, 0, MESSAGE_TYPE_CONFIG, ACS_CTRL_ORIENT_CONFIG, params));
		}
		ReturnMessage * ACSMOIWheels(const float & motorX, const float & motorY, const float & motorZ)
		{
			VariableTypeData motorX_hold(motorX);
			VariableTypeData motorY_hold(motorY);
			VariableTypeData motorZ_hold(motorZ);

			list<VariableTypeData *> params;
			params.push_back(&motorX_hold);
			params.push_back(&motorY_hold);
			params.push_back(&motorZ_hold);
			
			return(DispatchPacket(SERVER_LOCATION_ACS, HARDWARE_LOCATION_ACS, 1, 0, MESSAGE_TYPE_CONFIG, ACS_MOI_WHEELS_CONFIG, params));
		}
 		ReturnMessage * ACSMOISatellite(const float & I_11, const float & I_12, const float & I_13,
 				const float & I_22, const float & I_23, const float & I_33)
 		{
 			VariableTypeData I_11_hold(I_11);
 			VariableTypeData I_12_hold(I_12);
 			VariableTypeData I_13_hold(I_13);
 			VariableTypeData I_22_hold(I_22);
 			VariableTypeData I_23_hold(I_23);
 			VariableTypeData I_33_hold(I_33);

 			list<VariableTypeData *> params;
 			params.push_back(&I_11_hold);
 			params.push_back(&I_12_hold);
 			params.push_back(&I_13_hold);
 			params.push_back(&I_22_hold);
 			params.push_back(&I_23_hold);
 			params.push_back(&I_33_hold);

 			return(DispatchPacket(SERVER_LOCATION_ACS, HARDWARE_LOCATION_ACS, 1, 0, MESSAGE_TYPE_CONFIG, ACS_MOI_SAT_CONFIG, params));
 		}
		
		ReturnMessage * ACSTorqCoeff(const uint32 & rodNumber, const float & p1, const float & p2, const float & p3, const float & p4,
				const float & p5, const float & p6, const float & p7)
		{
			VariableTypeData rod_hold(rodNumber);
			VariableTypeData p1_hold(p1);
			VariableTypeData p2_hold(p2);
			VariableTypeData p3_hold(p3);
			VariableTypeData p4_hold(p4);
			VariableTypeData p5_hold(p5);
			VariableTypeData p6_hold(p6);
			VariableTypeData p7_hold(p7);

			list<VariableTypeData *> params;
			params.push_back(&rod_hold);
			params.push_back(&p1_hold);
			params.push_back(&p2_hold);
			params.push_back(&p3_hold);
			params.push_back(&p4_hold);
			params.push_back(&p5_hold);
			params.push_back(&p6_hold);
			params.push_back(&p7_hold);
			
			return(DispatchPacket(SERVER_LOCATION_ACS, HARDWARE_LOCATION_ACS, 1, 0, MESSAGE_TYPE_CONFIG, ACS_COEFF_TORQUER_CONFIG, params));
		}
		
		ReturnMessage * ACSOrbitEphem(const float & a, const float & e, const float & i, const float & Omega,
				const float & omega)
		{
			VariableTypeData a_hold(a);
			VariableTypeData e_hold(e);
			VariableTypeData i_hold(i);
			VariableTypeData Omega_hold(Omega);
			VariableTypeData omega_hold(omega);

			list<VariableTypeData *> params;
			params.push_back(&a_hold);
			params.push_back(&e_hold);
			params.push_back(&i_hold);
			params.push_back(&Omega_hold);
			params.push_back(&omega_hold);
			
			return(DispatchPacket(SERVER_LOCATION_ACS, HARDWARE_LOCATION_ACS, 1, 0, MESSAGE_TYPE_CONFIG, ACS_ORBIT_EPHEM_CONFIG, params));
		}
		
		ReturnMessage * ACSControllerGains(const float & K, const float & Px, const float & Py, const float & Pz, const float & Kix,
			const float & Kiy, const float & Kiz, const float & Kmx, const float & Kmy, const float & Kmz, const float & Kw)
		{
			VariableTypeData K_hold(K);
			VariableTypeData Px_hold(Px);
			VariableTypeData Py_hold(Py);
			VariableTypeData Pz_hold(Pz);
			VariableTypeData Kix_hold(Kix);
			VariableTypeData Kiy_hold(Kiy);
			VariableTypeData Kiz_hold(Kiz);
			VariableTypeData Kmx_hold(Kmx);
			VariableTypeData Kmy_hold(Kmy);
			VariableTypeData Kmz_hold(Kmz);
			VariableTypeData Kw_hold(Kw);

			list<VariableTypeData *> params;
			params.push_back(&K_hold);
			params.push_back(&Px_hold);
			params.push_back(&Py_hold);
			params.push_back(&Pz_hold);
			params.push_back(&Kix_hold);
			params.push_back(&Kiy_hold);
			params.push_back(&Kiz_hold);
			params.push_back(&Kmx_hold);
			params.push_back(&Kmy_hold);
			params.push_back(&Kmz_hold);
			params.push_back(&Kw_hold);
			
			return(DispatchPacket(SERVER_LOCATION_ACS, HARDWARE_LOCATION_ACS, 1, 0, MESSAGE_TYPE_CONFIG, ACS_CTRL_LAW_GAINS_CONFIG, params));
		}
		
		ReturnMessage * ACSContolFreq(const float & motor, const float & gyro, const float & magnetorquer,
			const float & starCamera, const float & magnetometerRate, const float & magnetometerSamples, const float & loopFreq,
			const uint32 & maxPWMTorq, const uint32 & maxPWMWheels)
		{
			VariableTypeData motor_hold(motor);
			VariableTypeData gyro_hold(gyro);
			VariableTypeData magtor_hold(magnetorquer);
			VariableTypeData star_hold(starCamera);
			VariableTypeData magtom1_hold(magnetometerRate);
			VariableTypeData magtom2_hold(magnetometerSamples);
			VariableTypeData loop_hold(loopFreq);
			VariableTypeData torq_hold(maxPWMTorq);
			VariableTypeData wheel_hold(maxPWMWheels);

			list<VariableTypeData *> params;
			params.push_back(&motor_hold);
			params.push_back(&gyro_hold);
			params.push_back(&magtor_hold);
			params.push_back(&star_hold);
			params.push_back(&magtom1_hold);
			params.push_back(&magtom2_hold);
			params.push_back(&loop_hold);
			params.push_back(&torq_hold);
			params.push_back(&wheel_hold);
			
			return(DispatchPacket(SERVER_LOCATION_ACS, HARDWARE_LOCATION_ACS, 1, 0, MESSAGE_TYPE_CONFIG, ACS_CTRL_LAW_FREQ_CONFIG, params));
		}
		
		ReturnMessage * ACSStarCameraSettings(const uint32 & digGain, const uint32 & angGain, const uint32 & exposure,
			const uint32 & threshold, const uint32 & maxBrightness, const float & normalErrRange, const float & largeAngleErrRange)
		{
			VariableTypeData digGain_hold(digGain);
			VariableTypeData angGain_hold(angGain);
			VariableTypeData exp_hold(exposure);
			VariableTypeData thres_hold(threshold);
			VariableTypeData mxBright_hold(maxBrightness);
			VariableTypeData nmlErr_hold(normalErrRange);
			VariableTypeData lrdErr_hold(largeAngleErrRange);

			list<VariableTypeData *> params;
			params.push_back(&digGain_hold);
			params.push_back(&angGain_hold);
			params.push_back(&exp_hold);
			params.push_back(&thres_hold);
			params.push_back(&mxBright_hold);
			params.push_back(&nmlErr_hold);
			params.push_back(&lrdErr_hold);
			
			return(DispatchPacket(SERVER_LOCATION_ACS, HARDWARE_LOCATION_ACS, 1, 0, MESSAGE_TYPE_CONFIG, ACS_STARCAMERA_SETTINGS_CONFIG, params));
		}
		
		ReturnMessage * ACSSetRot(const int32 & rot180, const int32 & rot45, const int32 & rotNeg90, const int32 & rot90)
		{
			VariableTypeData rot180_hold(rot180);
			VariableTypeData rot45_hold(rot45);
			VariableTypeData rotNeg90_hold(rotNeg90);
			VariableTypeData rot90_hold(rot90);

			list<VariableTypeData *> params;
			params.push_back(&rot180_hold);
			params.push_back(&rot45_hold);
			params.push_back(&rotNeg90_hold);
			params.push_back(&rot90_hold);
			
			return(DispatchPacket(SERVER_LOCATION_ACS, HARDWARE_LOCATION_ACS, 1, 0, MESSAGE_TYPE_CONFIG, ACS_UPDATE_ROTATION_MODE_CONFIG, params));
		}
		
		ReturnMessage * ACSUpdateNotch(const float & magSampleRate, const float & bandwidth1, const float & bandwidth2,
			const float & bandwidth3)
		{
			VariableTypeData magSampleRate_hold(magSampleRate);
			VariableTypeData band1_hold(bandwidth1);
			VariableTypeData band2_hold(bandwidth2);
			VariableTypeData band3_hold(bandwidth3);

			list<VariableTypeData *> params;
			params.push_back(&magSampleRate_hold);
			params.push_back(&band1_hold);
			params.push_back(&band2_hold);
			params.push_back(&band3_hold);
			
			return(DispatchPacket(SERVER_LOCATION_ACS, HARDWARE_LOCATION_ACS, 1, 0, MESSAGE_TYPE_CONFIG, ACS_NOTCH_FILTER_CONFIG, params));
		}
		
		ReturnMessage * ACSUpdateLeap(const uint32 & leapSec)
		{
			VariableTypeData leapSec_hold(leapSec);

			list<VariableTypeData *> params;
			params.push_back(&leapSec_hold);
			
			return(DispatchPacket(SERVER_LOCATION_ACS, HARDWARE_LOCATION_ACS, 1, 0, MESSAGE_TYPE_CONFIG, ACS_GPS_LEAP_SEC_CONFIG, params));
		}
		
		ReturnMessage * ACSGyroZRV(const float & x, const float & y, const float & z, const float & noise)
		{
			VariableTypeData x_hold(x);
			VariableTypeData y_hold(y);
			VariableTypeData z_hold(z);
			VariableTypeData noise_hold(noise);

			list<VariableTypeData *> params;
			params.push_back(&x_hold);
			params.push_back(&y_hold);
			params.push_back(&z_hold);
			params.push_back(&noise_hold);
			
			return(DispatchPacket(SERVER_LOCATION_ACS, HARDWARE_LOCATION_ACS, 1, 0, MESSAGE_TYPE_CONFIG, ACS_GYRO_ZRV_CONFIG, params));
		}
		
		ReturnMessage * ACSDeguassProfile(const int32 & x1, const int32 & x2, const int32 & x3, const int32 & x4,
			const int32 & x5, const int32 & x6, const int32 & x7, const int32 & x8, const int32 & x9, const int32 & x10, const float & ms)
		{
			VariableTypeData x1_hold(x1);
			VariableTypeData x2_hold(x2);
			VariableTypeData x3_hold(x3);
			VariableTypeData x4_hold(x4);
			VariableTypeData x5_hold(x5);
			VariableTypeData x6_hold(x6);
			VariableTypeData x7_hold(x7);
			VariableTypeData x8_hold(x8);
			VariableTypeData x9_hold(x9);
			VariableTypeData x10_hold(x10);
			VariableTypeData ms_hold(ms);

			list<VariableTypeData *> params;
			params.push_back(&x1_hold);
			params.push_back(&x2_hold);
			params.push_back(&x3_hold);
			params.push_back(&x4_hold);
			params.push_back(&x5_hold);
			params.push_back(&x6_hold);
			params.push_back(&x7_hold);
			params.push_back(&x8_hold);
			params.push_back(&x9_hold);
			params.push_back(&x10_hold);
			params.push_back(&ms_hold);
			
			return(DispatchPacket(SERVER_LOCATION_ACS, HARDWARE_LOCATION_ACS, 1, 0, MESSAGE_TYPE_CONFIG, ACS_DEGAUSS_CONFIG, params));
		}
		
		ReturnMessage * ACSUpdateGNDCorrds(const float & x, const float & y, const float & z)
		{
			VariableTypeData x_hold(x);
			VariableTypeData y_hold(y);
			VariableTypeData z_hold(z);

			list<VariableTypeData *> params;
			params.push_back(&x_hold);
			params.push_back(&y_hold);
			params.push_back(&z_hold);
			
			return(DispatchPacket(SERVER_LOCATION_ACS, HARDWARE_LOCATION_ACS, 1, 0, MESSAGE_TYPE_CONFIG, ACS_UPDATE_GND_STATION_COORDS_CONFIG, params));
		}
		
		ReturnMessage * ACSUseGyroBias(const uint32 & bias)
		{
			VariableTypeData bias_hold(bias);

			list<VariableTypeData *> params;
			params.push_back(&bias_hold);
			
			return(DispatchPacket(SERVER_LOCATION_ACS, HARDWARE_LOCATION_ACS, 1, 0, MESSAGE_TYPE_CONFIG, ACS_GYRO_BIAS_CONFIG, params));
		}
	}
}
