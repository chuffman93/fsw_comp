#include "subystem/CDH.h"


CDH::CDH(std::vector<HotSwapInterface*> * hotswaps, std::vector<PowerMonitorInterface*> * powermonitors, std::vector<TempInterface*> * thermalsensors, I2CManager& i2c);
CDH::~CDH();

//Will set up the Gpio lines and the acp devices
bool CDH::initialize(){


	return true;
}

//Handles any mode transition needs as well as any needs for tasks to be done in a mode.
void CDH::handleMode(FSWMode transition){

}

//Handles the capturing and storing of the health and status for a subsystem (Maybe find someway to implement the autocoding stuff?)
void CDH::getHealthStatus(){

}

ACPPacket CDH::sendOpcode(uint8_t opcode, std::vector<uint8_t> buffer){

}

//Various methods for data collection
void CDH::collectHotswap(){

}

void CDH::collectPowermon(){

}

void CDH::collectTherm(){

}

