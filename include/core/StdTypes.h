/*
 * StdTypes.h
 *
 *  Created on: Feb 9, 2010
 *      Author: Riley
 */

#ifndef _STDTYPES_H_
#define _STDTYPES_H_

#include <cstddef>
#include <utility>

typedef unsigned char uint8;
typedef signed char int8;
typedef unsigned short uint16;
typedef signed short int16;
typedef unsigned long uint32;
typedef signed long int32;
typedef unsigned long long uint64;
typedef signed long long int64;

typedef uint16 LocationIDType; // see HardwareLocationIDEnum + ServerLocationIDEnum
#define LOCATION_ID_INVALID 0  // used with LocationIDType

enum PowerSubsystemEnum{
	POWER_SUBSYSTEM_PROP = 0,
	POWER_SUBSYSTEM_ACS,
	POWER_SUBSYSTEM_COM,
	POWER_SUBSYSTEM_PLD,
	POWER_SUBSYSTEM_GPS,
	POWER_SUBSYSTEM_STR,
	POWER_NUM_SUBSYSTEMS
};

typedef uint8 MessageCodeType;

typedef enum HardwareLocationIDEnum
{
	HARDWARE_LOCATION_MIN = (LOCATION_ID_INVALID + 1),
	HARDWARE_LOCATION_EPS = HARDWARE_LOCATION_MIN,
	HARDWARE_LOCATION_COM,
	HARDWARE_LOCATION_ACS,
	HARDWARE_LOCATION_PLD,
	HARDWARE_LOCATION_GPS,
	HARDWARE_LOCATION_MAX
}HardwareLocationIDType;

enum ServerLocationIDEnum
{
	SERVER_LOCATION_MIN = HARDWARE_LOCATION_MAX,
	SERVER_LOCATION_EPS = SERVER_LOCATION_MIN,
	SERVER_LOCATION_COM, //9
	SERVER_LOCATION_ACS,
	SERVER_LOCATION_PROP,
	SERVER_LOCATION_CDH,
	SERVER_LOCATION_PLD,
	SERVER_LOCATION_GPS,
	SERVER_LOCATION_SCH,
	SERVER_LOCATION_CMD,
	SERVER_LOCATION_ERR, //17
	SERVER_LOCATION_GND,
	SERVER_LOCATION_SPI,
	SERVER_LOCATION_ETH,
	SERVER_LOCATION_MAX
};

typedef enum LoggerLevel
{
	LOGGER_LEVEL_SUPER_DEBUG = 1,
	LOGGER_LEVEL_DEBUG, // fine-grain output for debugging
	LOGGER_LEVEL_INFO, // nominal system info
	LOGGER_LEVEL_WARN, // warnings (minor errors)
	LOGGER_LEVEL_ERROR, // errors
	LOGGER_LEVEL_FATAL, // fatal errors
}LoggerLevelType;

enum FileHandlerIDEnum
{
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

#define OPCODE_ERRORS_MIN		0
#define OPCODE_ERRORS_MAX		256

enum CMDCommandEnum
{
	CMD_CMD_MIN = 1,
	CMD_ACP_SWITCH,
	CMD_CMD_MAX,

	CMD_ACP_SWITCH_SUCCESS = 100,

	CMD_ACP_ERROR_MIN = 200,
	CMD_ACP_SWITCH_FAILURE = CMD_ACP_ERROR_MIN,
	CMD_ACP_SWITCH_BAD_SUBSYS,
	CMD_ACP_SWITCH_BAD_PROTOCOL,
	CMD_ACP_ERROR_MAX
};

enum EPSCommandEnum
{
	EPS_CMD_MIN = 0,
	EPS_HS_CMD = EPS_CMD_MIN,
	EPS_ALIVE_CMD,
	EPS_NO_RESPONSE_CMD,

	EPS_POWER_CYCLE_CMD = 8,

	EPS_MSG_ERROR_MIN = 100,
	EPS_HS_FAILURE = EPS_MSG_ERROR_MIN,
	EPS_POWER_CYCLE_FAILURE,

	EPS_HS_SUCCESS = 200,
	EPS_POWER_CYCLE_SUCCESS,
};

enum ACSCommandEnum{
	ACS_CMD_MIN = 0,
	ACS_HS_CMD = ACS_CMD_MIN,
	ACS_ALIVE_CMD,
	ACS_NO_RESPONSE_CMD,

