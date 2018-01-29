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
}

NMEAInterface::~NMEAInterface(){}

/*!
 * Rewrite of the software in old FSW since I don't want to deal with writting better parsing code
 * \return a NMEA string
 */
std::string NMEAInterface::getString(){
	//TODO: timeout?
	std::string data;
	while(uart.readData() != '#');
	do{
		data += uart.readData();
	}while(*data.rbegin() != '*');
	//TODO: this was 10 in old fsw, may need to change when we test it
	for(int i = 0; i < 8; i++){
		data += uart.readData();
	}

	Logger::Stream(LEVEL_DEBUG, tags) << "Read: " << data;
	return data;
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


