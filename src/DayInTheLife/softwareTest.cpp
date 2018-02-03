/*
 * softwareTest.cpp
 *
 *  Created on: Feb 2, 2018
 *      Author: alpa3119
 */
#include "DayInTheLife/softwareTest.h"

#include <iostream>

SoftwareTest::SoftwareTest(){
	//LogTag tags;
	tags += LogTag("Name", "SoftwareDITL");
}


void SoftwareTest::createSchedule(std::vector<DITLSchedule> nextSchedule){
	std::vector<uint8_t> buff;
	std::vector<DITLSchedule>::iterator it;
	for (it = nextSchedule.begin(); it != nextSchedule.end(); it++){
		std::vector<uint8_t> temp = serialize(*(it));
		for (int i = 0; i < 3; i++){
			buff.push_back(temp[i]);
		}
	}
	FileManager::writeToFile("SCH",buff);
}

void SoftwareTest::createIEF(std::string nextIEF){
	std::ofstream out("IEF.txt");
	out << nextIEF;
	out.close();
}

void SoftwareTest::sendSOT(){
	std::vector<uint8_t> buffer;
	FileManager::writeToFile("SOT",buffer);
	FileManager::moveFile("SOT","./home/.uplink/SOT");
}

void SoftwareTest::sendIEF(){
	FileManager::moveFile("IEF.txt","./home/.uplink/IEF.txt");
}

void SoftwareTest::sendSchedule(){
	FileManager::moveFile("SCH","./home/.uplink/SCH");
}

std::vector<uint8_t> SoftwareTest::serialize(DITLSchedule bytes){
	std::vector<uint8_t> serialized;
	serialized.push_back(bytes.mode);
	serialized.push_back((uint8_t)bytes.epoch);
	serialized.push_back((uint8_t)bytes.duration);
	return serialized;

}

void SoftwareTest::runDITL(){


	Logger::Stream(LEVEL_INFO,tags)<< "----------Running Integrated Software Ground Simulation--------------";

	DITLSchedule sch1_1(2,60,120);
	std::vector<DITLSchedule> nextSchedule;
	nextSchedule.push_back(sch1_1);
	createSchedule(nextSchedule);
	createIEF("");

	//sleep(60);

	Logger::Stream(LEVEL_INFO,tags)<< "------------Sending Next Schedule for Payload Mode--------";
	sendSOT();
	sendSchedule();
	sendIEF();


}



