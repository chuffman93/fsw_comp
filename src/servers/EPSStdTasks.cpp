/*
 * EPSStdTasks.cpp
 * Written by: Conrad Hougen
 * Created: 7/10/12
 */
#include "HAL/RTC.h"

#include "servers/EPSServer.h"
#include "servers/EPSStdTasks.h"
#include "servers/DispatchStdTasks.h"
#include "servers/COMServer.h"

#include "core/ReturnMessage.h"
#include "core/DataMessage.h"
#include "core/CommandMessage.h"
#include "core/Singleton.h"
#include "core/Factory.h"

#include "util/FileHandler.h"

#include "HAL/Power.h"
#include "HAL/Interrupt.h"
#include "util/Logger.h"

//#include "boards/backplane/dbg_led.h"

using namespace Phoenix::Core;
using namespace Phoenix::HAL;

namespace Phoenix
{
	namespace Servers
	{
		ReturnMessage * EPSHealthStat()
		{
			Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
			ReturnMessage * HSRet = DispatchPacket(SERVER_LOCATION_EPS, HARDWARE_LOCATION_EPS, 1, 0, MESSAGE_TYPE_COMMAND, EPS_HS_CMD);
			logger->Log("EPSStdTasks: EPSHealthStat(): packet dispatched, HSRet acquired\n", LOGGER_LEVEL_INFO);
			
			// Translate H&S into beacon form
			//COMServer * comServer = dynamic_cast<COMServer *> (Factory::GetInstance(COM_SERVER_SINGLETON));
			void * outputArray[10] = {NULL};
			uint32 enumArray[10] = {
				VAR_TYPE_ENUM_UNSIGNED_INT,VAR_TYPE_ENUM_UNSIGNED_INT,VAR_TYPE_ENUM_UNSIGNED_INT,
				VAR_TYPE_ENUM_UNSIGNED_INT,VAR_TYPE_ENUM_UNSIGNED_INT,VAR_TYPE_ENUM_UNSIGNED_INT,
				VAR_TYPE_ENUM_UNSIGNED_INT,VAR_TYPE_ENUM_UNSIGNED_INT,VAR_TYPE_ENUM_UNSIGNED_INT,
				VAR_TYPE_ENUM_UNSIGNED_INT
			};

			if(!ExtractParameters(*HSRet,enumArray,10,outputArray))
			{
				logger->Log("EPSStdTasks: EPSHealthStat(): error extracting parameters\n", LOGGER_LEVEL_WARN);
			}
			else 
			{
																					// Original Types in outputArray
				int16_t battVoltage				= * (int16_t *) outputArray[0];		// EPS uint32_t voltage
				int16_t battCurrent				= * (int16_t *) outputArray[1];		// EPS uint32_t current
				uint16_t stateOfCharge			= * (uint16_t *) outputArray[2];	// EPS uint32_t relativeSOC 
				int16_t battRemainingCapacity	= * (int16_t *) outputArray[3];		// EPS uint32_t remainingCapacity
				int16_t cycleCount				= * (int16_t *) outputArray[8];		// EPS uint32_t cycleCount
				int16_t batteryManagerStatus	= * (int16_t *) outputArray[9];		// EPS uint32_t batteryStatus
					
				//comServer -> UpdateEPSHS(battVoltage,battCurrent, stateOfCharge,battRemainingCapacity,cycleCount,batteryManagerStatus);
			}
			
			return HSRet;
		}

		ReturnMessage * EPSStateOfCharge()
		{
			return(DispatchPacket(SERVER_LOCATION_EPS, HARDWARE_LOCATION_EPS, 1, 0, MESSAGE_TYPE_COMMAND, EPS_SOC_CMD));
		}

		ReturnMessage * EPSPowerCycle()
		{
			return(DispatchPacket(SERVER_LOCATION_EPS, HARDWARE_LOCATION_EPS, 1, 0, MESSAGE_TYPE_COMMAND, EPS_POWER_CYCLE_CMD));
		}

