#include "gtest/gtest.h"
#include "core/Factory.h"
#include "core/StdTypes.h"
#include "core/WatchdogManager.h"
#include "HAL/SPI_Server.h"
#include "util/Logger.h"
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
	GPSServer * gpsServer = static_cast<GPSServer *> (Factory::GetInstance(GPS_SERVER_SINGLETON));
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
//void prepBeacon(void){
//	CMDServer * cmdServer = static_cast<CMDServer *> (Factory::GetInstance(CMD_SERVER_SINGLETON));
//
//	cmdServer->beacon.bStruct.acsHS = {101,102,103,104,105,106,107};
//	cmdServer->beacon.bStruct.cdhHS = {1.01,1.02,1.03,1.04};
//	cmdServer->beacon.bStruct.epsHS = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18};
//	cmdServer->beacon.bStruct.pldHS = {1,2,{1,2,3,4,5,6,7,8,9,10},3,4,5};
//	cmdServer->beacon.bStruct.currentMode = MODE_BUS_PRIORITY;
//	cmdServer->beacon.bStruct.subPowerState = 0b00001111; // all on
//	cmdServer->beacon.bStruct.uptime = 9000;
//}

void startSPIServer(){
	WatchdogManager * wdm = static_cast<WatchdogManager *> (Factory::GetInstance(WATCHDOG_MANAGER_SINGLETON));
	SPI_HALServer * spiServer = static_cast<SPI_HALServer *> (Factory::GetInstance(SPI_HALSERVER_SINGLETON));
	bool threadsCreated = true;

	threadsCreated &= wdm->StartServer(spiServer, 0,	true);	 //SPI
}

bool ACSHealthStatus(){
	Logger * logger = static_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));

	ACPPacket * HSQuery = new ACPPacket(SERVER_LOCATION_ACS, HARDWARE_LOCATION_ACS, HEALTH_STATUS_CMD);
	ACPPacket * HSRet = DispatchPacket(HSQuery);

	if(HSRet == NULL){
		logger->Error("ACSServer: NULL HSRet");
		return false;
	}

	if(HSRet->getLength() != 7*sizeof(uint32)){
		logger->Warning("ACSServer: CheckHealthStatus(): incorrect message length! %u", HSRet->getLength());

		//TODO: return error?
		return false;
	}else{
		logger->Info("ACSServer: CheckHealthStatus(): packet dispatched, HSRet acquired");
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

		logger->Info("ACS H&S: MRP X:       %lx", outputArray[0]);
		logger->Info("ACS H&S: MRP Y:       %lx", outputArray[1]);
		logger->Info("ACS H&S: MRP Z:       %lx", outputArray[2]);
		logger->Info("ACS H&S: ST Status:   %lx", outputArray[3]);
		logger->Info("ACS H&S: RW Speed X:  %u", outputArray[4]);
		logger->Info("ACS H&S: RW Speed Y:  %u", outputArray[5]);
		logger->Info("ACS H&S: RW Speed Z:  %u", outputArray[6]);
		return true;
	}
}

// ------------------------------- Subsystem Tests -------------------------------

TEST(testCET, testACS){
	Logger * logger = static_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
	prepPowerGPIOs();
	startSPIServer();
	CDHServer * cdhServer = static_cast<CDHServer *> (Factory::GetInstance(CDH_SERVER_SINGLETON));
	//cdhServer->resetAssert(HARDWARE_LOCATION_ACS);
	cdhServer->subPowerOn(HARDWARE_LOCATION_ACS);
	//usleep(1000000);
	//cdhServer->resetDeassert(HARDWARE_LOCATION_ACS);
	usleep(1000000);

	// ----- Run the self check for ACS (LED On, LED Rate, LED data)
	logger->Info("Checking if ACS is alive");
	bool testAlive = ACSTestAlive();
	if(testAlive){
		logger->Info("ACS is alive");
	}
	logger->Info("Sending three opcode self check");
	bool selfCheck = ACSSelfCheck();
	if(selfCheck){
		logger->Info("ACS passed self check");
	}
	ASSERT_TRUE(testAlive && selfCheck);
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
	logger->Info("Sending test GPS data to ACS");
	gpsDataTest &= ACSSendGPS();
	ASSERT_TRUE(gpsDataTest);
}

