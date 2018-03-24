/*
 * MockNMEA.h
 *
 *  Created on: Jan 29, 2018
 *      Author: cyborg9
 */

#ifndef MOCKNMEA_H_
#define MOCKNMEA_H_

#include "interfaces/NMEAInterface.h"

class MockNMEA: public NMEAInterface{
public:
	MockNMEA();
	~MockNMEA();

	std::string getString();
	void setString(std::string fakeGPS);
	void sendCommand(std::string str);

private:
	LogTags tags;
	std::string teststr;
};



#endif /* MOCKNMEA_H_ */
