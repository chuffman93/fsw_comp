/*
 * StdTypes.h
 *
 *  Created on: Feb 9, 2010
 *      Author: Riley
 */

#ifndef _STDTYPES_H_
#define _STDTYPES_H_

typedef unsigned char uint8;
typedef signed char int8;
typedef unsigned short uint16;
typedef signed short int16;
typedef unsigned long uint32;
typedef signed long int32;
typedef unsigned long long uint64;
typedef signed long long int64;

// NOTE - A LocationIDType should always be < PHOENIX_NUM_SERVERS
// 		  i.e. 0 will be considered a location.
typedef uint16 LocationIDType;

#ifndef WIN32
#define MRAM_SECTION __attribute__((__section__(".mram")))
#else
#define MRAM_SECTION
#endif // WIN32

/*! \brief Minimum Message Size */
#define MESSAGE_OVERHEAD_SIZE   (5)

/*! \brief Minimum FSWPacket Size */
#define PACKET_OVERHEAD_SIZE    (12 + MESSAGE_OVERHEAD_SIZE)

/*! \brief Maximum Frame length */
#define	MAX_FRAME_LENGTH		255

/*! \brief Maximum Number of Frames */
#define	MAX_NUMBER_FRAMES		4

#ifdef __cplusplus
#ifdef _DEBUG
	#define _CRT_MAP_ALLOC
	#include <cstdlib>
	#include <new>
	#include <crtdbg.h>
//	#include <dbgnew.h>
	#define DEBUG_CLIENTBLOCK   new( _CLIENT_BLOCK, __FILE__, __LINE__)
	#define new DEBUG_CLIENTBLOCK
#else
	#define DEBUG_CLIENTBLOCK
	#include <cstdlib>
#endif // _DEBUG

#include <cstddef>
#include <utility>

enum PowerSubsystemEnum
{
    POWER_SUBSYSTEM_PROP = 0,
    POWER_SUBSYSTEM_ACS,
    POWER_SUBSYSTEM_COM,
    POWER_SUBSYSTEM_PLD,
    POWER_SUBSYSTEM_GPS,
    POWER_SUBSYSTEM_STR,
    POWER_NUM_SUBSYSTEMS
};

enum MessageTypeEnum
{
	MESSAGE_TYPE_COMMAND = 0,
	MESSAGE_TYPE_DATA = 1,
	MESSAGE_TYPE_ERROR = 2,
	MESSAGE_TYPE_CONFIG = 3,
	MESSAGE_TYPE_MAX
};

typedef uint8 MessageCodeType;

#define LOCATION_ID_INVALID		(0)

typedef enum HardwareLocationIDEnum
{
    HARDWARE_LOCATION_MIN = (LOCATION_ID_INVALID + 1),
	// Space COM
    HARDWARE_LOCATION_COM = HARDWARE_LOCATION_MIN,
    HARDWARE_LOCATION_EPS,
    HARDWARE_LOCATION_ACS,
    HARDWARE_LOCATION_PROP,
    HARDWARE_LOCATION_THM,
    HARDWARE_LOCATION_PLD,
    HARDWARE_LOCATION_GPS,
#ifdef WIN32
	// Needed an extra HW location to avoid confusing the dispatcher.
	// Ground COM
	HARDWARE_LOCATION_GND_COM,
#endif
    HARDWARE_LOCATION_MAX
}HardwareLocationIDType;

typedef enum LoggerLevel
{
	LOGGER_LEVEL_MIN = 1,
	LOGGER_LEVEL_DEBUG = LOGGER_LEVEL_MIN, // fine-grain output for debugging
	LOGGER_LEVEL_INFO, // nominal system info
	LOGGER_LEVEL_WARN, // warnings
	LOGGER_LEVEL_ERROR, // errors
	LOGGER_LEVEL_FATAL, // fatal errors
	LOGGER_LEVEL_MAX
}LoggerLevelType;