	ACS_GPS_CMD = 19,
	ACS_CMD_MAX,

	ACS_ERROR_MIN = 101,
	ACS_HS_ERROR = ACS_ERROR_MIN,
	ACS_GPS_ERROR = 19,

	ACS_HS_SUCCESS = 201,
	ACS_GPS_SUCCESS = 219
};

enum COMCommandEnum{
	COM_CMD_MIN = 0,
	COM_HS_CMD = COM_CMD_MIN,
	COM_ALIVE_CMD,
	COM_NO_RESPONSE_CMD,
	COM_CMD_MAX,

	COM_HS_SUCCESS = 201,

	COM_CMD_ERROR_MIN = 101,
	COM_HS_FAILURE = COM_CMD_ERROR_MIN,
};

enum GNDCommandEnum
{
	//Commands from GND to CDH
	EKERMIT_SEND_CMD = 1,
	EKERMIT_RECEIVE_CMD,

	//Commands from GND to CDH
	EKERMIT_SEND_SUCCESS = 201,
	EKERMIT_RECEIVE_SUCCESS,
	EKERMIT_SEND_PARTIAL_SUCCESS,

	//Commands from GND to CDH
	EKERMIT_SEND_FAILURE = 101,
	EKERMIT_RECEIVE_FAILURE,
	EKERMIT_NULL_BUFFER_FAILURE,
	EKERMIT_SET_PARAMS_FAILURE,
	EKERMIT_MSG_CAST_FAILURE,
	EKERMIT_SEND_TOO_MANY_FILES_FAILURE,
	EKERMIT_SEND_WRONG_VAR_TYPE_FAILURE,
	EKERMIT_SEND_FILENAME_TOO_LONG_FAILURE,
	EKERMIT_SEND_SET_STATE_FAILURE,
	EKERMIT_RECEIVE_SET_STATE_FAILURE,
	EKERMIT_RECEIVE_NUM_PARAMS_FAILURE,
};

enum OPCODE {
	OP_HEALTH_STATUS = 0,
	OP_PAYLOAD_DATA,
	OP_CONFIG_CLOCK = 16,
	OP_CONFIG_MOTOR,
	OP_CONFIG_DATA,
	OP_START_SCIENCE = 32,
	OP_STOP_SCIENCE,
	OP_ERROR_OPCODE = 128,
};

enum PLDCommandEnum
{
	PLD_CMD_MIN = 0,
	PLD_HS_CMD = PLD_CMD_MIN,
	PLD_ALIVE_CMD,
	PLD_NO_RESPONSE_CMD,

	PLD_DATA_CMD = 8,

	PLD_CONFIG_CLOCK = 16,
	PLD_CONFIG_MOTOR,
	PLD_CONFIG_DATA,

	PLD_START_SCIENCE = 32,
	PLD_STOP_SCIENCE,

	PLD_OPCODE_ERROR = 128,

	PLD_HS_SUCCESS = 201,
	PLD_DATA_SUCCESS,

	PLD_HS_ERROR = 101,
	PLD_DATA_ERROR
};

enum SCHCommandEnum
{
	SCH_BUILD_SCHEDULE_CMD = 1,
	SCH_BUILD_PLD_SCHEDULE_CMD,
	SCH_RUN_SCHEDULE_CMD,

	SCH_DEFAULT_RANGE_CONFIG = 10,

	SCH_BUILD_SCHEDULE_SUCCESS = 201,
	SCH_BUILD_PLD_SCHEDULE_SUCCESS,
	SCH_RUN_SCHEDULE_SUCCESS,

	SCH_DEFAULT_RANGE_SUCCESS = 210,

	SCH_CMD_ERROR_MIN = 101,
	SCH_BUILD_SCHEDULE_FAILURE = SCH_CMD_ERROR_MIN,
	SCH_BUILD_PLD_SCHEDULE_FAILURE,
	SCH_RUN_SCHEDULE_FAILURE,
	SCH_CMD_ERROR_MAX,

	SCH_DEFAULT_RANGE_FAILURE = 110
};

enum CDHCommandEnum
{

	CDH_CMD_MIN=1,
	CDH_CPU_USAGE_CMD = CDH_CMD_MIN,
	CDH_MEM_USAGE_CMD,
	CDH_STORAGE_CMD,
	CDH_TEMP_START_CMD,
	CDH_TEMP_READ_CMD,
	CDH_HOT_SWAPS_CMD,
	CDH_POWER_MONITORS_CMD,
	CDH_START_PM_CMD,
	CDH_CLEAN_FS_CMD,
	CDH_CMD_MAX,

