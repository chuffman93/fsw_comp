/*
 * softwareTest.h
 *
 *  Created on: Feb 3, 2018
 *      Author: alpa3119
 */

#ifndef SOFTWARETEST_H_
#define SOFTWARETEST_H_


//#include "util/ByteStream.h"
#include "core/FileManager.h"

struct DITLSchedule{
	DITLSchedule(uint8_t mode,uint32_t epoch,uint32_t duration) : mode(mode), epoch(epoch), duration(duration) {};
	uint8_t mode;
	uint32_t epoch;
	uint32_t duration;
};


class SoftwareTest {
public:
	SoftwareTest();
	void runDITL();
private:
	void createSchedule(std::vector<DITLSchedule> nextSchedule);
	void createIEF(std::string nextIEF);
	void sendSOT();
	void sendIEF();
	void sendSchedule();
	std::vector<uint8_t> serialize(DITLSchedule bytes);
	LogTags tags;
};


#endif /* SOFTWARETEST_H_ */