enum ServerLocationIDEnum
{
    SERVER_LOCATION_MIN = HARDWARE_LOCATION_MAX,
    SERVER_LOCATION_COM = SERVER_LOCATION_MIN,
    SERVER_LOCATION_EPS, //9
    SERVER_LOCATION_ACS,
    SERVER_LOCATION_PROP,
    SERVER_LOCATION_PLD,
    SERVER_LOCATION_GPS,
    SERVER_LOCATION_THM, //14
    SERVER_LOCATION_SCH,
    SERVER_LOCATION_CMD,
    SERVER_LOCATION_ERR, //17
    SERVER_LOCATION_GND,
    SERVER_LOCATION_CDH,
#ifdef WIN32
	SERVER_LOCATION_GND_COM,
#endif
	SERVER_LOCATION_MAX
};

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
	EPS_CMD_MIN = 1,
    EPS_HS_CMD = EPS_CMD_MIN,
    EPS_SOC_CMD,
    EPS_POWER_CYCLE_CMD,
    EPS_DISABLE_OC_PROT_CMD,
    EPS_ENABLE_OC_PROT_CMD,
	EPS_POWER_SUB_CMD,
	EPS_CMD_MAX,

	EPS_HS_SUCCESS = 201,
	EPS_SOC_SUCCESS,
	EPS_POWER_CYCLE_SUCCESS,
	EPS_DISABLE_OC_PROT_SUCCESS,
	EPS_ENABLE_OC_PROT_SUCCESS,
	EPS_POWER_SUB_SUCCESS,

	EPS_MSG_ERROR_MIN = 101,
	EPS_HS_FAILURE = EPS_MSG_ERROR_MIN,
	EPS_SOC_FAILURE,
	EPS_POWER_CYCLE_FAILURE,
	EPS_DISABLE_OC_PROT_FAILURE,
	EPS_ENABLE_OC_PROT_FAILURE,
	EPS_POWER_SUB_FAILURE,
	EPS_MSG_ERROR_MAX,

	EPS_SUB_ERROR_MIN = 156,
	EPS_OVERCURRENT_ERR = EPS_SUB_ERROR_MIN,
	EPS_BATMAN_NO_ARRAY,
	EPS_OVERVOLTAGE_ERR,
	EPS_LOW_VOLTAGE_ERR,
	EPS_HIGH_TEMP_ERR,
	EPS_LOW_TEMP_ERR,
	EPS_SUB_ERROR_MAX
};

enum ACSCommandEnum
{
	// Commands from CDH to ACS
	ACS_CMD_MIN = 1,
    ACS_HS_CMD = ACS_CMD_MIN,
	ACS_STATE_CMD,
    ACS_RAW_ADC_CMD,
    ACS_ATTITUDE_ERROR_REQUEST_CMD,
    ACS_POINT_MRP_VECTOR_CMD,
    ACS_POINT_GND_VECTOR_CMD,
    ACS_REQUEST_MRP_VECTOR_CMD,
    ACS_REQUEST_GND_VECTOR_CMD,
    ACS_SUNSOAK_CMD,
    ACS_POINT_NADIR_CMD,
    ACS_SET_TARGET_CMD,
    ACS_RANGE_TO_TARGET_CMD,
	ACS_HOLD_CMD,
	ACS_DISABLE_CMD,
    ACS_RESET_CMD,
    ACS_GPS_CMD,
	ACS_STARCAMERA_HS_CMD,
	ACS_POINT_COM_GND_CMD,
	ACS_HELLO_WORLD_TEST_CMD,
	ACS_FUNCTIONAL_TEST_CMD,
	ACS_FUNCTIONAL_TEST_COMPLETE_CMD,
	ACS_CMD_MAX,
	
	//Configurations from CDH to ACS
	ACS_CONFIG_MIN = 30,
	ACS_CTRL_ORIENT_CONFIG = ACS_CONFIG_MIN,
	ACS_MOI_WHEELS_CONFIG,
	ACS_MOI_SAT_CONFIG,
	ACS_COEFF_TORQUER_CONFIG,
	ACS_ORBIT_EPHEM_CONFIG,
	ACS_CTRL_LAW_GAINS_CONFIG,
	ACS_CTRL_LAW_FREQ_CONFIG,
	ACS_STARCAMERA_SETTINGS_CONFIG,
	ACS_UPDATE_ROTATION_MODE_CONFIG,
	ACS_NOTCH_FILTER_CONFIG,
	ACS_GPS_LEAP_SEC_CONFIG,
	ACS_GYRO_ZRV_CONFIG,
	ACS_DEGAUSS_CONFIG,
	ACS_UPDATE_GND_STATION_COORDS_CONFIG,
	ACS_GYRO_BIAS_CONFIG,
	ACS_CONFIG_MAX,
	
