/*******************************************************
 *
 * THIS FILE IS AUTO-GENERATED, DO NOT EDIT THIS FILE!
 * To edit this file, edit /TLM/struct.txt, then
 * execute /TLM/struct_autocoder/autocode.sh
 *
 ******************************************************/

#ifndef Structs_H_
#define Structs_H_

#include "util/serialize.h"
#include "core/StdTypes.h"

namespace AllStar{
namespace Servers{

class CDHStatus : public Serialize {
public:
  const static int size = 16;
  CDHStatus();
  CDHStatus(float cpu1, float cpu5, float cpu15, float memory);
  float cpu1; // 1  minute loads (% of CPU)
  float cpu5; // 5  minute loads (% of CPU)
  float cpu15; // 15 minute loads (% of CPU)
  float memory; // % of memory
  void serialize(void);
  void deserialize(void);
};

class ACSStatus : public Serialize {
public:
  const static int size = 28;
  ACSStatus();
  ACSStatus(uint32 MRP_X, uint32 MRP_Y, uint32 MRP_Z, uint32 ST_Status, uint32 RW_Speed_X, uint32 RW_Speed_Y, uint32 RW_Speed_Z);
  uint32 MRP_X; 
  uint32 MRP_Y; 
  uint32 MRP_Z; 
  uint32 ST_Status; 
  uint32 RW_Speed_X; 
  uint32 RW_Speed_Y; 
  uint32 RW_Speed_Z; 
  void serialize(void);
  void deserialize(void);
};

class PLDStatus : public Serialize {
public:
  const static int size = 17;
  PLDStatus();
  PLDStatus(uint8 powerFault, uint16 motorSpeed, uint8 thermistors[10], uint8 adcDataWorking, uint16 control, uint8 byteSize);
  uint8 powerFault; 
  uint16 motorSpeed; 
  uint8 thermistors[10]; 
  uint8 adcDataWorking; 
  uint16 control; 
  uint8 byteSize; 
  void serialize(void);
  void deserialize(void);
};

class EPSStatus : public Serialize {
public:
  const static int size = 36;
  EPSStatus();
  EPSStatus(uint16 current3v3, uint16 voltage3v3, uint16 currentVbat, uint16 voltageVbat, uint16 current12v, uint16 voltage12v, uint16 remainingCapacity, uint16 battCurrent, uint16 battVoltage, uint16 battStatus, uint16 frangCurrent, uint16 frangVoltage, uint16 convCurrentX, uint16 convThreshX, uint16 convCurrentY, uint16 convThreshY, uint16 convCurrentW, uint16 convThreshW);
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
  void serialize(void);
  void deserialize(void);
};

class CMDConfig : public Serialize {
public:
  const static int size = 20;
  CMDConfig();
  CMDConfig(int32 resetPeriod, int32 fileChunkSize, int32 maxDownlinkSize, int32 beaconPeriod, int32 increasedBeaconPeriod);
  int32 resetPeriod; // seconds
  int32 fileChunkSize; 
  int32 maxDownlinkSize; 
  int32 beaconPeriod; 
  int32 increasedBeaconPeriod; 
  void serialize(void);
  void deserialize(void);
};

class EPSConfig : public Serialize {
public:
  const static int size = 6;
  EPSConfig();
  EPSConfig(uint16 minCOMCharge, uint16 minPLDCharge, uint16 minDSTCharge);
  uint16 minCOMCharge; 
  uint16 minPLDCharge; 
  uint16 minDSTCharge; 
  void serialize(void);
  void deserialize(void);
};

}
}

#endif
