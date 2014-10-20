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
		Phoenix::Core::ReturnMessage * EPSHealthStat();
		Phoenix::Core::ReturnMessage * EPSStateOfCharge();
		Phoenix::Core::ReturnMessage * EPSPowerCycle();
		Phoenix::Core::ReturnMessage * EPSDisableOCProt();
		Phoenix::Core::ReturnMessage * EPSEnableOCProt();
		Phoenix::Core::ReturnMessage * EPSPowerSubsystems(PowerSubsystemEnum subsystem, bool onOff, uint32 time);
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