// test error response to unimplemented opcodes
TEST(testCET, testACSError){
	Logger * logger = static_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
	logger->threshold = LOGGER_LEVEL_WARN;

	prepPowerGPIOs();
	startSPIServer();
	CDHServer * cdhServer = static_cast<CDHServer *> (Factory::GetInstance(CDH_SERVER_SINGLETON));
	cdhServer->subPowerOn(HARDWARE_LOCATION_ACS);
	usleep(4000000);

	ACPPacket * testCMD;
	ACPPacket * testRet;

	for(uint16 i = LED_RATE_DATA + 1; i < TEST_ALIVE_CMD; i++){
		testCMD = new ACPPacket(SERVER_LOCATION_ACS, HARDWARE_LOCATION_ACS, i);
		testRet = DispatchPacket(testCMD);
		EXPECT_EQ(testRet->getErrorStatus(), 0xAA);
		if(!(testRet->getErrorStatus() == 0xAA)){
			printf("Opcode: %d\n", i);
		}
		usleep(100000);
	}

	for(uint16 i = ACS_TEST_DRIVER + 1; i < HEALTH_STATUS_CMD; i++){
		testCMD = new ACPPacket(SERVER_LOCATION_ACS, HARDWARE_LOCATION_ACS, i);
		testRet = DispatchPacket(testCMD);
		EXPECT_EQ(testRet->getErrorStatus(), 0xAA);
		if(!(testRet->getErrorStatus() == 0xAA)){
			printf("Opcode: %d\n", i);
		}
		usleep(100000);
	}

	for(uint16 i = SUBSYSTEM_RESET_CMD + 1; i < ACS_OPCODE_MIN; i++){
		testCMD = new ACPPacket(SERVER_LOCATION_ACS, HARDWARE_LOCATION_ACS, i);
		testRet = DispatchPacket(testCMD);
		EXPECT_EQ(testRet->getErrorStatus(), 0xAA);
		if(!(testRet->getErrorStatus() == 0xAA)){
			printf("Opcode: %d\n", i);
		}
		usleep(100000);
	}

	for(uint16 i = ACS_OPCODE_MAX; i < SUBSYSTEM_OPCODE_MAX + 1; i++){
		testCMD = new ACPPacket(SERVER_LOCATION_ACS, HARDWARE_LOCATION_ACS, i);
		testRet = DispatchPacket(testCMD);
		EXPECT_EQ(testRet->getErrorStatus(), 0xAA);
		if(!(testRet->getErrorStatus() == 0xAA)){
			printf("Opcode: %d\n", i);
		}
		usleep(100000);
	}
}

TEST(testCET, testCOM){
//	Logger * logger = static_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
//	prepPowerGPIOs();
//	startSPIServer();
//	CDHServer * cdhServer = static_cast<CDHServer *> (Factory::GetInstance(CDH_SERVER_SINGLETON));
//	cdhServer->subPowerOn(HARDWARE_LOCATION_COM);
//	usleep(1000000);
//
//	// ----- Run the self check for COM (LED On, LED Rate, LED data)
//	logger->Info("Checking if COM is alive");
//	bool testAlive = COMTestAlive();
//	if(testAlive){
//		logger->Info("COM is alive");
//	}
//	logger->Info("Sending three opcode self check");
//	bool selfCheck = COMSelfCheck();
//	if(selfCheck){
//		logger->Info("COM passed self check");
//	}
//	ASSERT_TRUE(testAlive && selfCheck);
//	usleep(1000000);
//
//	// ----- Test com modes
//	bool modeTest = true;
//	modeTest &= COMSimplex();
//	usleep(2000000);
//	modeTest &= COMHalfDuplex();
//	usleep(2000000);
//	modeTest &= COMFullDuplex();
//	ASSERT_TRUE(modeTest);
//	usleep(1000000);
//
//	// ----- Test beacon
//	bool beaconTest = true;
//	prepBeacon();
//	beaconTest &= COMSendBeacon();
//	ASSERT_TRUE(beaconTest);
}