		ReturnMessage * EPSDisableOCProt()
		{
			return(DispatchPacket(SERVER_LOCATION_EPS, HARDWARE_LOCATION_EPS, 1, 0, MESSAGE_TYPE_COMMAND, EPS_DISABLE_OC_PROT_CMD));
		}


		ReturnMessage * EPSEnableOCProt()
		{
			return(DispatchPacket(SERVER_LOCATION_EPS, HARDWARE_LOCATION_EPS, 1, 0, MESSAGE_TYPE_COMMAND, EPS_ENABLE_OC_PROT_CMD));
		}
		
		ReturnMessage * EPSPowerSubsystems(PowerSubsystemEnum subsystem, bool onOff, uint32 time)
		{
			EPSServer * epsServer = dynamic_cast<EPSServer *>(Factory::GetInstance(EPS_SERVER_SINGLETON));
			ReturnMessage * disRet;
			ReturnMessage * enRet;

			switch(subsystem)
			{

				case POWER_SUBSYSTEM_ACS:
					if(onOff)
					{
						if(!epsServer->getPowerSate(subsystem))
						{
							//turn on acs
	//						PowerSet(POWER_SUBSYSTEM_ACS, false);
							//usleep(500000);
							printf("EPS Server Powering On ACS\n");
							//disRet = EPSDisableOCProt();
//							PowerSet(POWER_SUBSYSTEM_ACS, true);
							//enRet = EPSEnableOCProt();
						
							//MessageProcess(SERVER_LOCATION_EPS, enRet);
							//MessageProcess(SERVER_LOCATION_EPS, disRet);
						
							//EnablePinInterrupt(INTERRUPT_PIN_ACS, INTERRUPT_EDGE_RISING);
							epsServer->setPowerOffTimer(subsystem, 0);
							epsServer->setPowerState(subsystem, true);
							system("/home/root/acsOn.sh");
							//usleep(5000000);
							DataMessage msg(EPS_POWER_SUB_SUCCESS);
							ReturnMessage * ret = new ReturnMessage(&msg, true);
							return ret;
						}
						ErrorMessage err(EPS_POWER_SUB_FAILURE);
						ReturnMessage * ret = new ReturnMessage(&err, false);
						return ret;
					}
					else
					{
						if(epsServer->getPowerSate(subsystem))
						{
							//DisablePinInterrupt(INTERRUPT_PIN_ACS);
							//turn off acs
//							PowerSet(POWER_SUBSYSTEM_ACS, false);
							printf("EPS Server Powering Off ACS for %i seconds\n", time/1000);
							epsServer->setPowerOffTimer(subsystem, time);
							epsServer->setPowerState(subsystem, false);
							system("/home/root/acsOff.sh");
							//usleep(1000000);
							DataMessage msg(EPS_POWER_SUB_SUCCESS);
							ReturnMessage * ret = new ReturnMessage(&msg, true);
							return ret;
						}
						else
						{
							printf("EPS Server Incrementing ACS Powering Off timer for %i seconds\n", time/1000);
							epsServer->setPowerOffTimer(subsystem, time + epsServer->getPowerOffTimer(subsystem));
							
							DataMessage msg(EPS_POWER_SUB_SUCCESS);
							ReturnMessage * ret = new ReturnMessage(&msg, true);
							return ret;
						}
						ErrorMessage err(EPS_POWER_SUB_FAILURE);
						ReturnMessage * ret = new ReturnMessage(&err, false);
						return ret;

					}

				case POWER_SUBSYSTEM_COM:
					if(onOff)
					{
						if(!epsServer->getPowerSate(subsystem))
						{
							//turn on com
							
							//PowerSet(POWER_SUBSYSTEM_COM, false);
							//usleep(500000);

							//disRet = EPSDisableOCProt();
//							PowerSet(POWER_SUBSYSTEM_COM, true);
							//enRet = EPSEnableOCProt();
						
							//MessageProcess(SERVER_LOCATION_EPS, enRet);
							//MessageProcess(SERVER_LOCATION_EPS, disRet);
							printf("EPS Server Powering On COM\n");
//							EnablePinInterrupt(INTERRUPT_PIN_COM, INTERRUPT_EDGE_RISING);
							epsServer->setPowerOffTimer(subsystem, 0);
							epsServer->setPowerState(subsystem, true);
							system("/home/root/comOn.sh");
							//usleep(5000000);
							DataMessage msg(EPS_POWER_SUB_SUCCESS);
							ReturnMessage * ret = new ReturnMessage(&msg, true);
							return ret;
						}
						ErrorMessage err(EPS_POWER_SUB_FAILURE);
						ReturnMessage * ret = new ReturnMessage(&err, false);
						return ret;
					}
					else
					{
						if(epsServer->getPowerSate(subsystem))
						{
//							DisablePinInterrupt(INTERRUPT_PIN_COM);
							//turn off com
							
//							PowerSet(POWER_SUBSYSTEM_COM, false);
							printf("EPS Server Powering Off COM for %i seconds\n", time/1000);
							epsServer->setPowerOffTimer(subsystem, time);
							epsServer->setPowerState(subsystem, false);
							system("/home/root/comOff.sh");
							//usleep(1000000);
							DataMessage msg(EPS_POWER_SUB_SUCCESS);
							ReturnMessage * ret = new ReturnMessage(&msg, true);
							return ret;
						}
						else
						{
							printf("EPS Server Incrementing COM Powering Off timer for %i seconds\n", time/1000);
							epsServer->setPowerOffTimer(subsystem, time + epsServer->getPowerOffTimer(subsystem));
							
							DataMessage msg(EPS_POWER_SUB_SUCCESS);
							ReturnMessage * ret = new ReturnMessage(&msg, true);
							return ret;
						}
						ErrorMessage err(EPS_POWER_SUB_FAILURE);
						ReturnMessage * ret = new ReturnMessage(&err, false);
						return ret;
					}
					
				case POWER_SUBSYSTEM_PROP:
					//no prop on ALLSTAR 1
				default:
					//do nothing
					ErrorMessage err(EPS_POWER_SUB_FAILURE);
					ReturnMessage * ret = new ReturnMessage(&err, false);
					return ret;
			}
		}
		
