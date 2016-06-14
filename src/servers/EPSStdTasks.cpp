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
using namespace Phoenix::Servers;

namespace Phoenix
{
	namespace Servers
	{
		FSWPacket * EPSHealthStat()
		{
			Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
			logger->Log("EPSStdTasks: unfinished function entered!", LOGGER_LEVEL_FATAL);

			FSWPacket * HSQuery = new FSWPacket(SERVER_LOCATION_EPS, HARDWARE_LOCATION_EPS, 0, EPS_HS_CMD, true, false, MESSAGE_TYPE_COMMAND);
			FSWPacket * HSRet = DispatchPacket(HSQuery);
			logger->Log("EPSStdTasks: EPSHealthStat(): packet dispatched, HSRet acquired", LOGGER_LEVEL_INFO);
			if(HSRet == NULL){
				printf("Null HSRet\n");
			}
			printf("Length: %u\n", HSRet->GetMessageLength());
			if(HSRet->GetMessageLength() != 6*sizeof(uint16)){
				logger->Log("EPSStdTasks: EPSHealthStat(): incorrect message length!", LOGGER_LEVEL_WARN);

				//TODO: return error?
				return HSRet;
			}else{
				// Parse buffer
				printf("pointer\n");
				uint8 * msgPtr = HSRet->GetMessageBufPtr();
				if(msgPtr==NULL){
					//Error
					return HSRet;
				}
				uint16 outputArray[6];
				for(uint8 i = 0; i < 6; i++){
					printf("Loop\n");
					outputArray[i] = GetUInt16(msgPtr);
					msgPtr += 2;
				}

				uint16 volt_3v3 	= outputArray[0];
				logger->Log("3v3  Voltage: %u", (uint32) volt_3v3, LOGGER_LEVEL_DEBUG);
				uint16 curr_3v3 	= outputArray[1];
				logger->Log("3v3  Current: %u", (uint32) curr_3v3, LOGGER_LEVEL_DEBUG);
				uint16 volt_vbat 	= outputArray[2];
				logger->Log("Vbat Voltage: %u", (uint32) volt_vbat, LOGGER_LEVEL_DEBUG);
				uint16 curr_vbat 	= outputArray[3];
				logger->Log("Vbat Current: %u", (uint32) curr_vbat, LOGGER_LEVEL_DEBUG);
				uint16 volt_12v 	= outputArray[4];
				logger->Log("12v  Voltage: %u", (uint32) volt_12v, LOGGER_LEVEL_DEBUG);
				uint16 curr_12v 	= outputArray[5];
				logger->Log("12v  Current: %u", (uint32) curr_12v, LOGGER_LEVEL_DEBUG);

				// Update beacon
				//COMServer * comServer = dynamic_cast<COMServer *> (Factory::GetInstance(COM_SERVER_SINGLETON));
				//comServer -> UpdateEPSHS(battVoltage,battCurrent, stateOfCharge,battRemainingCapacity,cycleCount,batteryManagerStatus);
				return HSRet;
			}
		}

		FSWPacket * EPSStateOfCharge()
		{
			FSWPacket * query = new FSWPacket(SERVER_LOCATION_EPS, HARDWARE_LOCATION_EPS, 0, EPS_SOC_CMD, true, false, MESSAGE_TYPE_COMMAND);
			return(DispatchPacket(query));
		}

		FSWPacket * EPSPowerCycle()
		{
			FSWPacket * query = new FSWPacket(SERVER_LOCATION_EPS, HARDWARE_LOCATION_EPS, 0, EPS_POWER_CYCLE_CMD, true, false, MESSAGE_TYPE_COMMAND);
			return(DispatchPacket(query));
		}

		FSWPacket * EPSDisableOCProt()
		{
			FSWPacket * query = new FSWPacket(SERVER_LOCATION_EPS, HARDWARE_LOCATION_EPS, 0, EPS_DISABLE_OC_PROT_CMD, true, false, MESSAGE_TYPE_COMMAND);
			return(DispatchPacket(query));
		}


		FSWPacket * EPSEnableOCProt()
		{
			FSWPacket * query = new FSWPacket(SERVER_LOCATION_EPS, HARDWARE_LOCATION_EPS, 0, EPS_ENABLE_OC_PROT_CMD, true, false, MESSAGE_TYPE_COMMAND);
			return(DispatchPacket(query));
		}
		
