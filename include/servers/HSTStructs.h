#ifndef HSTSTRUCTS_H_
#define HSTSTRUCTS_H_

namespace AllStar{
namespace Servers{

struct CDHStatus{
	float cpu1;   // 1  minute loads (% of CPU)
	float cpu5;   // 5  minute loads (% of CPU)
	float cpu15;  // 15 minute loads (% of CPU)
	float memory; // % of memory
};

struct ACSStatus{
	uint32 MRP_X;
	uint32 MRP_Y;
	uint32 MRP_Z;
	uint32 ST_Status;
	uint32 RW_Speed_X;
	uint32 RW_Speed_Y;
	uint32 RW_Speed_Z;
};

struct PLDStatus{
	uint8 powerFault;
	uint16 motorSpeed;
	uint8 thermistors[10];
	uint8 acdDataWorking;
	uint16 control;
	uint8 byteSize;
};

struct EPSStatus{
	uint16 current3v3;
	uint16 voltage3v3;
	uint16 currentVbat;
	uint16 voltageVbat;
	uint16 current12v;
	uint16 voltage12v;
	uint16 remainingCapacity;
	uint16 battCurrent;
	uint16 battVoltage;
	uint16 battStatus;
	uint16 frangCurrent;
	uint16 frangVoltage;
	uint16 convCurrentX;
	uint16 convThreshX;
	uint16 convCurrentY;
	uint16 convThreshY;
	uint16 convCurrentW;
	uint16 convThreshW;
};


}
}

#endif
