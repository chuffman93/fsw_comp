/*
 * test_ScheduleManager.cpp
 *
 *  Created on: Jan 9, 2018
 *      Author: alpa3119
 */

#include "test/catch.hpp"
#include "core/ScheduleManager.h"
#include "core/FileManager.h"
#include "util/TimeKeeper.h"
#include "test/testmacros.h"
#include "core/FileSystem.h"

TEST_CASE("Schedule Manager: test check new mode", "[.][scheduler]"){
	ScheduleManager sman;
	ScheduleStruct sch;

	sch.mode = Mode_Payload;
	sch.timeSinceEpoch = getCurrentTime() + 2;
	sch.duration = 2;
	sman.ScheduleQueue.push(sch);

	sch.mode = Mode_Com;
	sch.timeSinceEpoch = getCurrentTime() + 2;
	sch.duration = 2;
	sman.ScheduleQueue.push(sch);

	sman.CurrentMode = Mode_Bus;
	sman.currentSchedule = sman.ScheduleQueue.front();

	REQUIRE(sman.checkNewMode() == Mode_Bus);
	sleep(2);
	REQUIRE(sman.checkNewMode() == Mode_Payload);
	sleep(2);
	REQUIRE(sman.checkNewMode() == Mode_Bus);
	sleep(2);
	REQUIRE(sman.checkNewMode() == Mode_Com);
	sleep(2);
	REQUIRE(sman.checkNewMode() == Mode_Bus);
	REQUIRE(sman.ScheduleQueue.empty());
	REQUIRE(sman.checkNewMode() == Mode_Reset);

}

TEST_CASE("Schedule Manager: test load schedule", "[scheduler]"){
	uint8_t mode1 = 2;
	uint8_t mode2 = 3;
	uint8_t mode3 = 2;
	uint8_t mode4 = 3;
	uint32_t epoch1 = getCurrentTime() + 2;
	uint32_t epoch2 = getCurrentTime() + 3;
	uint32_t epoch3 = getCurrentTime() + 4;
	uint32_t epoch4 = getCurrentTime() + 5;
	uint32_t duration1 = getCurrentTime() + 2;
	uint32_t duration2 = getCurrentTime() + 3;
	uint32_t duration3 = getCurrentTime() + 4;
	uint32_t duration4 = getCurrentTime() + 5;
	SECTION("Test correct schedule file"){
		ByteStream bs;
		bs << mode1 << epoch1 << duration1 << mode2 << epoch2 << duration2 << mode3 << epoch3 << duration3 << mode4 << epoch4 << duration4;
		std:vector<uint8_t> newSchedule = bs.vec();

		for (std::vector<uint8_t>::iterator i = newSchedule.begin(); i != newSchedule.end(); i++){
			std::cout << (int)*i << " ";
		}
		std::cout << endl;

		FileManager fm;
		fm.writeToFile("home/dum/dummmySchedule", newSchedule);
		ScheduleManager sman;
		sman.loadSchedule("home/dum/dummmySchedule");

		REQUIRE(!sman.ScheduleQueue.empty());
		//REQUIRE(sman.ScheduleQueue.front().mode == mode1);
		REQUIRE(sman.ScheduleQueue.front().timeSinceEpoch == epoch1);
		REQUIRE(sman.ScheduleQueue.front().duration == duration1);
		sman.ScheduleQueue.pop();
		REQUIRE(sman.ScheduleQueue.front().mode == mode2);
		REQUIRE(sman.ScheduleQueue.front().timeSinceEpoch == epoch2);
		REQUIRE(sman.ScheduleQueue.front().duration == duration2);
		sman.ScheduleQueue.pop();
		REQUIRE(sman.ScheduleQueue.front().mode == mode3);
		REQUIRE(sman.ScheduleQueue.front().timeSinceEpoch == epoch3);
		REQUIRE(sman.ScheduleQueue.front().duration == duration3);
		sman.ScheduleQueue.pop();
		REQUIRE(sman.ScheduleQueue.front().mode == mode4);
		REQUIRE(sman.ScheduleQueue.front().timeSinceEpoch == epoch4);
		REQUIRE(sman.ScheduleQueue.front().duration == duration4);
		sman.ScheduleQueue.pop();
		REQUIRE(sman.ScheduleQueue.empty());


	}

}