    // Returns from ACS to CDH
	ACS_HS_SUCCESS = 201,
	ACS_STATE_SUCCESS,
	ACS_RAW_ADC_SUCCESS,
	ACS_ATTITUDE_ERROR_REQUEST_SUCCESS,
	ACS_POINT_MRP_VECTOR_SUCCESS,
	ACS_POINT_GND_VECTOR_SUCCESS,
	ACS_REQUEST_MRP_VECTOR_SUCCESS,
	ACS_REQUEST_GND_VECTOR_SUCCESS,
	ACS_SUNSOAK_SUCCESS,
	ACS_POINT_NADIR_SUCCESS,
	ACS_SET_TARGET_SUCCESS,
	ACS_RANGE_TO_TARGET_SUCCESS,
	ACS_HOLD_SUCCESS,
	ACS_DISABLE_SUCCESS,
	ACS_RESET_SUCCESS,
	ACS_GPS_SUCCESS,
	ACS_STARCAMERA_LAST_LOCK_SUCCESS,
	ACS_POINT_COM_GND_SUCCESS,
	ACS_FUNCTIONAL_TEST_SUCCESS,
	ACS_FUNCTIONAL_TEST_COMPLETE_SUCCESS,
	
	//Configurations returns from ACS to CDH
	ACS_CTRL_ORIENT_SUCCESS = 230,
	ACS_MOI_WHEELS_SUCCESS,
	ACS_MOI_SAT_SUCCESS,
	ACS_COEFF_TORQUER_SUCCESS,
	ACS_ORBIT_EPHEM_SUCCESS,
	ACS_CTRL_LAW_GAINS_SUCCESS,
	ACS_CTRL_LAW_FREQ_SUCCESS,
	ACS_STARCAMERA_SETTINGS_SUCCESS,
	ACS_UPDATE_ROTATION_MODE_SUCCESS,
	ACS_NOTCH_FILTER_SUCCESS,
	ACS_GPS_LEAP_SEC_SUCCESS,
	ACS_GYRO_ZRV_SUCCESS,
	ACS_DEGAUSS_SUCCESS,
	ACS_UPDATE_GND_STATION_COORDS_SUCCESS,
	ACS_GYRO_BIAS_SUCCESS,

	// Errors returns from ACS to CDH
	ACS_CMD_ERROR_MIN = 101,
	ACS_HS_FAILURE = ACS_CMD_ERROR_MIN,
	ACS_STATE_FAILURE,
	ACS_RAW_ADC_FAILURE,
	ACS_ATTITUDE_ERROR_REQUEST_FAILURE,
	ACS_POINT_MRP_VECTOR_FAILURE, 
	ACS_POINT_GND_VECTOR_FAILURE,
	ACS_REQUEST_MRP_VECTOR_FAILURE,
	ACS_REQUEST_GND_VECTOR_FAILURE,
	ACS_SUNSOAK_FAILURE,
	ACS_POINT_NADIR_FAILURE,
	ACS_SET_TARGET_FAILURE,
	ACS_RANGE_TO_TARGET_FAILURE,
	ACS_HOLD_FAILURE,
	ACS_DISABLE_FAILURE,
	ACS_RESET_FAILURE,
	ACS_GPS_FAILURE,
	ACS_STARCAMERA_LAST_LOCK_FAILURE,
	ACS_POINT_COM_GND_FAILURE,
	ACS_FUNCTIONAL_TEST_FAILURE,
	ACS_FUNCTIONAL_TEST_COMPLETE_FAILURE,
	ACS_CMD_ERROR_MAX,
	
	//Configurations returns from ACS to CDH
	ACS_CTRL_ORIENT_FAILURE = 130,
	ACS_MOI_WHEELS_FAILURE,
	ACS_MOI_SAT_FAILURE,
	ACS_COEFF_TORQUER_FAILURE,
	ACS_ORBIT_EPHEM_FAILURE,
	ACS_CTRL_LAW_GAINS_FAILURE,
	ACS_CTRL_LAW_FREQ_FAILURE,
	ACS_STARCAMERA_SETTINGS_FAILURE,
	ACS_UPDATE_ROTATION_MODE_FAILURE,
	ACS_NOTCH_FILTER_FAILURE,
	ACS_GPS_LEAP_SEC_FAILURE,
	ACS_GYRO_ZRV_FAILURE,
	ACS_DEGAUSS_FAILURE,
	ACS_UPDATE_GND_STATION_COORDS_FAILURE,
	ACS_GYRO_BIAS_FAILURE,