		void CheckPowerTimers(void)
		{
			EPSServer * epsServer = dynamic_cast<EPSServer *> (Factory::GetInstance(EPS_SERVER_SINGLETON));
			//check if ACS should be turned on if off
			if((epsServer->getPowerOffTimer(POWER_SUBSYSTEM_ACS)) <= 0)
			{
				EPSPowerSubsystems(POWER_SUBSYSTEM_ACS, true, 0);
			}
			
			else
			{
				(epsServer->ACSOffTimer) = (epsServer->ACSOffTimer) - 1000;
			}
			//check if PLD should be turned on if off
			if((epsServer->getPowerOffTimer(POWER_SUBSYSTEM_PLD)) <= 0)
			{
				EPSPowerSubsystems(POWER_SUBSYSTEM_PLD, true, 0);
			}
			else
			{
				(epsServer->PLDOffTimer) = (epsServer->PLDOffTimer) - 1000;
			}
			//check if GPS should be turned on if off
			if((epsServer->getPowerOffTimer(POWER_SUBSYSTEM_GPS)) <= 0)
			{
				EPSPowerSubsystems(POWER_SUBSYSTEM_GPS, true, 0);
			}
			else
			{
				(epsServer->GPSOffTimer) = (epsServer->GPSOffTimer) - 1000;
			}
			//check if COM should be turned on if off
			if((epsServer->getPowerOffTimer(POWER_SUBSYSTEM_COM))  <= 0)
			{
				EPSPowerSubsystems(POWER_SUBSYSTEM_COM, true, 0);
			}
			else
			{
				(epsServer->COMOffTimer) = (epsServer->COMOffTimer) - 1000;
			}
		}
		
		void SattyResetTimer(void)
		{
			uint32 time;
			static uint32 lastResetTime = 0;
			//TODO::FIX THIS FUNCTION
			//RTCGetTime(&time, NULL);
			if(((time % 86400 ) == 0))
			{
				EPSPowerCycle();
				lastResetTime = time;
			}
			
		}
		

