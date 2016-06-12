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

namespace Phoenix
{
	namespace Servers
	{
		Phoenix::Core::FSWPacket * EPSHealthStat();
		Phoenix::Core::FSWPacket * EPSStateOfCharge();
		Phoenix::Core::FSWPacket * EPSPowerCycle();
		Phoenix::Core::FSWPacket * EPSDisableOCProt();
		Phoenix::Core::FSWPacket * EPSEnableOCProt();
		Phoenix::Core::FSWPacket * EPSPowerSubsystems(PowerSubsystemEnum subsystem, bool onOff, uint32 time);
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
