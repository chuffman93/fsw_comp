/*
 * architecture.h
 *
 *  Created on: Jan 22, 2018
 *      Author: alpa3119
 */

#ifndef ARCHITECTURE_H_
#define ARCHITECTURE_H_

enum Subsystem {
	ACS,
	CDH,
	COM,
	EPS,
	GPS,
	RAD,
};

enum HalMode {
	Software_Test,
	Hardware_Test,
};

class Architecture{
public:
	static void buildSubsystem(Subsystem subsystem);
	static void setHalMode(HalMode mode);

private:
	static ACS* acs;
	static CDH* cdh;
	static COM* com;
	static EPS* eps;
	static GPS* gps;
	static RAD* rad;
	static HalMode mode;

};




#endif /* ARCHITECTURE_H_ */