		bool CheckDeploymentFlag(void)
		{
			//FileHandler * fileHandler = dynamic_cast<FileHandler *> (Factory::GetInstance(FILE_HANDLER_SINGLETON));
			
			int check = 0;
			
			char * file1 = (char *) "DeploymentFlag1.dat";
			char * file2 = (char *) "DeploymentFlag2.dat";
			char * file3 = (char *) "DeploymentFlag3.dat";
			char * file4 = (char *) "DeploymentFlag4.dat";
			char * file5 = (char *) "DeploymentFlag5.dat";
			char * file6 = (char *) "DeploymentFlag6.dat";
			char * file7 = (char *) "DeploymentFlag7.dat";
			
			size_t file1Size = 0;
			size_t file2Size = 0;
			size_t file3Size = 0;
			size_t file4Size = 0;
			size_t file5Size = 0;
			size_t file6Size = 0;
			size_t file7Size = 0;
			
			//read in all 7 files
			//TODO::filehandler
			//uint8 * file1Dat = fileHandler->ReadFile(file1, &file1Size);
			//uint8 * file2Dat = fileHandler->ReadFile(file2, &file2Size);
			//uint8 * file3Dat = fileHandler->ReadFile(file3, &file3Size);
			//uint8 * file4Dat = fileHandler->ReadFile(file4, &file4Size);
			//uint8 * file5Dat = fileHandler->ReadFile(file5, &file5Size);
			//uint8 * file6Dat = fileHandler->ReadFile(file6, &file6Size);
			//uint8 * file7Dat = fileHandler->ReadFile(file7, &file7Size);
			
			//if deployed data will be 63 or ascii @ symbol
			/*if((file1Size >= 1) && (file1Dat[0] == 49))
			{
				check++;
			}
			if((file2Size >= 1) && (file2Dat[0] == 49))
			{
				check++;
			}
			if((file3Size >= 1) && (file3Dat[0] == 49))
			{
				check++;
			}
			if((file4Size >= 1) && (file4Dat[0] == 49))
			{
				check++;
			}
			if((file5Size >= 1) && (file5Dat[0] == 49))
			{
				check++;
			}
			if((file6Size >= 1) && (file6Dat[0] == 49))
			{
				check++;
			}
			if((file7Size >= 1) && (file7Dat[0] == 49))
			{
				check++;
			}*/
			
			//Majority rules
			if(check >= 4)
			{
				return true;
			}
			return false;
		}

		//TODO:FIX THIS FUNCTION
		void BurnFrangiBolt(void)
		{
			int i;
			//burn the bolt
			//PowerSet(POWER_SUBSYSTEM_STR, true);
			
			//wait
			for(i = 0; i < DEPLOYMENT_BURN_TIME; i++)
			{
				//do stuff here if needed.
				usleep(1000000);
			}
			//stop the burn
			//PowerSet(POWER_SUBSYSTEM_STR, false);
			
			//cool down period
			for(i = 0; i < DEPLOYMENT_WAIT_TIME; i++)
			{
				//do stuff here if needed.
				usleep(1000000);
			}
		}
		
		float SetStarCameraThreshold(void)
		{
			uint32 enumArray[] = {VAR_TYPE_ENUM_FLOAT, VAR_TYPE_ENUM_FLOAT, VAR_TYPE_ENUM_FLOAT, VAR_TYPE_ENUM_FLOAT, VAR_TYPE_ENUM_UNSIGNED_INT,
				VAR_TYPE_ENUM_UNSIGNED_INT, VAR_TYPE_ENUM_UNSIGNED_INT, VAR_TYPE_ENUM_UNSIGNED_INT, VAR_TYPE_ENUM_FLOAT};
			ReturnMessage * starCamHS = DispatchPacket(SERVER_LOCATION_EPS, SERVER_LOCATION_ACS, 1,
				0, MESSAGE_TYPE_COMMAND , ACS_STARCAMERA_HS_CMD);
			//MessageProcess(SERVER_LOCATION_EPS, starCamHS);
			
			void * outputArray[9] = {NULL};
			if(!ExtractParameters(*starCamHS, enumArray, 9, outputArray))
			{
				//cannot read data, assume not deployed?
				return 0;
			}
			float sensorThreshold = * (float *) outputArray[8];
			return sensorThreshold;
		}
		
