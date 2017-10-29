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

class BeaconStruct : public Serialize {
public:
  const static int size = 85;
  BeaconStruct();
  BeaconStruct(int32 satTime, int32 GPSWeek, float GPSSec, double xPosition, double yPosition, double zPosition, double xVelocity, double yVelocity, double zVelocity, uint8 systemMode, uint8 subpowerStates, int16 epochNumber, uint16 radNumber, uint32 spiSent, uint32 spiDropped, uint16 batteryCap, int8 acsMode, float memory, float cpu15);
  int32 satTime; 
  int32 GPSWeek; 
  float GPSSec; 
  double xPosition; 
  double yPosition; 
  double zPosition; 
  double xVelocity; 
  double yVelocity; 
  double zVelocity; 
  uint8 systemMode; 
  uint8 subpowerStates; 
  int16 epochNumber; 
  uint16 radNumber; 
  uint32 spiSent; 
  uint32 spiDropped; 
  uint16 batteryCap; 
  int8 acsMode; 
  float memory; 
  float cpu15; 
  void serialize(void);
  void deserialize(void);
};

class CDHStatus : public Serialize {
public:
  const static int size = 20;
  CDHStatus();
  CDHStatus(int32 time, float cpu1, float cpu5, float cpu15, float memory);
  int32 time; // log the time the stats were taken
  float cpu1; // 1  minute loads (% of CPU)
  float cpu5; // 5  minute loads (% of CPU)
  float cpu15; // 15 minute loads (% of CPU)
  float memory; // % of memory
  void serialize(void);
  void deserialize(void);
};

class SWPStatus : public Serialize {
public:
  const static int size = 132;
  SWPStatus();
  SWPStatus(int32 time, float hotswaps[32]);
  int32 time; 
  float hotswaps[32]; 
  void serialize(void);
  void deserialize(void);
};

class THMStatus : public Serialize {
public:
  const static int size = 260;
  THMStatus();
  THMStatus(int32 time, float tempSensors[64]);
  int32 time; 
  float tempSensors[64]; 
  void serialize(void);
  void deserialize(void);
};

class ACSStatus : public Serialize {
public:
  const static int size = 161;
  ACSStatus();
  ACSStatus(uint8 mode, float st_mrp[3], float curr_mrp[3], float ref_mrp[3], float time_since_lock, float gyro0[3], float gryo1[3], float ang_rate[3], float mag0[3], float mag1[3], float rw_torque[3], float rw_pwm[3], float rw_speed[3], float tr_torque[3], float tr_pwm[3]);
  uint8 mode;
  float st_mrp[3];
  float curr_mrp[3]; 
  float ref_mrp[3];
  float time_since_lock; 
  float gyro0[3]; 
  float gryo1[3];
  float ang_rate[3];
  float mag0[3];
  float mag1[3]; 
  float rw_torque[3];
  float rw_pwm[3]; 
  float rw_speed[3];
  float tr_torque[3]; 
  float tr_pwm[3]; 
  void serialize(void);
  void deserialize(void);
};

class PLDStatus : public Serialize {
public:
  const static int size = 16;
  PLDStatus();
  PLDStatus(uint8 powerFault, uint16 motorSpeed, uint8 thermistors[10], uint8 adcDataWorking, uint16 control);
  uint8 powerFault; 
  uint16 motorSpeed; 
  uint8 thermistors[10]; 
  uint8 adcDataWorking; 
  uint16 control; 
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

class GPSPositionTime : public Serialize {
public:
  const static int size = 54;
  GPSPositionTime();
  GPSPositionTime(double posX, double posY, double posZ, double velX, double velY, double velZ, uint16 GPSWeek, float GPSSec);
  double posX; 
  double posY; 
  double posZ; 
  double velX; 
  double velY; 
  double velZ; 
  uint16 GPSWeek; 
  float GPSSec; 
  void serialize(void);
  void deserialize(void);
};

class GPSInertial : public Serialize {
public:
  const static int size = 55;
  GPSInertial();
  GPSInertial(double posX, double posY, double posZ, double velX, double velY, double velZ, uint16 GPSWeek, float GPSSec, uint8 isAccurate);
  double posX; 
  double posY; 
  double posZ; 
  double velX; 
  double velY; 
  double velZ; 
  uint16 GPSWeek; 
  float GPSSec; 
  uint8 isAccurate; 
  void serialize(void);
  void deserialize(void);
};

class ACSmrp : public Serialize {
public:
  const static int size = 12;
  ACSmrp();
  ACSmrp(float mrpX, float mrpY, float mrpZ);
  float mrpX; 
  float mrpY; 
  float mrpZ; 
  void serialize(void);
  void deserialize(void);
};

class CDHConfig : public Serialize {
public:
  const static int size = 4;
  CDHConfig();
  CDHConfig(int32 testConfig);
  int32 testConfig; 
  void serialize(void);
  void deserialize(void);
};

class CMDConfig : public Serialize {
public:
  const static int size = 22;
  CMDConfig();
  CMDConfig(int32 resetPeriod, int32 fileChunkSize, int32 maxDownlinkSize, int32 beaconPeriod, int32 increasedBeaconPeriod, uint16 expectedRebootDuration);
  int32 resetPeriod; // seconds
  int32 fileChunkSize; 
  int32 maxDownlinkSize; 
  int32 beaconPeriod; 
  int32 increasedBeaconPeriod; 
  uint16 expectedRebootDuration; 
  void serialize(void);
  void deserialize(void);
};

class COMConfig : public Serialize {
public:
  const static int size = 4;
  COMConfig();
  COMConfig(int32 testConfig);
  int32 testConfig; 
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

class FMGConfig : public Serialize {
public:
  const static int size = 4;
  FMGConfig();
  FMGConfig(int32 testConfig);
  int32 testConfig; 
  void serialize(void);
  void deserialize(void);
};

class GPSConfig : public Serialize {
public:
  const static int size = 28;
  GPSConfig();
  GPSConfig(float a, float e, float i, float Omega, float omega, float anom, float epochSeconds);
  float a; // Semi-major axis
  float e; // Eccentricity
  float i; // Inclination
  float Omega; // Right ascension of the ascending node
  float omega; // Argument of perigee
  float anom; // True anomoly
  float epochSeconds; // [rad]
  void serialize(void);
  void deserialize(void);
};

class PLDConfig : public Serialize {
public:
  const static int size = 4;
  PLDConfig();
  PLDConfig(int32 chunkSize);
  int32 chunkSize; 
  void serialize(void);
  void deserialize(void);
};

class SCHItem : public Serialize {
public:
  const static int size = 34; //changed from 42
  SCHItem();
  SCHItem(double ecefPos[2], double radius, uint8 enter_mode, uint32 timeout, uint8 mode, int32 duration);
  double ecefPos[2];
  double radius; 
  uint8 enter_mode; 
  uint32 timeout; 
  uint8 mode; 
  int32 duration; 
  void serialize(void);
  void deserialize(void);
};

}
}

#endif
