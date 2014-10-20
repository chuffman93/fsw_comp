/*
 * ACSStdTasks.h
 * A collection of functions to be run in ACSSubsystemLoop
 *
 * Written by: Conrad Hougen
 * Created: 7/10/12
 */

#ifndef ACSSTDTASKS_H_
#define ACSSTDTASKS_H_

#include "core/CommandMessage.h"
#include "core/ReturnMessage.h"
#include "core/StdTypes.h"

namespace Phoenix
{
	namespace Servers
	{
		Phoenix::Core::ReturnMessage * ACSHealthStatus(void);
		Phoenix::Core::ReturnMessage * ACSState(void);
		Phoenix::Core::ReturnMessage * ACSRawADC(void);
		Phoenix::Core::ReturnMessage * ACSAttitudeError(void);

		//Modified Rodrigez Paramaters in ECI Frame
		Phoenix::Core::ReturnMessage * ACSPointMRP(const float & x, const float & y, const float & z);

		//ECEF Frame (km)
		Phoenix::Core::ReturnMessage * ACSPointGND(const float & x, const float & y, const float & z, const ACSSystemPointEnum & system);

		Phoenix::Core::ReturnMessage * ACSRequestMRP(void);
		Phoenix::Core::ReturnMessage * ACSRequestGND(void);
		Phoenix::Core::ReturnMessage * ACSSunSoak(void);
		Phoenix::Core::ReturnMessage * ACSPointNadir(const ACSSystemPointEnum & system);

		//ECEF Frame
// 		Phoenix::Core::ReturnMessage * ACSSetTarget(const float & x, const float & y, const float & z, const SchedulerTargetEnum & targetNum);
// 		Phoenix::Core::ReturnMessage * ACSRangeToTarget(const SchedulerTargetEnum & targetNum);

		Phoenix::Core::ReturnMessage * ACSHold(void);
		Phoenix::Core::ReturnMessage * ACSDisable(void);
		Phoenix::Core::ReturnMessage * ACSReset(void);

		// Send ECEF; Receive ECI Conversion
		Phoenix::Core::ReturnMessage * ACSSendGPS(const float & posX, const float & posY, const float & posZ, const float & velX, 
			const float & velY, const float & velZ, const float & seconds, const uint32 & week);
							
		Phoenix::Core::ReturnMessage * ACSStarCameraHS(void);
		Phoenix::Core::ReturnMessage * ACSPointComGnd(void);
// 		Phoenix::Core::ReturnMessage * ACSFunctionalTest(const int32 & testNum);
// 		Phoenix::Core::ReturnMessage * ACSFunctionalTestComplete(void);
		
		// configuration commands
		Phoenix::Core::ReturnMessage * ACSControlOrientations(const uint32 & selection, const float & x1, const float & x2, const float & x3,
			const float & y1, const float & y2, const float & y3, const float & z1, const float & z2, const float & z3);
		Phoenix::Core::ReturnMessage * ACSMOIWheels(const float & motorX, const float & motorY, const float & motorZ);
// 		Phoenix::Core::ReturnMessage * ACSMOISatellite(const float & I_11, const float & I_12, const float & I_13, const float & I_22,
// 			const float & I_23, const float & I_33);
		Phoenix::Core::ReturnMessage * ACSTorqCoeff(const uint32 & rodNumber, const float & p1, const float & p2, const float & p3,
			const float & p4, const float & p5, const float & p6, const float & p7);
		Phoenix::Core::ReturnMessage * ACSOrbitEphem(const float & a, const float & e, const float & i, const float & Omega,
			const float & omega);
		Phoenix::Core::ReturnMessage * ACSControllerGains(const float & K, const float & Px, const float & Py, const float & Pz,
			const float & Kix, const float & Kiy, const float & Kiz, const float & Kmx, const float & Kmy, const float & Kmz, const float & Kw);
		Phoenix::Core::ReturnMessage * ACSContolFreq(const float & motor, const float & gyro, const float & magnetorquer,
			const float & starCamera, const float & magnetometerRate, const float & magnetometerSamples, const float & loopFreq,
			const uint32 & maxPWMTorq, const uint32 & maxPWMWheels);
		Phoenix::Core::ReturnMessage * ACSStarCameraSettings(const uint32 & digGain, const uint32 & angGain, const uint32 & exposure,
			const uint32 & threshold, const uint32 & maxBrightness, const float & normalErrRange, const float & largeAngleErrRange);
		Phoenix::Core::ReturnMessage * ACSSetRot(const int32 & rot180, const int32 & rot45, const int32 & rotNeg90, const int32 & rot90);
		Phoenix::Core::ReturnMessage * ACSUpdateNotch(const float & magSampleRate, const float & bandwidth1, const float & bandwidth2,
			const float & bandwidth3);
		Phoenix::Core::ReturnMessage * ACSUpdateLeap(const uint32 & leapSec);
		Phoenix::Core::ReturnMessage * ACSGyroZRV(const float & x, const float & y, const float & z, const float & noise);
		Phoenix::Core::ReturnMessage * ACSDeguassProfile(const int32 & x1, const int32 & x2, const int32 & x3, const int32 & x4,
			const int32 & x5, const int32 & x6, const int32 & x7, const int32 & x8, const int32 & x9, const int32 & x10, const float & ms);
		Phoenix::Core::ReturnMessage * ACSUpdateGNDCorrds(const float & x, const float & y, const float & z);
		Phoenix::Core::ReturnMessage * ACSUseGyroBias(const uint32 & bias);
	}
}

#endif /* ACSSTDTASKS_H_ */
