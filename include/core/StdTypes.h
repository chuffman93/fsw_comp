/*
 * StdTypes.h
 *
 *  Created on: Feb 9, 2010
 *      Author: Riley
 *
 *  Last Modified: Jack Dinkel 12/15/16
 */

#ifndef _STDTYPES_H_
#define _STDTYPES_H_

#include <cstddef>
#include <utility>
#include <stdint.h>

typedef uint8_t uint8;
typedef int8_t int8;
typedef uint16_t uint16;
typedef int16_t int16;
typedef uint32_t uint32;
typedef int32_t int32;
typedef uint64_t uint64;
typedef int64_t int64;


typedef uint16 LocationIDType; // see HardwareLocationIDEnum + ServerLocationIDEnum
#define LOCATION_ID_INVALID 0  // used with LocationIDType

typedef enum HardwareLocationIDEnum {
	HARDWARE_LOCATION_MIN = (LOCATION_ID_INVALID + 1),
	HARDWARE_LOCATION_EPS = HARDWARE_LOCATION_MIN,
	HARDWARE_LOCATION_COM,
	HARDWARE_LOCATION_ACS,
	HARDWARE_LOCATION_PLD,
	HARDWARE_LOCATION_GPS,
	HARDWARE_LOCATION_MAX
} HardwareLocationIDType;

typedef enum WDMStateEnum {
	WDM_UNKNOWN,
	WDM_ASLEEP,
	WDM_ALIVE
} WDMStateType;

enum ServerLocationIDEnum {
	SERVER_LOCATION_MIN = HARDWARE_LOCATION_MAX,
	SERVER_LOCATION_EPS = SERVER_LOCATION_MIN,
	SERVER_LOCATION_COM, //9
	SERVER_LOCATION_ACS,
	SERVER_LOCATION_CDH,
	SERVER_LOCATION_PLD,
	SERVER_LOCATION_GPS,
	SERVER_LOCATION_SCH,
	SERVER_LOCATION_CMD,
	SERVER_LOCATION_FMG,
	SERVER_LOCATION_GND,
	SERVER_LOCATION_SPI,
	SERVER_LOCATION_ERR,
	SERVER_LOCATION_MAX
};

typedef enum LoggerLevel {
	LOGGER_LEVEL_SUPER_DEBUG = 1,
	LOGGER_LEVEL_DEBUG, // fine-grain output for debugging
	LOGGER_LEVEL_INFO, // nominal system info
	LOGGER_LEVEL_WARN, // warnings (minor errors)
	LOGGER_LEVEL_ERROR, // errors
	LOGGER_LEVEL_FATAL, // fatal errors
} LoggerLevelType;

enum FileHandlerIDEnum {
	SUBSYSTEM_COM = 1,
	SUBSYSTEM_EPS,
	SUBSYSTEM_ACS,
	SUBSYSTEM_PLD,
	SUBSYSTEM_GPS,
	SUBSYSTEM_THM,
	SUBSYSTEM_SCH,
	SUBSYSTEM_STR,
	SUBSYSTEM_CMD,
	SYSTEM_CDH,

	LOG_ERROR,
	LOG_MODE,
	SYSTEM_MAX
};

enum FILServerDestinationEnum {
	DESTINATION_CMD = 1,
	DESTINATION_ERR,
	DESTINATION_GEN,
	DESTINATION_GPS,
	DESTINATION_MOD,
	DESTINATION_PWR,
	DESTINATION_SPI,
	DESTINATION_SWP,
	DESTINATION_THM,
	DESTINATION_ACS_HST,
	DESTINATION_CDH_HST,
	DESTINATION_COM_HST,
	DESTINATION_EPS_HST,
	DESTINATION_PLD_HST
};

typedef enum FSWCommandTypes {
	FSW_CMD_REQUEST_RESET = 1,
	FSW_CMD_HARD_SATELLITE_RESET,
	FSW_CMD_TX_SILENCE_START,
	FSW_CMD_TX_SILENCE_END,
	FSW_CMD_CLEAR_DWNLK,
	FSW_CMD_CLEAR_IMMED,
	FSW_CMD_CLEAR_UPLK,
	FSW_CMD_MAX
} FSWCommand;

typedef enum IEFCommandTypes {
	IEF_CMD_DOWNLINK = 1,
	IEF_CMD_MAX = IEF_CMD_DOWNLINK
} IEFCommand;

// ------------------------------------------------------------ Subsystem OpCodes --------------------------------------------------------

enum SubsystemOpcodes {
	// Test Opcodes (0-7)
	LED_ON_CMD = 0,
	LED_RATE_CONFIG,
	LED_RATE_DATA,

	// Diagnostic Opcodes (8-15)
	TEST_ALIVE_CMD = 8,

