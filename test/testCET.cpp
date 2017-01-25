#include "gtest/gtest.h"
#include "core/Factory.h"
#include "core/StdTypes.h"
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

// ------------------------------- Subsystem Tests -------------------------------

TEST(testCET, testACS){
	// ----- Run the self check for ACS (LED On, LED Rate, LED data)
	bool selfTest = true;
	selfTest &= ACSSelfCheck();
	ASSERT_TRUE(selfTest);
	usleep(1000000);

	// -----Test reaction wheel and torque rod
	bool driverTest = true;
	driverTest &= ACSTestDriver(1,0.04,0.5);
	usleep(5000000); // run for 5 seconds
	driverTest &= ACSTestDriver(1,0.0,0.0);
	ASSERT_TRUE(driverTest);
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
	bool beaconTest = true;
	prepBeacon();
	beaconTest &= COMSendBeacon();
	ASSERT_TRUE(beaconTest);
}

TEST(testCET, testPLD){
	// ----- Run the self check for PLD (LED On, LED Rate, LED data)
	bool selfTest = true;
	selfTest &= PLDSelfCheck();
	ASSERT_TRUE(selfTest);
	usleep(1000000);

	// Other opcodes verified via DITL testing
}

TEST(testCET, testEPS){
	// ----- Run the self check for EPS (LED On, LED Rate, LED data)
	bool selfTest = true;
	selfTest &= EPSSelfCheck();
	ASSERT_TRUE(selfTest);
	usleep(1000000);

	// EPS reset must be verified independently
}
