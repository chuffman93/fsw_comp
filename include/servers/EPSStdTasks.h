/*
 * EPSStdTasks.h
 * A collection of functions to be run in EPSSubsystemLoop
 *
 * Written by: Conrad Hougen
 * Created: 7/10/12
 */

#ifndef EPSSTDTASKS_H_
#define EPSSTDTASKS_H_

namespace AllStar{
namespace Servers{

AllStar::Core::ACPPacket * EPSHealthStat();
AllStar::Core::ACPPacket * EPSStateOfCharge();
AllStar::Core::ACPPacket * EPSPowerCycle();
AllStar::Core::ACPPacket * EPSDisableOCProt();
AllStar::Core::ACPPacket * EPSEnableOCProt();
AllStar::Core::ACPPacket * EPSPowerSubsystems(PowerSubsystemEnum subsystem, bool onOff, uint32 time);
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
