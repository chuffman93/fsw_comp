#include "gtest/gtest.h"
#include "core/Factory.h"
#include "core/StdTypes.h"
#include "core/WatchdogManager.h"
#include "servers/FMGServer.h"

using namespace std;
using namespace AllStar::Core;
using namespace AllStar::Servers;

TEST(DISABLED_TestFMGServer, testPacketDestinations){
	//Factory::GetInstance(WATCHDOG_MANAGER_SINGLETON);
	FMGServer * fmgServer = dynamic_cast <FMGServer *> (Factory::GetInstance(FMG_SERVER_SINGLETON));
	WatchdogManager::StartServer(fmgServer, 0,	true);

	for (int iter = 0; iter < 10; iter++) {
		FilePacket myPacket2;
		myPacket2.buffer = "12345678901234567890123456789012345678901234567890";
		myPacket2.dest = DESTINATION_HST;
		fmgServer->FileQueue.push(myPacket2);

		FilePacket myPacket3;
		myPacket3.buffer = "12345678901234567890123456789012345678901234567890";
		myPacket3.dest = DESTINATION_MOD;
		fmgServer->FileQueue.push(myPacket3);

		FilePacket myPacket4;
		myPacket4.buffer = "12345678901234567890123456789012345678901234567890";
		myPacket4.dest = DESTINATION_SWP;
		fmgServer->FileQueue.push(myPacket4);

		FilePacket myPacket5;
		myPacket5.buffer = "12345678901234567890123456789012345678901234567890";
		myPacket5.dest = DESTINATION_ERR;
		fmgServer->FileQueue.push(myPacket5);

		FilePacket myPacket6;
		myPacket6.buffer = "12345678901234567890123456789012345678901234567890";
		myPacket6.dest = DESTINATION_DGN;
		fmgServer->FileQueue.push(myPacket6);

		FilePacket myPacket7;
		myPacket7.buffer = "12345678901234567890123456789012345678901234567890";
		myPacket7.dest = DESTINATION_GEN;
		fmgServer->FileQueue.push(myPacket7);

		FilePacket myPacket8;
		myPacket8.buffer = "12345678901234567890123456789012345678901234567890";
		myPacket8.dest = DESTINATION_FSS;
		fmgServer->FileQueue.push(myPacket8);

		FilePacket myPacket9;
		myPacket9.buffer = "12345678901234567890123456789012345678901234567890";
		myPacket9.dest = DESTINATION_SSS;
		fmgServer->FileQueue.push(myPacket9);

		FilePacket myPacket10;
		myPacket10.buffer = "12345678901234567890123456789012345678901234567890";
		myPacket10.dest = DESTINATION_RAD;
		fmgServer->FileQueue.push(myPacket10);
	}

	while (fmgServer->FileQueue.size() > 0) {
		usleep(100000);
	}

	// Need to manually verify that files exist

}

TEST(DISABLED_TestFMGServer, testNoDestination){
	FMGServer * fmgServer = dynamic_cast <FMGServer *> (Factory::GetInstance(FMG_SERVER_SINGLETON));
	WatchdogManager::StartServer(fmgServer, 0,	true);

	for (int iter = 0; iter < 10; iter++) {
		FilePacket myPacket1;
		myPacket1.buffer = "no dest specified";
		fmgServer->FileQueue.push(myPacket1);
	}

	// Need to manually verify that no files were created
}

TEST(DISABLED_TestFMGServer, testLongMessage){
	FMGServer * fmgServer = dynamic_cast <FMGServer *> (Factory::GetInstance(FMG_SERVER_SINGLETON));
	WatchdogManager::StartServer(fmgServer, 0,	true);

	for (int iter = 0; iter < 10; iter++) {
		FilePacket myPacket11;
		myPacket11.buffer = "this is a very long message. There is lots and lots and lots of data here! Oh no! What am I possibly going to do with all this data?? Who knows? But one thing is for sure... There is a hell of a lot of data here. Jeez, it just keeps on coming doesn't it. Yessir. Just keeps on coming. Why are you still reading this buffer anyway? Don't you have better things to be doing than reading a buffer??? Okay, I'm done.";
		myPacket11.dest = DESTINATION_GEN;
		fmgServer->FileQueue.push(myPacket11);
	}

	// Need to manually verify that files exist
}