	//Errors generated on ACS sent to CDH
	ACS_SUB_ERROR_MIN = 156,
    ACS_GYRO_STUCK_ERR = ACS_SUB_ERROR_MIN,
	ACS_NO_MOTOR_RESP_ERR,
	ACS_CAMERA_SATURATED_ERR,
	ACS_MAG_STUCK_ERR,
	ACS_ATTITUDE_NOT_CONVERGE_ERR,
	ACS_SUB_ERROR_MAX
};

enum GPSCommandEnum
{
	GPS_CMD_MIN = 1,
	GPS_HS_CMD = GPS_CMD_MIN,
	GPS_TIME_CMD,
	GPS_POSITION_CMD,
	GPS_RESET_CMD,
	GPS_CMD_MAX,

	GPS_HS_SUCCESS = 201,
	GPS_TIME_SUCCESS,
	GPS_POSITION_SUCCESS,
	GPS_RESET_SUCCESS,

	GPS_CMD_ERROR_MIN = 101,
	GPS_HS_FAILURE = GPS_CMD_ERROR_MIN,
	GPS_TIME_FAILURE,
	GPS_POSITION_FAILURE,
	GPS_RESET_FAILURE,
	GPS_CMD_ERROR_MAX,

	GPS_SUB_ERROR_MIN = 156,
	GPS_INSUFFICIENT_SATELLITES = GPS_SUB_ERROR_MIN,
	GPS_INACURATE_DATA,
	GPS_SUB_ERROR_MAX
};

enum COMCommandEnum
{
    //Commands from CDH to COM
    COM_CMD_MIN = 1,
    COM_HS_CMD = COM_CMD_MIN,
    COM_BEACON_CMD,
    COM_DATA_SEND_CMD,
    COM_LOGOUT_CMD,
    COM_RESET_CMD,
    COM_SET_TX_POWER_CMD,

    //Commands from COM to CDH
    COM_DATA_RECEIVE_CMD,
    COM_LOGIN_CMD,
    COM_GND_LOGGED_OUT_CMD,
    COM_CONNECTION_LOST_CMD,
    COM_PASSWORD_REFUSED_CMD,
    COM_CMD_MAX,

    //Commands from CDH to COM
    COM_HS_SUCCESS = 201,
    COM_BEACON_SUCCESS,
    COM_DATA_SEND_SUCCESS,
    COM_LOGOUT_SUCCESS,
    COM_RESET_SUCCESS,

    //Commands from COM to CDH
    COM_DATA_RECEIVE_SUCCESS,
    COM_LOGIN_SUCCESS,
    COM_GND_LOGGED_OUT_SUCCESS,
    COM_CONNECTION_LOST_SUCCESS,
    COM_PASSWORD_REFUSED_SUCCESS,
    COM_CMD_SUCCESS_MAX,

    //Commands from CDH to COM
    COM_CMD_ERROR_MIN = 101,
    COM_HS_FAILURE = COM_CMD_ERROR_MIN,
    COM_BEACON_FAILURE,
    COM_DATA_SEND_FAILURE,
    COM_LOGOUT_FAILURE,
    COM_RESET_FAILURE,

    //Commands from COM to CDH
    COM_DATA_RECEIVE_FAILURE,
    COM_LOGIN_FAILURE,
    COM_GND_LOGGED_OUT_FAILURE,
    COM_CONNECTION_LOST_FAILURE,
    COM_PASSWORD_REFUSED_FAILURE,
    COM_CMD_ERROR_MAX,
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

enum PLDCommandEnum
{
	PLD_CMD_MIN = 1,
	PLD_HS_CMD = PLD_CMD_MIN,
	PLD_PIC_CMD,
	PLD_PIC_GET_CMD,
	PLD_DATA_CMD,
	PLD_RESET_CMD,
	PLD_CMD_MAX,
	
	PLD_CONFIG_MIN = 10,
	PLD_RES_CONFIG = PLD_CONFIG_MIN,
	PLD_CHUNK_CONFIG,
	PLD_GAIN_CONFIG,
	PLD_EXPTIME_CONFIG,
	PLD_CONFIG_MAX,

