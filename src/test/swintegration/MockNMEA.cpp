#include "test/swintegration/MockNMEA.h"

static UARTManager dummyu("");

MockNMEA::MockNMEA()
:NMEAInterface(dummyu)
{
	tags += LogTag("Name", "MockNMEA");
}
MockNMEA::~MockNMEA(){}

std::string MockNMEA::getString(){
	return "BESTXYZA,COM1,0,55.0,FINESTEERING,1419,340033.000,02000040,d821,2724;SOL_COMPUTED,NARROW_INT,-1634531.5683,-3664618.0326,4942496.3270,0.0099,0.0219,0.0115,SOL_COMPUTED,NARROW_INT,0.0011,-0.0049,-0.0001,0.0199,0.0439,0.0230,\"AAAA\",0.250,1.000,0.000,12,11,11,11,0,01,0,33*e9eafeca";
}

void MockNMEA::sendCommand(std::string str){
	Logger::Stream(LEVEL_INFO, tags) << "Sending \"" << str << "\"";
}