// test error response to unimplemented opcodes
TEST(testCET, testCOMError){
	Logger * logger = static_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
	logger->threshold = LOGGER_LEVEL_WARN;

	prepPowerGPIOs();
	startSPIServer();
	CDHServer * cdhServer = static_cast<CDHServer *> (Factory::GetInstance(CDH_SERVER_SINGLETON));
	cdhServer->subPowerOn(HARDWARE_LOCATION_COM);
	usleep(4000000);

	ACPPacket * testCMD;
	ACPPacket * testRet;

	for(uint16 i = LED_RATE_DATA + 1; i < TEST_ALIVE_CMD; i++){
		testCMD = new ACPPacket(SERVER_LOCATION_COM, HARDWARE_LOCATION_COM, i);
		testRet = DispatchPacket(testCMD);
		EXPECT_EQ(testRet->getErrorStatus(), 0xAA);
		if(!(testRet->getErrorStatus() == 0xAA)){
			printf("Opcode: %d\n", i);
		}
		usleep(100000);
	}

	for(uint16 i = SUBSYSTEM_RESET_CMD + 1; i < COM_OPCODE_MIN; i++){
		testCMD = new ACPPacket(SERVER_LOCATION_COM, HARDWARE_LOCATION_COM, i);
		testRet = DispatchPacket(testCMD);
		EXPECT_EQ(testRet->getErrorStatus(), 0xAA);
		if(!(testRet->getErrorStatus() == 0xAA)){
			printf("Opcode: %d\n", i);
		}
		usleep(100000);
	}

	for(uint16 i = COM_OPCODE_MAX + 1; i < SUBSYSTEM_OPCODE_MAX + 1; i++){
		testCMD = new ACPPacket(SERVER_LOCATION_COM, HARDWARE_LOCATION_COM, i);
		testRet = DispatchPacket(testCMD);
		EXPECT_EQ(testRet->getErrorStatus(), 0xAA);
		if(!(testRet->getErrorStatus() == 0xAA)){
			printf("Opcode: %d\n", i);
		}
		usleep(100000);
	}
}

TEST(testCET, testPLD){
	Logger * logger = static_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
	prepPowerGPIOs();
	startSPIServer();
	CDHServer * cdhServer = static_cast<CDHServer *> (Factory::GetInstance(CDH_SERVER_SINGLETON));
	cdhServer->resetAssert(HARDWARE_LOCATION_PLD);
	cdhServer->subPowerOn(HARDWARE_LOCATION_PLD);
	usleep(1000000);
	cdhServer->resetDeassert(HARDWARE_LOCATION_PLD);
	usleep(3000000);

	// ----- Run the self check for PLD (LED On, LED Rate, LED data)
	logger->Info("Checking if PLD is alive");
	bool testAlive = PLDTestAlive();
	if(testAlive){
		logger->Info("PLD is alive");
	}
	logger->Info("Sending three opcode self check");
	bool selfCheck = PLDSelfCheck();
	if(selfCheck){
		logger->Info("PLD passed self check");
	}
	ASSERT_TRUE(testAlive && selfCheck);
	usleep(1000000);

	// test PLD health and status
	PLDServer * pldServer = static_cast<PLDServer *> (Factory::GetInstance(PLD_SERVER_SINGLETON));
	bool hs = pldServer->CheckHealthStatus();
	ASSERT_TRUE(hs);

	// Other opcodes verified via DITL testing
}

// test error response to unimplemented opcodes
TEST(testCET, testPLDError){
	Logger * logger = static_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
	logger->threshold = LOGGER_LEVEL_WARN;

	prepPowerGPIOs();
	startSPIServer();
	CDHServer * cdhServer = static_cast<CDHServer *> (Factory::GetInstance(CDH_SERVER_SINGLETON));
	cdhServer->subPowerOn(HARDWARE_LOCATION_PLD);
	usleep(4000000);

	ACPPacket * testCMD;
	ACPPacket * testRet;

	for(uint16 i = LED_RATE_DATA + 1; i < TEST_ALIVE_CMD; i++){
		testCMD = new ACPPacket(SERVER_LOCATION_PLD, HARDWARE_LOCATION_PLD, i);
		testRet = DispatchPacket(testCMD);
		EXPECT_EQ(testRet->getErrorStatus(), 0xAA);
		if(!(testRet->getErrorStatus() == 0xAA)){
			printf("Opcode: %d\n", i);
		}
		usleep(100000);
	}

	for(uint16 i = SUBSYSTEM_RESET_CMD + 1; i < PLD_CMD_MIN; i++){
		testCMD = new ACPPacket(SERVER_LOCATION_PLD, HARDWARE_LOCATION_PLD, i);
		testRet = DispatchPacket(testCMD);
		EXPECT_EQ(testRet->getErrorStatus(), 0xAA);
		if(!(testRet->getErrorStatus() == 0xAA)){
			printf("Opcode: %d\n", i);
		}
		usleep(100000);
	}

	for(uint16 i = PLD_CMD_MAX; i < SUBSYSTEM_CFG_MIN; i++){
		testCMD = new ACPPacket(SERVER_LOCATION_PLD, HARDWARE_LOCATION_PLD, i);
		testRet = DispatchPacket(testCMD);
		EXPECT_EQ(testRet->getErrorStatus(), 0xAA);
		if(!(testRet->getErrorStatus() == 0xAA)){
			printf("Opcode: %d\n", i);
		}
		usleep(100000);
	}

	for(uint16 i = PLD_CONFIG_MAX; i < SUBSYSTEM_DAT_MIN; i++){
		testCMD = new ACPPacket(SERVER_LOCATION_PLD, HARDWARE_LOCATION_PLD, i);
		testRet = DispatchPacket(testCMD);
		EXPECT_EQ(testRet->getErrorStatus(), 0xAA);
		if(!(testRet->getErrorStatus() == 0xAA)){
			printf("Opcode: %d\n", i);
		}
		usleep(100000);
	}

	for(uint16 i = PLD_DATA_MAX; i < SUBSYSTEM_OPCODE_MAX + 1; i++){
		testCMD = new ACPPacket(SERVER_LOCATION_PLD, HARDWARE_LOCATION_PLD, i);
		testRet = DispatchPacket(testCMD);
		EXPECT_EQ(testRet->getErrorStatus(), 0xAA);
		if(!(testRet->getErrorStatus() == 0xAA)){
			printf("Opcode: %d\n", i);
		}
		usleep(100000);
	}
}

