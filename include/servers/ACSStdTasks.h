/*
 * ACSStdTasks.h
 * A collection of functions to be run in ACSSubsystemLoop
 *
 * Written by: Conrad Hougen
 * Created: 7/10/12
 */

#ifndef ACSSTDTASKS_H_
#define ACSSTDTASKS_H_

#include "core/ACPPacket.h"
#include "core/StdTypes.h"

using namespace AllStar::Core;

namespace AllStar{
namespace Servers{

ACPPacket * ACSHealthStatus(void);
ACPPacket * ACSState(void);
ACPPacket * ACSRawADC(void);
ACPPacket * ACSAttitudeError(void);

//Modified Rodrigez Paramaters in ECI Frame
ACPPacket * ACSPointMRP(const float & x, const float & y, const float & z);

//ECEF Frame (km)
ACPPacket * ACSPointGND(const float & x, const float & y, const float & z, const ACSSystemPointEnum & system);

ACPPacket * ACSRequestMRP(void);
ACPPacket * ACSRequestGND(void);
ACPPacket * ACSSunSoak(void);
ACPPacket * ACSPointNadir(const ACSSystemPointEnum & system);

//ECEF Frame
// 		AllStar::Core::ReturnMessage * ACSSetTarget(const float & x, const float & y, const float & z, const SchedulerTargetEnum & targetNum);
// 		AllStar::Core::ReturnMessage * ACSRangeToTarget(const SchedulerTargetEnum & targetNum);

ACPPacket * ACSHold(void);
ACPPacket * ACSDisable(void);
ACPPacket * ACSReset(void);

// Send ECEF; Receive ECI Conversion
ACPPacket * ACSSendGPS();

ACPPacket * ACSStarCameraHS(void);
ACPPacket * ACSPointComGnd(void);
ACPPacket * ACSHelloworld(const ACPPacket & packet);
// 		AllStar::Core::ReturnMessage * ACSFunctionalTest(const int32 & testNum);
// 		AllStar::Core::ReturnMessage * ACSFunctionalTestComplete(void);

// configuration commands
ACPPacket * ACSControlOrientations(const uint32 & selection, const float & x1, const float & x2, const float & x3,
		const float & y1, const float & y2, const float & y3, const float & z1, const float & z2, const float & z3);
ACPPacket * ACSMOIWheels(const float & motorX, const float & motorY, const float & motorZ);
// 		AllStar::Core::ReturnMessage * ACSMOISatellite(const float & I_11, const float & I_12, const float & I_13, const float & I_22,
// 			const float & I_23, const float & I_33);
ACPPacket * ACSTorqCoeff(const uint32 & rodNumber, const float & p1, const float & p2, const float & p3,
		const float & p4, const float & p5, const float & p6, const float & p7);
ACPPacket * ACSOrbitEphem(const float & a, const float & e, const float & i, const float & Omega,
		const float & omega);
ACPPacket * ACSControllerGains(const float & K, const float & Px, const float & Py, const float & Pz,
		const float & Kix, const float & Kiy, const float & Kiz, const float & Kmx, const float & Kmy, const float & Kmz, const float & Kw);
ACPPacket * ACSContolFreq(const float & motor, const float & gyro, const float & magnetorquer,
		const float & starCamera, const float & magnetometerRate, const float & magnetometerSamples, const float & loopFreq,
		const uint32 & maxPWMTorq, const uint32 & maxPWMWheels);
ACPPacket * ACSStarCameraSettings(const uint32 & digGain, const uint32 & angGain, const uint32 & exposure,
		const uint32 & threshold, const uint32 & maxBrightness, const float & normalErrRange, const float & largeAngleErrRange);
ACPPacket * ACSSetRot(const int32 & rot180, const int32 & rot45, const int32 & rotNeg90, const int32 & rot90);
ACPPacket * ACSUpdateNotch(const float & magSampleRate, const float & bandwidth1, const float & bandwidth2,
		const float & bandwidth3);
ACPPacket * ACSUpdateLeap(const uint32 & leapSec);
ACPPacket * ACSGyroZRV(const float & x, const float & y, const float & z, const float & noise);
ACPPacket * ACSDeguassProfile(const int32 & x1, const int32 & x2, const int32 & x3, const int32 & x4,
		const int32 & x5, const int32 & x6, const int32 & x7, const int32 & x8, const int32 & x9, const int32 & x10, const float & ms);
ACPPacket * ACSUpdateGNDCorrds(const float & x, const float & y, const float & z);
ACPPacket * ACSUseGyroBias(const uint32 & bias);
}
}

#endif /* ACSSTDTASKS_H_ */
