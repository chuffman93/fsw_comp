/*
 * NMEAInterface.cpp
 *
 *  Created on: Jan 1, 2018
 *      Author: cyborg9
 */

#include "interfaces/NMEAInterface.h"

/*!
 * Initialize the interface with a uart device
 * \param uart the uart device
 */
NMEAInterface::NMEAInterface(UARTManager& uart)
:uart(uart)
{
	tags += LogTag("Name", "NMEAInterface");
	databuffer = "";
	state = WAIT_FOR_HASH;
}

NMEAInterface::~NMEAInterface(){}

/*!
 * Rewrite of the software in old FSW since I don't want to deal with writting better parsing code
 * \return a NMEA string
 */
std::string NMEAInterface::getString(){
	char c;
	do{
		c = uart.readData();
		if(c == 0){
			Logger::Stream(LEVEL_DEBUG, tags) << "Nothing to be read";
			return "";
		}
		databuffer += c;
	}while(c != '\n');
	uart.readData();
	Logger::Stream(LEVEL_DEBUG, tags) << "Read: " << databuffer.c_str();

	std::string ret = databuffer;
	databuffer = "";
	return ret;
}

/*!
 * Pretty much just an adapter
 * \param str The command to send
 */
void NMEAInterface::sendCommand(std::string str){
	Logger::Stream(LEVEL_DEBUG, tags) << "Sending: " << str;
	std::vector<uint8_t> data(str.begin(), str.end());
	uart.writeData(data);
}