	CDH_CPU_USAGE_SUCCESS = 201,
	CDH_MEM_USAGE_SUCCESS,
	CDH_STORAGE_SUCCESS,
	CDH_TEMP_START_SUCCESS,
	CDH_TEMP_READ_SUCCESS,
	CDH_HOT_SWAPS_SUCCESS,
	CDH_POWER_MONITORS_SUCCESS,
	CDH_CLEAN_FS_SUCCESS,
	CDH_START_PM_SUCCESS,

	CDH_CMD_ERROR_MIN = 101,
	CDH_CPU_USAGE_FAILURE = CDH_CMD_ERROR_MIN,
	CDH_MEM_USAGE_FAILURE,
	CDH_STORAGE_FAILURE,
	CDH_TEMP_START_FAILURE,
	CDH_TEMP_READ_FAILURE,
	CDH_HOT_SWAPS_FAILURE,
	CDH_POWER_MONITORS_FAILURE,
	CDH_START_PM_FAILURE,
	CDH_CLEAN_FS_FAILURE,
	CDH_CMD_ERROR_MAX
};

/* Standard enumeration for general errors that can
`* occur on any subsystem server.
 */
enum ErrorCodeEnum
{
	DISPATCHER_ERR_START = 180,
	DISPATCHER_NO_INSTANCE = DISPATCHER_ERR_START,
	DISPATCHER_STATUS_ERR,
	DISPATCH_FAILED,
	DISPATCHER_ERR_END = DISPATCH_FAILED,
	FILE_HANDLER_ERR_START,
	FILE_HANDLER_NO_INSTANCE = FILE_HANDLER_ERR_START,
	FILE_HANDLER_FAILED,
	FILE_HANDLER_ERR_END = FILE_HANDLER_FAILED,
	MODEMAN_NO_INSTANCE,
	WATCHDOGMAN_NO_INSTANCE,
	ERROR_QUEUE_FAILED,
	PACKET_FORMAT_FAIL,
	ERROR_OPCODE_UNRECOGNIZED_TYPE
};

enum FileSizeEnum
{
	ACS_HS = 100,
	COM_HS = 100,
	EPS_HS = 100,
	GPS_HS = 100,
	PLD_HS = 100,
	THM_BUS1 = 100,
	THM_BUS2 = 100,
	THM_BUS3 = 100,
	THM_BUS4 = 100,
	ERROR_LOGS = 1000,
	EVENT_LOGS = 1000
};

#define PAYLOAD_SCHEDULE_QUEUE_MAXSIZE 100
#define SCHEDULE_QUEUE_MAXSIZE 100

/// \brief Context (Scheduler) Switch Type
/// TIME = Wait a set amount of time before going onto next task
/// LOCATION = Location. Wait unil all star hits a location before switching
enum ScheduleSwitchTypeEnum
{
	SCH_ST_START = 0,
	SCH_ST_TIME = SCH_ST_START,
	SCH_ST_LOCATION,
	SCH_ST_END = SCH_ST_LOCATION
};

enum SystemModeEnum
{
	MODE_FIRST_MODE = 0,
	MODE_ACCESS = MODE_FIRST_MODE,
	MODE_STARTUP,
	MODE_BUS_PRIORITY,
	MODE_PLD_PRIORITY,
	MODE_COM,
	MODE_DIAGNOSTIC,
	MODE_NUM_MODES
};

enum SchedulerTargetEnum
{
	MODE_TARGET = 0,
	EVENT_TARGET,
	GND_STATION,
	MAX_SCH_TARGETS
};

enum ACSSystemPointEnum
{
	COM_RF_ANTENNA = 0,
	THEIA_CAMERA,
	SYSTEM_INVALID
};

enum COMProtocol
{
	ACP_PROTOCOL_MIN = 1,
	ACP_PROTOCOL_SPI = ACP_PROTOCOL_MIN,
	ACP_PROTOCOL_I2C,
	ACP_PROTOCOL_ETH,
	ACP_PROTOCOL_MAX
};

typedef uint32 ConfigItemType;

typedef std::pair<LocationIDType, MessageCodeType> MessageIdentifierType;

#endif /* _STDTYPES_H_ */
