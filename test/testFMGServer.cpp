#include "gtest/gtest.h"
#include "core/Factory.h"
#include "core/StdTypes.h"
#include "servers/FMGServer.h"

using namespace std;
using namespace AllStar::Core;
using namespace AllStar::Servers;

TEST(DISABLED_TestFMGServer, testPacketDestinations){
	FMGServer * fileServer = dynamic_cast <FMGServer *> (Factory::GetInstance(FMG_SERVER_SINGLETON));

	FilePacket myPacket2;
	myPacket2.buffer = "to health!";
	myPacket2.dest = DESTINATION_HST;
	fileServer->FileQueue.push(myPacket2);

	FilePacket myPacket3;
	myPacket3.buffer = "to mode!";
	myPacket3.dest = DESTINATION_MOD;
	fileServer->FileQueue.push(myPacket3);

	FilePacket myPacket4;
	myPacket4.buffer = "to hotswap!";
	myPacket4.dest = DESTINATION_SWP;
	fileServer->FileQueue.push(myPacket4);

	FilePacket myPacket5;
	myPacket5.buffer = "to error!";
	myPacket5.dest = DESTINATION_ERR;
	fileServer->FileQueue.push(myPacket5);

	FilePacket myPacket6;
	myPacket6.buffer = "to diagnostic!";
	myPacket6.dest = DESTINATION_DGN;
	fileServer->FileQueue.push(myPacket6);

	FilePacket myPacket7;
	myPacket7.buffer = "to general!";
	myPacket7.dest = DESTINATION_GEN;
	fileServer->FileQueue.push(myPacket7);

	FilePacket myPacket8;
	myPacket8.buffer = "to file system stats!";
	myPacket8.dest = DESTINATION_FSS;
	fileServer->FileQueue.push(myPacket8);

	FilePacket myPacket9;
	myPacket9.buffer = "to science stats!";
	myPacket9.dest = DESTINATION_SSS;
	fileServer->FileQueue.push(myPacket9);

	FilePacket myPacket10;
	myPacket10.buffer = "to rad!";
	myPacket10.dest = DESTINATION_RAD;
	fileServer->FileQueue.push(myPacket10);

	ASSERT_EQ(1,1);
}

TEST(DISABLED_TestFMGServer, testNoDestination){
	FMGServer * fileServer = dynamic_cast <FMGServer *> (Factory::GetInstance(FMG_SERVER_SINGLETON));

	FilePacket myPacket1;
	myPacket1.buffer = "no dest specified";
	fileServer->FileQueue.push(myPacket1);
}

TEST(DISABLED_TestFMGServer, testLongMessage){
	FMGServer * fileServer = dynamic_cast <FMGServer *> (Factory::GetInstance(FMG_SERVER_SINGLETON));

	FilePacket myPacket11;
	myPacket11.buffer = "this is a very long message. There is lots and lots and lots of data here! Oh no! What am I possibly going to do with all this data?? Who knows? But one thing is for sure... There is a hell of a lot of data here. Jeez, it just keeps on coming doesn't it. Yessir. Just keeps on coming. Why are you still reading this buffer anyway? Don't you have better things to be doing than reading a buffer??? Okay, I'm done.";
	myPacket11.dest = DESTINATION_GEN;
	fileServer->FileQueue.push(myPacket11);
}
