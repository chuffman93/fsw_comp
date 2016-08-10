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

namespace AllStar
{
	namespace Servers
	{
		AllStar::Core::FSWPacket * ACSHealthStatus(void);
		AllStar::Core::FSWPacket * ACSState(void);
		AllStar::Core::FSWPacket * ACSRawADC(void);
		AllStar::Core::FSWPacket * ACSAttitudeError(void);

		//Modified Rodrigez Paramaters in ECI Frame
		AllStar::Core::FSWPacket * ACSPointMRP(const float & x, const float & y, const float & z);

		//ECEF Frame (km)
		AllStar::Core::FSWPacket * ACSPointGND(const float & x, const float & y, const float & z, const ACSSystemPointEnum & system);

		AllStar::Core::FSWPacket * ACSRequestMRP(void);
		AllStar::Core::FSWPacket * ACSRequestGND(void);
		AllStar::Core::FSWPacket * ACSSunSoak(void);
		AllStar::Core::FSWPacket * ACSPointNadir(const ACSSystemPointEnum & system);

		//ECEF Frame
// 		AllStar::Core::ReturnMessage * ACSSetTarget(const float & x, const float & y, const float & z, const SchedulerTargetEnum & targetNum);
// 		AllStar::Core::ReturnMessage * ACSRangeToTarget(const SchedulerTargetEnum & targetNum);

		AllStar::Core::FSWPacket * ACSHold(void);
		AllStar::Core::FSWPacket * ACSDisable(void);
		AllStar::Core::FSWPacket * ACSReset(void);

		// Send ECEF; Receive ECI Conversion
		AllStar::Core::FSWPacket * ACSSendGPS();
							
		AllStar::Core::FSWPacket * ACSStarCameraHS(void);
		AllStar::Core::FSWPacket * ACSPointComGnd(void);
		AllStar::Core::FSWPacket * ACSHelloworld(const AllStar::Core::FSWPacket & packet);
// 		AllStar::Core::ReturnMessage * ACSFunctionalTest(const int32 & testNum);
// 		AllStar::Core::ReturnMessage * ACSFunctionalTestComplete(void);
		
		// configuration commands
		AllStar::Core::FSWPacket * ACSControlOrientations(const uint32 & selection, const float & x1, const float & x2, const float & x3,
			const float & y1, const float & y2, const float & y3, const float & z1, const float & z2, const float & z3);
		AllStar::Core::FSWPacket * ACSMOIWheels(const float & motorX, const float & motorY, const float & motorZ);
// 		AllStar::Core::ReturnMessage * ACSMOISatellite(const float & I_11, const float & I_12, const float & I_13, const float & I_22,
// 			const float & I_23, const float & I_33);
		AllStar::Core::FSWPacket * ACSTorqCoeff(const uint32 & rodNumber, const float & p1, const float & p2, const float & p3,
			const float & p4, const float & p5, const float & p6, const float & p7);
		AllStar::Core::FSWPacket * ACSOrbitEphem(const float & a, const float & e, const float & i, const float & Omega,
			const float & omega);
		AllStar::Core::FSWPacket * ACSControllerGains(const float & K, const float & Px, const float & Py, const float & Pz,
			const float & Kix, const float & Kiy, const float & Kiz, const float & Kmx, const float & Kmy, const float & Kmz, const float & Kw);
		AllStar::Core::FSWPacket * ACSContolFreq(const float & motor, const float & gyro, const float & magnetorquer,
			const float & starCamera, const float & magnetometerRate, const float & magnetometerSamples, const float & loopFreq,
			const uint32 & maxPWMTorq, const uint32 & maxPWMWheels);
		AllStar::Core::FSWPacket * ACSStarCameraSettings(const uint32 & digGain, const uint32 & angGain, const uint32 & exposure,
			const uint32 & threshold, const uint32 & maxBrightness, const float & normalErrRange, const float & largeAngleErrRange);
		AllStar::Core::FSWPacket * ACSSetRot(const int32 & rot180, const int32 & rot45, const int32 & rotNeg90, const int32 & rot90);
		AllStar::Core::FSWPacket * ACSUpdateNotch(const float & magSampleRate, const float & bandwidth1, const float & bandwidth2,
			const float & bandwidth3);
		AllStar::Core::FSWPacket * ACSUpdateLeap(const uint32 & leapSec);
		AllStar::Core::FSWPacket * ACSGyroZRV(const float & x, const float & y, const float & z, const float & noise);
		AllStar::Core::FSWPacket * ACSDeguassProfile(const int32 & x1, const int32 & x2, const int32 & x3, const int32 & x4,
			const int32 & x5, const int32 & x6, const int32 & x7, const int32 & x8, const int32 & x9, const int32 & x10, const float & ms);
		AllStar::Core::FSWPacket * ACSUpdateGNDCorrds(const float & x, const float & y, const float & z);
		AllStar::Core::FSWPacket * ACSUseGyroBias(const uint32 & bias);
	}
}

#endif /* ACSSTDTASKS_H_ */
