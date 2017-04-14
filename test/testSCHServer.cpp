#include "gtest/gtest.h"
#include "core/Factory.h"
#include "core/StdTypes.h"
#include "core/WatchdogManager.h"
#include "servers/SCHServer.h"
#include "servers/FileSystem.h"
#include <fstream>


using namespace std;
using namespace AllStar::Core;
using namespace AllStar::Servers;



TEST(TestSCHServer, testParse){
	SCHServer * schServer = dynamic_cast <SCHServer *> (Factory::GetInstance(SCH_SERVER_SINGLETON));

	SCHServer::SCHItem item = schServer->ParseLine("1.2345672.3456783.456789012345678123456789\n");
	ASSERT_EQ(item.latitude, 1.234567);
	ASSERT_EQ(item.longitude, 2.345678);
	ASSERT_EQ(item.radius, 3.456789);
	ASSERT_EQ(item.enter_mode, 0);
	ASSERT_EQ(item.timeout, 12345678);
	ASSERT_EQ(item.mode, 1);
	ASSERT_EQ(item.duration, 23456789);
}

TEST(TestSCHServer, testSchedFile){
	SCHServer * schServer = dynamic_cast <SCHServer *> (Factory::GetInstance(SCH_SERVER_SINGLETON));

	rename(SCH_SCHEDULE_FILE, SCH_SCHEDULE_FILE ".test.bak");

	ofstream file;
	file.open(SCH_SCHEDULE_FILE);

	file << "1.2345672.3456783.456789012345678123456789\n";
	file << "9.8765438.7654327.654321198765432187654321\n";

	file.close();

	int err = schServer->LoadNextSchedule();
	ASSERT_EQ(err, 1);

	SCHServer::SCHItem sched1 = schServer->currentSchedule.front();
	ASSERT_EQ(sched1.latitude, 1.234567);
	ASSERT_EQ(sched1.longitude, 2.345678);
	ASSERT_EQ(sched1.radius, 3.456789);
	ASSERT_EQ(sched1.enter_mode, 0);
	ASSERT_EQ(sched1.timeout, 12345678);
	ASSERT_EQ(sched1.mode, 1);
	ASSERT_EQ(sched1.duration, 23456789);
	schServer->currentSchedule.pop_front();

	SCHServer::SCHItem sched2 = schServer->currentSchedule.front();
	ASSERT_EQ(sched2.latitude, 9.876543);
	ASSERT_EQ(sched2.longitude, 8.765432);
	ASSERT_EQ(sched2.radius, 7.654321);
	ASSERT_EQ(sched2.enter_mode, 1);
	ASSERT_EQ(sched2.timeout, 98765432);
	ASSERT_EQ(sched2.mode, 1);
	ASSERT_EQ(sched2.duration, 87654321);
	schServer->currentSchedule.pop_front();

	remove(SCH_SCHEDULE_FILE);
	rename(SCH_SCHEDULE_FILE ".test.bak", SCH_SCHEDULE_FILE);
}


TEST(TestSCHServer, testNoSchedFile){
	SCHServer * schServer = dynamic_cast <SCHServer *> (Factory::GetInstance(SCH_SERVER_SINGLETON));

	rename(SCH_SCHEDULE_FILE, SCH_SCHEDULE_FILE ".test.bak");

	int err = schServer->LoadNextSchedule();
	ASSERT_EQ(err, -1);

	rename(SCH_SCHEDULE_FILE ".test.bak", SCH_SCHEDULE_FILE);
}


TEST(TestSCHServer, testBadSchedFile){
	SCHServer * schServer = dynamic_cast <SCHServer *> (Factory::GetInstance(SCH_SERVER_SINGLETON));

	rename(SCH_SCHEDULE_FILE, SCH_SCHEDULE_FILE ".test.bak");
	ofstream file;
	file.open(SCH_SCHEDULE_FILE);
	file << "asdf";

	file.close();

	int err = schServer->LoadNextSchedule();
	ASSERT_EQ(err, -3);

	remove(SCH_SCHEDULE_FILE);
	rename(SCH_SCHEDULE_FILE ".test.bak", SCH_SCHEDULE_FILE);
}

TEST(TestSCHServer, testGeneric){
	SCHServer * schServer = dynamic_cast <SCHServer *> (Factory::GetInstance(SCH_SERVER_SINGLETON));
	WatchdogManager::StartServer(schServer, 0,	true);

	usleep(10000000);

}