		bool CheckStarCameraDeployed(const float & sensorThreshhold)
		{
			
			uint32 enumArray[] = {VAR_TYPE_ENUM_FLOAT, VAR_TYPE_ENUM_FLOAT, VAR_TYPE_ENUM_FLOAT, VAR_TYPE_ENUM_FLOAT, VAR_TYPE_ENUM_UNSIGNED_INT,
				VAR_TYPE_ENUM_UNSIGNED_INT, VAR_TYPE_ENUM_UNSIGNED_INT, VAR_TYPE_ENUM_UNSIGNED_INT, VAR_TYPE_ENUM_FLOAT};
			ReturnMessage * starCamHS = DispatchPacket(SERVER_LOCATION_EPS, SERVER_LOCATION_ACS, 1,
				0, MESSAGE_TYPE_COMMAND , ACS_STARCAMERA_HS_CMD);
			
			//MessageProcess(SERVER_LOCATION_ACS, starCamHS);
			
			void * outputArray[9] = {NULL};
			if(!ExtractParameters(*starCamHS, enumArray, 9, outputArray))
			{
				//cannot read data, assume not deployed?
				return false;
			}
				
			//float lastLock = * (float *) outputArray[0];
			//float MRPVecOne = * (float *) outputArray[1];
			//float MRPVecTwo = * (float *) outputArray[2];
			//float MRPVecThree = * (float *) outputArray[3];
			uint32 numStars = * (uint32 *) outputArray[4];
			if(numStars > STAR_CAMERA_STARS_THRESHOLD)
			{
				return true;
			}
			//uint32 numTries = * (uint32 *) outputArray[5];
			//uint32 error = * (uint32 *) outputArray[6];
			//uint32 status = * (uint32 *) outputArray[7];
			float avgSensorBright = * (float *) outputArray[8];
			if(avgSensorBright > (sensorThreshhold*STAR_CAMERA_LIGHT_THRESHOLD))
			{
				return true;
			}		
			return false;
		}
		
		void SetDeploymentFlag(void)
		{
			//FileHandler * fileHandler = dynamic_cast<FileHandler *> (Factory::GetInstance(FILE_HANDLER_SINGLETON));
			
			char * file1 = (char *) "DeploymentFlag1.dat";
			char * file2 = (char *) "DeploymentFlag2.dat";
			char * file3 = (char *) "DeploymentFlag3.dat";
			char * file4 = (char *) "DeploymentFlag4.dat";
			char * file5 = (char *) "DeploymentFlag5.dat";
			char * file6 = (char *) "DeploymentFlag6.dat";
			char * file7 = (char *) "DeploymentFlag7.dat";
			//TODO::FILEHANDLER
			/*fileHandler->DeleteFile(file1);
			fileHandler->DeleteFile(file2);
			fileHandler->DeleteFile(file3);
			fileHandler->DeleteFile(file4);
			fileHandler->DeleteFile(file5);
			fileHandler->DeleteFile(file6);
			fileHandler->DeleteFile(file7);
			
			char buf[] = {49};
			
			fileHandler->FileWrite(file1, buf, 1);
			usleep(10000);
			fileHandler->FileWrite(file2, buf, 1);
			usleep(10000);
			fileHandler->FileWrite(file3, buf, 1);
			usleep(10000);
			fileHandler->FileWrite(file4, buf, 1);
			usleep(10000);
			fileHandler->FileWrite(file5, buf, 1);
			usleep(10000);
			fileHandler->FileWrite(file6, buf, 1);
			usleep(10000);
			fileHandler->FileWrite(file7, buf, 1);*/
		}
	} // end namespace servers
} // end namespace phoenix