TEST(testCET, testEPS){
	Logger * logger = static_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
	startSPIServer();
	usleep(1000000);

	// ----- Run the self check for EPS (LED On, LED Rate, LED data)
	logger->Info("Checking if EPS is alive");
	bool testAlive = EPSTestAlive();
	if(testAlive){
		logger->Info("EPS is alive");
	}
	logger->Info("Sending three opcode self check");
	bool selfCheck = EPSSelfCheck();
	if(selfCheck){
		logger->Info("EPS passed self check");
	}
	ASSERT_TRUE(testAlive && selfCheck);
	usleep(1000000);

	// test EPS health and status
	EPSServer * epsServer = static_cast<EPSServer *> (Factory::GetInstance(EPS_SERVER_SINGLETON));
	bool hs = epsServer->CheckHealthStatus();
	ASSERT_TRUE(hs);

	// EPS reset must be verified independently
}

TEST(testCET, testEPSReset){
	startSPIServer();
	usleep(1000000);

	EPSPowerCycle();
}

// test error response to unimplemented opcodes
TEST(testCET, testEPSError){
	Logger * logger = static_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
	logger->threshold = LOGGER_LEVEL_WARN;

	prepPowerGPIOs();
	startSPIServer();
	CDHServer * cdhServer = static_cast<CDHServer *> (Factory::GetInstance(CDH_SERVER_SINGLETON));
	cdhServer->subPowerOn(HARDWARE_LOCATION_EPS);
	usleep(4000000);

	ACPPacket * testCMD;
	ACPPacket * testRet;

	for(uint16 i = LED_RATE_DATA + 1; i < TEST_ALIVE_CMD; i++){
		testCMD = new ACPPacket(SERVER_LOCATION_EPS, HARDWARE_LOCATION_EPS, i);
		testRet = DispatchPacket(testCMD);
		EXPECT_EQ(testRet->getErrorStatus(), 0xAA);
		if(!(testRet->getErrorStatus() == 0xAA)){
			printf("Opcode: %d\n", i);
		}
		usleep(100000);
	}

	for(uint16 i = TEST_ALIVE_CMD + 1; i < HEALTH_STATUS_CMD; i++){
		testCMD = new ACPPacket(SERVER_LOCATION_EPS, HARDWARE_LOCATION_EPS, i);
		testRet = DispatchPacket(testCMD);
		EXPECT_EQ(testRet->getErrorStatus(), 0xAA);
		if(!(testRet->getErrorStatus() == 0xAA)){
			printf("Opcode: %d\n", i);
		}
		usleep(100000);
	}

	for(uint16 i = SUBSYSTEM_RESET_CMD + 1; i < EPS_OPCODE_MIN; i++){
		testCMD = new ACPPacket(SERVER_LOCATION_EPS, HARDWARE_LOCATION_EPS, i);
		testRet = DispatchPacket(testCMD);
		EXPECT_EQ(testRet->getErrorStatus(), 0xAA);
		if(!(testRet->getErrorStatus() == 0xAA)){
			printf("Opcode: %d\n", i);
		}
		usleep(100000);
	}

	for(uint16 i = EPS_CMD_MAX + 1; i < SUBSYSTEM_OPCODE_MAX + 1; i++){
		testCMD = new ACPPacket(SERVER_LOCATION_EPS, HARDWARE_LOCATION_EPS, i);
		testRet = DispatchPacket(testCMD);
		EXPECT_EQ(testRet->getErrorStatus(), 0xAA);
		if(!(testRet->getErrorStatus() == 0xAA)){
			printf("Opcode: %d\n", i);
		}
		usleep(100000);
	}
}
