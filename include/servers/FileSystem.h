/*
 * 	FileSystem.h
 *
 * 	Author: Alex St. Clair
 */

#ifndef FILESYSTEM_H_
#define FILESYSTEM_H_

using namespace std;
namespace AllStar{
namespace Servers{

#define HOME_DIRECTORY			"/home/root"

#define DOWNLINK_DIRECTORY		HOME_DIRECTORY		"/.downlink"
#define UPLINK_DIRECTORY		HOME_DIRECTORY		"/.uplink"
#define IMMED_DIRECTORY			HOME_DIRECTORY		"/.immed"
#define CONFIG_DIRECTORY		HOME_DIRECTORY		"/.config"
#define VERBOSE_HST_DIRECTORY	HOME_DIRECTORY		"/.vhs"
#define BINARIES_DIRECTORY		HOME_DIRECTORY		"/binaries"

#define SOT_PATH				UPLINK_DIRECTORY	"/SOT.txt"
#define DRF_PATH				UPLINK_DIRECTORY	"/DRF.txt"
#define DLT_PATH				UPLINK_DIRECTORY	"/DLT.txt"
#define PPE_PATH				UPLINK_DIRECTORY	"/PPE.txt"
#define EOT_PATH				UPLINK_DIRECTORY	"/EOT.txt"
#define IEF_PATH				UPLINK_DIRECTORY	"/IEF.txt"
#define DFL_PATH				UPLINK_DIRECTORY	"/DFL.txt"

#define ACS_CFG_UP				UPLINK_DIRECTORY	"/ACSConfig"
#define CDH_CFG_UP				UPLINK_DIRECTORY	"/CDHConfig"
#define CMD_CFG_UP				UPLINK_DIRECTORY	"/CMDConfig"
#define COM_CFG_UP				UPLINK_DIRECTORY	"/COMConfig"
#define EPS_CFG_UP				UPLINK_DIRECTORY	"/EPSConfig"
#define FMG_CFG_UP				UPLINK_DIRECTORY	"/FMGConfig"
#define GPS_CFG_UP				UPLINK_DIRECTORY	"/GPSConfig"
#define PLD_CFG_UP				UPLINK_DIRECTORY	"/PLDConfig"
#define SCH_CFG_UP				CONFIG_DIRECTORY	"/SCHConfig" //changed for default schedule

#define ACS_CONFIG				CONFIG_DIRECTORY	"/ACSConfig"
#define CDH_CONFIG				CONFIG_DIRECTORY	"/CDHConfig"
#define CMD_CONFIG				CONFIG_DIRECTORY	"/CMDConfig"
#define COM_CONFIG				CONFIG_DIRECTORY	"/COMConfig"
#define EPS_CONFIG				CONFIG_DIRECTORY	"/EPSConfig"
#define FMG_CONFIG				CONFIG_DIRECTORY	"/FMGConfig"
#define GPS_CONFIG				CONFIG_DIRECTORY	"/GPSConfig"
#define PLD_CONFIG				CONFIG_DIRECTORY	"/PLDConfig"
#define SCH_CONFIG	 			CONFIG_DIRECTORY	"/SCHConfig"

#define DFL_DOWN				DOWNLINK_DIRECTORY	"/DFL.txt"

#define EPOCH_FILE				CONFIG_DIRECTORY	"/PolarCube_Epoch"	// reboot number
#define RAD_NUM_FILE			CONFIG_DIRECTORY	"/RAD_Number"		// science collection number
#define REBOOT_TIME_FILE		CONFIG_DIRECTORY	"/Reboot_Time"		// stores an estimate for the time after a reboot
#define GPS_LOCK_FILE			CONFIG_DIRECTORY	"/LastGPSLock"		// for storing the last lock in ECI
#define LAST_MRP_FILE			CONFIG_DIRECTORY	"/Last_MRP"			// for storing the last ACS MRP on a reboot

#define SCH_UP					UPLINK_DIRECTORY	"/SCH"
#define SCHEDULE_FILE 			"/home/root/SCH"

#define UFTP_PATH				BINARIES_DIRECTORY	"/uftp"
#define UFTPD_PATH				BINARIES_DIRECTORY	"/uftpd"
#define KISSATTACH_PATH			BINARIES_DIRECTORY	"/kissattach"
#define KISSPARMS_PATH			BINARIES_DIRECTORY	"/kissparms"

#define CMD_FILE_PATH			"/SD_2/CMD" // Commands (ACKs and NAKs)
#define ERR_FILE_PATH			"/SD_2/ERR" // Errors
#define GEN_FILE_PATH			"/SD_2/GEN" // General
#define GPS_FILE_PATH			"/SD_2/GPS" // GPS data
#define MOD_FILE_PATH			"/SD_2/MOD" // Mode switches
#define PWR_FILE_PATH			"/SD_2/PWR" // Subsystem power changes
#define SPI_FILE_PATH			"/SD_2/SPI" // SPI (successes only---failures are in ERR)
#define SWP_FILE_PATH			"/SD_2/SWP" // Hot swaps (power monitors?)
#define THM_FILE_PATH			"/SD_2/THM" // Thermal (ie. temp sensor data)
#define RAD_FILE_PATH			"/SD_3/RAD" // Science data

#define HST_FILE_PATH			"/SD_2/HST" // Health and status
#define ACS_HST_PATH			HST_FILE_PATH		"/ACS"
#define CDH_HST_PATH			HST_FILE_PATH		"/CDH"
#define COM_HST_PATH			HST_FILE_PATH		"/COM"
#define EPS_HST_PATH			HST_FILE_PATH		"/EPS"
#define PLD_HST_PATH			HST_FILE_PATH		"/PLD"

#define UPLK_PASSWORD			"P0!@rCube\n"
#define UPLK_PASSWORD_SKIP		"P0!@rCube_1\n"
#define UPLK_PASSWORD_SEND		"P0!@rCube_2\n"

} // End of namespace Servers
} // End of namespace AllStar

#endif /* FILESYSTEM_H_ */
