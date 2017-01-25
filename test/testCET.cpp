#include "gtest/gtest.h"
#include "core/Factory.h"
#include "core/StdTypes.h"
#include "HAL/SPI_Server.h"
#include "servers/ACSServer.h"
#include "servers/ACSStdTasks.h"
#include "servers/COMServer.h"
#include "servers/COMStdTasks.h"
#include "servers/EPSServer.h"
#include "servers/EPSStdTasks.h"
#include "servers/PLDServer.h"
#include "servers/PLDStdTasks.h"
#include "servers/GPSServer.h"
#include "servers/CMDServer.h"
#include "servers/CDHStdTasks.h"
#include "servers/DispatchStdTasks.h"

using namespace std;
using namespace AllStar::Core;
using namespace AllStar::Servers;

// ------------------------------- Helper Functions -------------------------------

// initialize junk GPS data for testing purposes
void prepGPSData(void){
	GPSServer * gpsServer = dynamic_cast<GPSServer *> (Factory::GetInstance(GPS_SERVER_SINGLETON));
	gpsServer->GPSDataHolder->posX = 1.1;
	gpsServer->GPSDataHolder->posY = 1.2;
	gpsServer->GPSDataHolder->posZ = 1.3;
	gpsServer->GPSDataHolder->velX = 2.1;
	gpsServer->GPSDataHolder->velY = 2.2;
	gpsServer->GPSDataHolder->velZ = 2.3;
	gpsServer->GPSDataHolder->GPSSec = 10.0;
	gpsServer->GPSDataHolder->GPSWeek = 20;
}

// initialize junk beacon data for testing purposes
void prepBeacon(void){
	CMDServer * cmdServer = dynamic_cast<CMDServer *> (Factory::GetInstance(CMD_SERVER_SINGLETON));

	cmdServer->beacon.bStruct.acsHS = {101,102,103,104,105,106,107};
	cmdServer->beacon.bStruct.cdhHS = {1.01,1.02,1.03,1.04};
	cmdServer->beacon.bStruct.epsHS = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18};
	cmdServer->beacon.bStruct.pldHS = {1,2,{1,2,3,4,5,6,7,8,9,10},3,4,5};
	cmdServer->beacon.bStruct.currentMode = MODE_BUS_PRIORITY;
	cmdServer->beacon.bStruct.subPowerState = 0b00001111; // all on
	cmdServer->beacon.bStruct.uptime = 9000;
}

void startSPIServer(){
	SPI_HALServer * spiServer = dynamic_cast<SPI_HALServer *> (Factory::GetInstance(SPI_HALSERVER_SINGLETON));
	bool threadsCreated = true;

	threadsCreated &= WatchdogManager::StartServer(spiServer, 0,	true);	 //SPI
}

bool ACSHealthStatus(){
	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));

	ACPPacket * HSQuery = new ACPPacket(SERVER_LOCATION_ACS, HARDWARE_LOCATION_ACS, HEALTH_STATUS_CMD);
	ACPPacket * HSRet = DispatchPacket(HSQuery);

	if(HSRet == NULL){
		logger->Log(LOGGER_LEVEL_ERROR, "ACSServer: NULL HSRet");
		return false;
	}

	if(HSRet->getLength() != 7*sizeof(uint32)){
		logger->Log(LOGGER_LEVEL_WARN, "ACSServer: CheckHealthStatus(): incorrect message length! %u", HSRet->getLength());

		//TODO: return error?
		return false;
	}else{
		logger->Log(LOGGER_LEVEL_INFO, "ACSServer: CheckHealthStatus(): packet dispatched, HSRet acquired");
		// Parse buffer
		uint8 * msgPtr = HSRet->getMessageBuff();
		if(msgPtr==NULL){
			//Error
			return false;
		}

		uint32 outputArray[7];
		for(uint8 i = 0; i < 7; i++){
			outputArray[i] = GetUInt32(msgPtr);
			msgPtr += 2;
		}

		logger->Log(LOGGER_LEVEL_INFO, "ACS H&S: MRP X:       %lx", outputArray[0]);
		logger->Log(LOGGER_LEVEL_INFO, "ACS H&S: MRP Y:       %lx", outputArray[1]);
		logger->Log(LOGGER_LEVEL_INFO, "ACS H&S: MRP Z:       %lx", outputArray[2]);
		logger->Log(LOGGER_LEVEL_INFO, "ACS H&S: ST Status:   %lx", outputArray[3]);
		logger->Log(LOGGER_LEVEL_INFO, "ACS H&S: RW Speed X:  %u", outputArray[4]);
		logger->Log(LOGGER_LEVEL_INFO, "ACS H&S: RW Speed Y:  %u", outputArray[5]);
		logger->Log(LOGGER_LEVEL_INFO, "ACS H&S: RW Speed Z:  %u", outputArray[6]);
		return true;
	}
}

