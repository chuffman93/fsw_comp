#include "test/catch.hpp"
#include "util/Logger.h"
#include "FSWThreads.h"
#include "core/Architecture.h"

using namespace std;

TEST_CASE("TEST RAD and GPS integration","[hardware][radgps]"){
	Architecture::buildTime();
	FileManager::handleConfig();
	Logger::setMode(MODE_PW);
	Logger::setLevel(LEVEL_INFO);
	Logger::registerThread("MAIN");
	Logger::registerFilter(LogTag("Name", "ScheduleManager"), LEVEL_DEBUG);
	Logger::registerFilter(LogTag("Name", "FileManager"), LEVEL_DEBUG);
	//Logger::registerFilter(LogTag("Name", "NMEAInterface"), LEVEL_DEBUG);
	Logger::registerFilter(LogTag("Name", "ExternP"), LEVEL_DEBUG);
	Logger::registerFilter(LogTag("Name", "GPS"), LEVEL_DEBUG);
	Logger::registerFilter(LogTag("Name", "RAD"), LEVEL_DEBUG);
	Logger::registerFilter(LogTag("Name", "GroundCommunication"), LEVEL_DEBUG);
	Logger::log(LEVEL_FATAL, "Entering Main");

	//---------Step1: Build FSW---------------------------
	Architecture::setInterfaceMode(HARDWARE);
	Architecture::buildACS();
	Architecture::buildEPS();
	Architecture::buildRAD();
	Architecture::buildCOM();
	Architecture::buildCDH();
	Architecture::buildGPS();
	Architecture::buildScheduleManager();
	Architecture::buildBeaconManager();
	Architecture::buildGND();

	//---------Step2: Initialize HAL-----------------------
	Logger::log(LEVEL_FATAL, "Initializing HAL");
	vector<HardwareManager*> halinit = Architecture::buildHALInitVector();
	for(vector<HardwareManager*>::iterator i = halinit.begin(); i != halinit.end(); i++){
		(*i)->initialize();
	}

	//---------Step3: Initialize Subsystems-----------------------
	vector<SubsystemBase*> subinit = Architecture::buildInitVector();
	Logger::log(LEVEL_FATAL, "Initializing Subsystems");
	for(vector<SubsystemBase*>::iterator i = subinit.begin(); i != subinit.end(); i++){
		(*i)->initialize();
	}

	//---------Step4: Initialize Watchdog-----------------------
	Watchdog watchdog(*(Architecture::getEPS()));
	Architecture::getRAD()->watchdog = &watchdog;

	//---------Step5: Initialize HS Thread-----------------------
	Thread hsThread;
	HSStruct hsargs;
	hsargs.subsystemSequence = Architecture::buildHSVector();
	hsargs.watchdog = &watchdog;
	hsThread.CreateThread(NULL, FSWThreads::HealthStatusThread, (void*)&hsargs);
	watchdog.AddThread(hsThread.GetID());

	//---------Step6: Initialize GPS Thread-----------------------
	Thread gpsThread;
	GPSStruct gpsargs;
	gpsargs.gps = Architecture::getGPS();
	gpsargs.watchdog = &watchdog;
	gpsargs.acs = Architecture::getACS();
	gpsThread.CreateThread(NULL, FSWThreads::GPSThread, (void*)&gpsargs);
	watchdog.AddThread(gpsThread.GetID());

	//---------Step8: Initialize GND Thread-----------------------

	Thread gndThread;
	GroundStruct gndargs;
	gndargs.watchdog = &watchdog;
	gndargs.gnd = Architecture::getGND();
	gndargs.scheduler = Architecture::getSchedulerManager();
	gndThread.CreateThread(NULL, FSWThreads::GroundThread, (void*)&gndargs);
	watchdog.AddThread(gndThread.GetID());


	Logger::Stream(LEVEL_ERROR) << "Starting RAD file parsing";

	RAD * rad = Architecture::getRAD();
	rad->dataFile = FileManager::createFileName(RAD_FILE_PATH);
	std::string s(RAD_TMP_DATA);
	FileManager::copyFile(s,rad->dataFile);
	int splits = rad->splitData();
	rad->tarBallData(splits);

	int rv = 0;
	hsThread.JoinThread((void*)&rv);
	gpsThread.JoinThread((void*)&rv);
	gndThread.JoinThread((void*)&rv);






}