		FSWPacket * EPSPowerSubsystems(PowerSubsystemEnum subsystem, bool onOff, uint32 time)
		{
			Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
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
							logger->Log("EPS Server Powering On ACS", LOGGER_LEVEL_INFO);
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
							FSWPacket * ret = new FSWPacket(0, EPS_POWER_SUB_SUCCESS, true, true, MESSAGE_TYPE_DATA);
							return ret;
						}
						FSWPacket * ret = new FSWPacket(0, EPS_POWER_SUB_FAILURE, false, true, MESSAGE_TYPE_ERROR);
						return ret;
					}
					else
					{
						if(epsServer->getPowerSate(subsystem))
						{
							//DisablePinInterrupt(INTERRUPT_PIN_ACS);
							//turn off acs
//							PowerSet(POWER_SUBSYSTEM_ACS, false);
							logger->Log("EPS Server Powering Off ACS for %u seconds", time/1000, LOGGER_LEVEL_INFO);
							epsServer->setPowerOffTimer(subsystem, time);
							epsServer->setPowerState(subsystem, false);
							system("/home/root/acsOff.sh");
							//usleep(1000000);
							FSWPacket * ret = new FSWPacket(0, EPS_POWER_SUB_SUCCESS, true, true, MESSAGE_TYPE_DATA);
							return ret;
						}
						else
						{
							logger->Log("EPS Server Incrementing ACS Powering Off timer for %u seconds", time/1000, LOGGER_LEVEL_INFO);
							epsServer->setPowerOffTimer(subsystem, time + epsServer->getPowerOffTimer(subsystem));
							
							FSWPacket * ret = new FSWPacket(0, EPS_POWER_SUB_SUCCESS, true, true, MESSAGE_TYPE_DATA);
							return ret;
						}
						FSWPacket * ret = new FSWPacket(0, EPS_POWER_SUB_FAILURE, false, true, MESSAGE_TYPE_ERROR);
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
							FSWPacket * ret = new FSWPacket(0, EPS_POWER_SUB_SUCCESS, true, true, MESSAGE_TYPE_DATA);
							return ret;
						}
						FSWPacket * ret = new FSWPacket(0, EPS_POWER_SUB_FAILURE, false, true, MESSAGE_TYPE_ERROR);
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
							FSWPacket * ret = new FSWPacket(0, EPS_POWER_SUB_SUCCESS, true, true, MESSAGE_TYPE_DATA);
							return ret;
						}
						else
						{
							printf("EPS Server Incrementing COM Powering Off timer for %i seconds\n", time/1000);
							epsServer->setPowerOffTimer(subsystem, time + epsServer->getPowerOffTimer(subsystem));
							
							FSWPacket * ret = new FSWPacket(0, EPS_POWER_SUB_SUCCESS, true, true, MESSAGE_TYPE_DATA);
							return ret;
						}
						FSWPacket * ret = new FSWPacket(0, EPS_POWER_SUB_FAILURE, false, true, MESSAGE_TYPE_ERROR);
						return ret;
					}
					
				case POWER_SUBSYSTEM_PROP:
					//no prop on ALLSTAR 1
				default:
					//do nothing
					logger->Log("EPS power subsystem: unknown subsystem!", LOGGER_LEVEL_ERROR);
					FSWPacket * ret = new FSWPacket(0, EPS_POWER_SUB_FAILURE, false, true, MESSAGE_TYPE_ERROR);
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
			Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
			logger->Log("EPSStdTasks: SetStarCameraThreshold(): Unfinished function!", LOGGER_LEVEL_FATAL);

//			uint32 enumArray[] = {VAR_TYPE_ENUM_FLOAT, VAR_TYPE_ENUM_FLOAT, VAR_TYPE_ENUM_FLOAT, VAR_TYPE_ENUM_FLOAT, VAR_TYPE_ENUM_UNSIGNED_INT,
//				VAR_TYPE_ENUM_UNSIGNED_INT, VAR_TYPE_ENUM_UNSIGNED_INT, VAR_TYPE_ENUM_UNSIGNED_INT, VAR_TYPE_ENUM_FLOAT};
//			ReturnMessage * starCamHS = DispatchPacket(SERVER_LOCATION_EPS, SERVER_LOCATION_ACS, 1,
//				0, MESSAGE_TYPE_COMMAND , ACS_STARCAMERA_HS_CMD);
//			//MessageProcess(SERVER_LOCATION_EPS, starCamHS);
//
//			void * outputArray[9] = {NULL};
//			if(!ExtractParameters(*starCamHS, enumArray, 9, outputArray))
//			{
//				//cannot read data, assume not deployed?
//				return 0;
//			}
//			float sensorThreshold = * (float *) outputArray[8];
//			return sensorThreshold;
		}
		
		bool CheckStarCameraDeployed(const float & sensorThreshhold)
		{
			Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
			logger->Log("EPSStdTasks: CheckStarCameraDeployed(): Unfinished function!", LOGGER_LEVEL_FATAL);
			
//			uint32 enumArray[] = {VAR_TYPE_ENUM_FLOAT, VAR_TYPE_ENUM_FLOAT, VAR_TYPE_ENUM_FLOAT, VAR_TYPE_ENUM_FLOAT, VAR_TYPE_ENUM_UNSIGNED_INT,
//				VAR_TYPE_ENUM_UNSIGNED_INT, VAR_TYPE_ENUM_UNSIGNED_INT, VAR_TYPE_ENUM_UNSIGNED_INT, VAR_TYPE_ENUM_FLOAT};
//			ReturnMessage * starCamHS = DispatchPacket(SERVER_LOCATION_EPS, SERVER_LOCATION_ACS, 1,
//				0, MESSAGE_TYPE_COMMAND , ACS_STARCAMERA_HS_CMD);
//
//			//MessageProcess(SERVER_LOCATION_ACS, starCamHS);
//
//			void * outputArray[9] = {NULL};
//			if(!ExtractParameters(*starCamHS, enumArray, 9, outputArray))
//			{
//				//cannot read data, assume not deployed?
//				return false;
//			}
//
//			//float lastLock = * (float *) outputArray[0];
//			//float MRPVecOne = * (float *) outputArray[1];
//			//float MRPVecTwo = * (float *) outputArray[2];
//			//float MRPVecThree = * (float *) outputArray[3];
//			uint32 numStars = * (uint32 *) outputArray[4];
//			if(numStars > STAR_CAMERA_STARS_THRESHOLD)
//			{
//				return true;
//			}
//			//uint32 numTries = * (uint32 *) outputArray[5];
//			//uint32 error = * (uint32 *) outputArray[6];
//			//uint32 status = * (uint32 *) outputArray[7];
//			float avgSensorBright = * (float *) outputArray[8];
//			if(avgSensorBright > (sensorThreshhold*STAR_CAMERA_LIGHT_THRESHOLD))
//			{
//				return true;
//			}
//			return false;
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