	// Standard Opcodes (16-31)
	HEALTH_STATUS_CMD = 16,
	SUBSYSTEM_RESET_CMD,

	// Subsystem Commands and configs (see subsystem enums)
	SUBSYSTEM_CMD_MIN = 32,
	SUBSYSTEM_CFG_MIN = 64,
	SUBSYSTEM_DAT_MIN = 96,
	SUBSYSTEM_OPCODE_MAX = 255
};

enum ACSCommandEnum {
	ACS_TEST_DRIVER = 9,

	ACS_OPCODE_MIN = SUBSYSTEM_CMD_MIN,
	ACS_POINT_NADIR = ACS_OPCODE_MIN,
	ACS_POINT_DEST,
	ACS_POINT_SUN,
	ACS_POINT_GND,
	ACS_DETUMBLE,
	ACS_GPS_CMD,
	ACS_CONFIG,
	ACS_OPCODE_MAX
};

enum COMCommandEnum {
	COM_OPCODE_MIN = SUBSYSTEM_CMD_MIN,
	COM_SIMPLEX = COM_OPCODE_MIN,
	COM_HALF_DUPLEX,
	COM_FULL_DUPLEX,
	COM_BEACON,
	COM_OPCODE_MAX
};

enum EPSCommandEnum {
	EPS_OPCODE_MIN = SUBSYSTEM_CMD_MIN,
	EPS_BATTERY_CONFIG = EPS_OPCODE_MIN,
	EPS_CMD_MAX
};

// TODO: update payload opcodes to new scheme
enum PLDCommandEnum {
	PLD_CMD_MIN = SUBSYSTEM_CMD_MIN,
	PLD_START_SCIENCE = PLD_CMD_MIN,
	PLD_DATA_CMD,
	PLD_CMD_MAX,

	PLD_CONFIG_CLOCK = SUBSYSTEM_CFG_MIN,
	PLD_CONFIG_MOTOR,
	PLD_CONFIG_DATA,
	PLD_CONFIG_MAX,

	PLD_BACKUP_SEND_SCIENCE = SUBSYSTEM_DAT_MIN,
	PLD_DATA_MAX
};

enum ErrorCodeEnum {
	DISPATCHER_ERR_START = 180,
	DISPATCHER_NO_INSTANCE = DISPATCHER_ERR_START,
	DISPATCHER_STATUS_ERR,
	DISPATCH_FAILED,
	DISPATCHER_ERR_END = DISPATCH_FAILED,
	PACKET_FORMAT_FAIL,
	ERROR_OPCODE_UNRECOGNIZED_TYPE
};

enum SystemModeEnum {
	MODE_FIRST_MODE = 0,
	MODE_STARTUP,
	MODE_BUS_PRIORITY,
	MODE_PLD_PRIORITY,
	MODE_COM,
	MODE_RESET,
	MODE_NUM_MODES
};

typedef enum ErrorOpcodeEnum {
	ERR_MIN = 0,

	// ACS Errors
	ERR_ACS_MIN = ERR_MIN,
	ERR_ACS_NOTALIVE = ERR_ACS_MIN,
	ERR_ACS_SELFCHECK,
	ERR_ACS_MAX,

	// CDH Errors
	ERR_CDH_MIN = ERR_ACS_MAX,
	ERR_CDH_MAX,

	// CMD Errors
	ERR_CMD_MIN = ERR_CDH_MAX,
	ERR_CMD_MAX,

	// COM Errors
	ERR_COM_MIN = ERR_CMD_MAX,
	ERR_COM_NOTALIVE = ERR_COM_MIN,
	ERR_COM_SELFCHECK,
	ERR_COM_MAX,

	// EPS Errors
	ERR_EPS_MIN = ERR_COM_MAX,
	ERR_EPS_NOTALIVE = ERR_EPS_MIN,
	ERR_EPS_SELFCHECK,
	ERR_EPS_MAX,

	// FMG Errors
	ERR_FMG_MIN = ERR_EPS_MAX,
	ERR_FMG_MAX,

	// GPS Errors
	ERR_GPS_MIN = ERR_FMG_MAX,
	ERR_GPS_MAX,

	// PLD Errors
	ERR_PLD_MIN = ERR_GPS_MAX,
	ERR_PLD_NOTALIVE = ERR_PLD_MIN,
	ERR_PLD_SELFCHECK,
	ERR_PLD_MAX,

	// SCH Errors
	ERR_SCH_MIN = ERR_PLD_MAX,
	ERR_SCH_MAX,

	// General errors (from the core)
	ERR_GEN_MIN = ERR_SCH_MAX,
	ERR_GEN_MAX,

	ERR_MAX
} ErrorOpcodeType;

#endif /* _STDTYPES_H_ */