	PLD_HS_SUCCESS = 201,
	PLD_PIC_SUCCESS,
	PLD_PIC_GET_SUCCESS,
	PLD_DATA_SUCCESS,
	PLD_RESET_SUCCESS,
	
	PLD_RES_SUCCESS = 210,
	PLD_CHUNK_SUCCESS,
	PLD_GAIN_SUCCESS,
	PLD_EXPTIME_SUCCESS,

	PLD_CMD_ERROR_MIN = 101,
	PLD_HS_FAILURE = PLD_CMD_ERROR_MIN,
	PLD_PIC_FAILURE,
	PLD_PIC_GET_FAILURE,
	PLD_DATA_FAILURE,
	PLD_RESET_FAILURE,
	PLD_CMD_ERROR_MAX,
	
	PLD_CONFIG_ERROR_MIN = 110,
	PLD_RES_FAILURE = PLD_CONFIG_ERROR_MIN,
	PLD_CHUNK_FAILURE,
	PLD_GAIN_FAILURE,
	PLD_EXPTIME_FAILURE,
	PLD_CONFIG_ERROR_MAX,

	PLD_SUB_ERROR_MIN,
	PLD_CMOS_READ_FAILURE = PLD_SUB_ERROR_MIN,
	PLD_MEM_ERROR,
	PLD_SUB_ERROR_MAX
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

enum THMCommandEnum
{
	THM_HS_CMD = 1,

	THM_HS_SUCCESS = 201,

	THM_HS_FAILURE = 101,
};

enum CDHCommandEnum
{

	CDH_CMD_MIN=1,
	CDH_CPU_USAGE_CMD = CDH_CMD_MIN,
	CDH_MEM_USAGE_CMD,
	CDH_STORAGE_CMD,
	CDH_TEMP_BUS_CMD,
	CDH_HOT_SWAPS_CMD,
	CDH_CMD_MAX,

	CDH_CPU_USAGE_SUCCESS = 201,
	CDH_MEM_USAGE_SUCCESS,
	CDH_STORAGE_SUCCESS,
	CDH_TEMP_BUS_SUCCESS,
	CDH_HOT_SWAPS_SUCCESS,

	CDH_CMD_ERROR_MIN = 101,
	CDH_CPU_USAGE_FAILURE,
	CDH_MEM_USAGE_FAILURE,
	CDH_STORAGE_FAILURE,
	CDH_TEMP_BUS_FAILURE,
	CDH_HOT_SWAPS_FAILURE,
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
	PACKET_FORMAT_FAIL
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

/// \brief Context (Payload Scheduler) Switch Type
/// TIME = Wait a set amount of time before going onto next task
/// LOCATION = Location. Wait unil all star hits a location before switching
enum PayloadScheduleEventTypeEnum
{
	PLD_SCH_ST_START = 0,
	PLD_SCH_ST_TIME = PLD_SCH_ST_START,
	PLD_SCH_ST_LOCATION,
	PLD_SCH_ST_END = PLD_SCH_ST_LOCATION
};

enum SystemModeEnum
{
	MODE_FIRST_MODE = 0,
	MODE_ACCESS = MODE_FIRST_MODE,
	MODE_STARTUP,
	MODE_BUS_PRIORITY,
	MODE_PLD_PRIORITY,
	MODE_ERROR,
	MODE_COM,
#ifdef TEST
	MODE_TEST,
	MODE_TEST2,
#endif // DEBUG
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

typedef std::pair<MessageTypeEnum, MessageCodeType> MessageIdentifierType;

#define USE_WATCHDOG    0

#if USE_WATCHDOG
#include "WatchdogManager.h"
#define CREATE_TASK(taskFunction,name,stackDepth,parameters,priority,taskHandle) \
        WatchdogManager::CreateTask(taskFunction,name,stackDepth,parameters,priority,taskHandle)

#define DELETE_TASK(taskHandle) WatchdogManager::DeleteTask(taskHandle)
#else
#define CREATE_TASK(taskFunction,name,stackDepth,parameters,priority,taskHandle) \
        xTaskCreate(taskFunction,name,stackDepth,parameters,priority,taskHandle)

#define DELETE_TASK(taskHandle) vTaskDelete(taskHandle)
#endif // USE_WATCHDOG

#endif /* __cplusplus */
#endif /* _STDTYPES_H_ */