// ------------------------------- Subsystem Tests -------------------------------

TEST(testCET, testACS){
	prepPowerGPIOs();
	startSPIServer();
	CDHServer * cdhServer = dynamic_cast<CDHServer *> (Factory::GetInstance(CDH_SERVER_SINGLETON));
	cdhServer->subPowerOn(HARDWARE_LOCATION_ACS);
	usleep(1000000);

	// ----- Run the self check for ACS (LED On, LED Rate, LED data)
	bool selfTest = true;
	selfTest &= ACSSelfCheck();
	ASSERT_TRUE(selfTest);
	usleep(1000000);

	// -----Test reaction wheel and torque rod
	bool driverTest = true;
	driverTest &= ACSTestDriver(0,0.01,0.5);
	usleep(5000000); // run for 5 seconds
	driverTest &= ACSTestDriver(0,0.0,0.0);
	ASSERT_TRUE(driverTest);
	usleep(1000000);

	// ----- Test health and status
	bool hsTest = true;
	hsTest &= ACSHealthStatus();
	ASSERT_TRUE(hsTest);
	usleep(1000000);

	// ----- Test pointing opcodes
	bool pointTest = true;
	pointTest &= ACSPointNadir();
	usleep(2000000);
	pointTest &= ACSPointDest();
	usleep(2000000);
	pointTest &= ACSPointSun();
	usleep(2000000);
	pointTest &= ACSPointGND();
	ASSERT_TRUE(pointTest);
	usleep(1000000);

	// ----- Test GPS sending
	bool gpsDataTest = true;
	prepGPSData();
	gpsDataTest &= ACSSendGPS();
	ASSERT_TRUE(gpsDataTest);
}

TEST(testCET, testCOM){
	prepPowerGPIOs();
	startSPIServer();
	CDHServer * cdhServer = dynamic_cast<CDHServer *> (Factory::GetInstance(CDH_SERVER_SINGLETON));
	cdhServer->subPowerOn(HARDWARE_LOCATION_COM);
	usleep(1000000);

	// ----- Run the self check for COM (LED On, LED Rate, LED data)
	bool selfTest = true;
	selfTest &= COMSelfCheck();
	ASSERT_TRUE(selfTest);
	usleep(1000000);

	// ----- Test com modes
	bool modeTest = true;
	modeTest &= COMSimplex();
	usleep(2000000);
	modeTest &= COMHalfDuplex();
	usleep(2000000);
	modeTest &= COMFullDuplex();
	ASSERT_TRUE(modeTest);
	usleep(1000000);

	// ----- Test beacon
//	bool beaconTest = true;
//	prepBeacon();
//	beaconTest &= COMSendBeacon();
//	ASSERT_TRUE(beaconTest);
}

TEST(testCET, testPLD){
	prepPowerGPIOs();
	startSPIServer();
	CDHServer * cdhServer = dynamic_cast<CDHServer *> (Factory::GetInstance(CDH_SERVER_SINGLETON));
	cdhServer->resetAssert(HARDWARE_LOCATION_PLD);
	cdhServer->subPowerOn(HARDWARE_LOCATION_PLD);
	usleep(1000000);
	cdhServer->resetDeassert(HARDWARE_LOCATION_PLD);
	usleep(1000000);

	// ----- Run the self check for PLD (LED On, LED Rate, LED data)
	bool selfTest = true;
	selfTest &= PLDSelfCheck();
	ASSERT_TRUE(selfTest);
	usleep(1000000);

	// Other opcodes verified via DITL testing
}

TEST(testCET, testEPS){
	startSPIServer();
	usleep(1000000);

	// ----- Run the self check for EPS (LED On, LED Rate, LED data)
	bool selfTest = true;
	selfTest &= EPSSelfCheck();
	ASSERT_TRUE(selfTest);
	usleep(1000000);

	// EPS reset must be verified independently
}
