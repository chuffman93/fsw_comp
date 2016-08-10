/*
 * EPSStdTasks.h
 * A collection of functions to be run in EPSSubsystemLoop
 *
 * Written by: Conrad Hougen
 * Created: 7/10/12
 */

#ifndef EPSSTDTASKS_H_
#define EPSSTDTASKS_H_

#include "core/ReturnMessage.h"

//#include "boards/board.h"

namespace AllStar
{
	namespace Servers
	{
		AllStar::Core::FSWPacket * EPSHealthStat();
		AllStar::Core::FSWPacket * EPSStateOfCharge();
		AllStar::Core::FSWPacket * EPSPowerCycle();
		AllStar::Core::FSWPacket * EPSDisableOCProt();
		AllStar::Core::FSWPacket * EPSEnableOCProt();
		AllStar::Core::FSWPacket * EPSPowerSubsystems(PowerSubsystemEnum subsystem, bool onOff, uint32 time);
		void CheckPowerTimers(void);
		void SattyResetTimer(void);
		void SattyReset(void);
		bool CheckDeploymentFlag(void);
		void BurnFrangiBolt(void);
		float SetStarCameraThreshold(void);
		bool CheckStarCameraDeployed(const float & sensorThreshhold);
		void SetDeploymentFlag(void);
	}
}

#endif /* EPSSTDTASKS_H_ */
