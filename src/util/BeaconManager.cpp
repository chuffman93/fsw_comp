/*
 * BeaconManager.cpp
 *
 *  Created on: Jan 14, 2018
 *      Author: chuffman93
 */

#include "util/BeaconManager.h"
#include "core/FileManager.h"

BeaconManager::BeaconManager(ScheduleManager* sch, ACS* acs, EPS* eps, GPS* gps, RAD* rad)
: sch(sch), acs(acs), eps(eps), gps(gps), rad(rad),beaconRate(0),increasedBeaconRate(0)
{}

BeaconManager::~BeaconManager(){}
//TODO: Finalize this!
//! Manages the collection of beacon data to be sent
void BeaconManager::sendBeacon(){
	//TODO::add GPS and CDH fields

	Beacon b;
	b.epochTime = getCurrentTime();

	timespec t;
	clock_gettime(CLOCK_REALTIME, &t);
	b.systemTime = t.tv_sec;

	b.rebootCount = FileManager::GetReboot();
	b.satelliteMode = (uint8_t)sch->getCurrentMode();
	b.currentModeEnterTime = sch->getModeEnterTime();
	b.comPassCount = sch->getComPassCount();
	b.timeSinceStarLock = acs->getTimeSinceLock();
	b.starMRP = acs->getStarMRP();
	b.targetMRP = acs->getTargetMRP();
	b.actualMRP = acs->getActualMRP();
	b.xPixel = acs->getXPixel();
	b.yPixel= acs->getYPixel();
	b.catalogID = acs->getCatalogID();
	b.numStarsFound = acs->getNumStarsFound();
	b.memory = 0; //fix
	b.cpu = 0; //fix
	b.batteryStateofCharge = eps->getBatteryStateOfCharge();
	b.xyzPosition.assign(3,0);//fix
	b.xyzVelocity.assign(3,0);//fix
	b.gpsWeek = 0;//fix
	b.gpsSecond = 0; //fix
	b.radNumber = rad->RADDataNum;


	ByteStream bs;
	bs << b.epochTime << b.systemTime <<b.rebootCount << b.satelliteMode << b.currentModeEnterTime << b.comPassCount << b.timeSinceStarLock << b.targetMRP << b.actualMRP << b.memory << b.cpu << b.batteryStateofCharge << b.xyzPosition << b.xyzVelocity << b.gpsWeek << b.gpsSecond << b.radNumber;

	std::vector<uint8_t> buff = bs.vec();


	//TODO: figure out naming convention... do we want to include timestamp and reboot count?
	FileManager::writeToFile(BEACON,buff);

}





