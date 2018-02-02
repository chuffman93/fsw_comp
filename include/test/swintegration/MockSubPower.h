/*
 * MockSubPower.h
 *
 *  Created on: Jan 29, 2018
 *      Author: cyborg9
 */

#ifndef MOCKSUBPOWER_H_
#define MOCKSUBPOWER_H_

#include "interfaces/SubPowerInterface.h"

class MockSubPower: public SubPowerInterface{
public:
	MockSubPower(std::string instance);
	~MockSubPower();

	void configDelay(size_t resetdelay, size_t startupdelay);

	void powerOff();
	void powerOn();
	void reset();

	bool faultOccurred();
private:
	//! Tags to use for logging
	LogTags tags;
};



#endif /* MOCKSUBPOWER_H_ */
