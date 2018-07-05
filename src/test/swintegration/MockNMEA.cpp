#include "test/swintegration/MockNMEA.h"

static UARTManager dummyu("");

MockNMEA::MockNMEA()
:NMEAInterface(dummyu)
{
	tags += LogTag("Name", "MockNMEA");
	teststr =  "#BESTXYZA,COM1,0,55.0,FINESTEERING,1981,141600.000,02000040,d821,2724;SOL_COMPUTED,NARROW_INT,-424512.363,-6786732.88,11179.885,0.0,0.0,0.0,SOL_COMPUTED,NARROW_INT,499.107,-29.901,7600.00,0.0,0.0,0.0,\"AAAA\",0.250,1.000,0.000,12,11,11,11,0,01,0,33*e9eafeca";
}
MockNMEA::~MockNMEA(){}

void MockNMEA::setString(std::string fakeGPS){
	Logger::Stream(LEVEL_DEBUG, tags) << "setting \"" << fakeGPS << "\"";
	teststr = fakeGPS;

}

std::string MockNMEA::getString(){
	Logger::Stream(LEVEL_DEBUG, tags) << "getting \"" << teststr << "\"";
	return teststr;
}

void MockNMEA::sendCommand(std::string str){
	Logger::Stream(LEVEL_INFO, tags) << "Sending \"" << str << "\"";
}
