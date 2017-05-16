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

#define DOWNLINK_DIRECTORY		"/home/root/.downlink"
#define UPLINK_DIRECTORY		"/home/root/.uplink"
#define IMMED_DIRECTORY			"/home/root/.immed"
#define CONFIG_DIRECTORY		"/home/root/.config"
#define VERBOSE_HST_DIRECTORY	"/home/root/.vhs"
#define BINARIES_DIRECTORY		"/home/root/binaries"

#define SOT_PATH				UPLINK_DIRECTORY	"/SOT.txt"
#define DRF_PATH				UPLINK_DIRECTORY	"/DRF.txt"
#define DLT_PATH				UPLINK_DIRECTORY	"/DLT.txt"
#define PPE_PATH				UPLINK_DIRECTORY	"/PPE.txt"
#define EOT_PATH				UPLINK_DIRECTORY	"/EOT.txt"
#define IEF_PATH				UPLINK_DIRECTORY	"/IEF.txt"

#define ACS_CFG_UP				UPLINK_DIRECTORY	"/ACSConfig"
#define CDH_CFG_UP				UPLINK_DIRECTORY	"/CDHConfig"
#define CMD_CFG_UP				UPLINK_DIRECTORY	"/CMDConfig"
#define COM_CFG_UP				UPLINK_DIRECTORY	"/COMConfig"
#define EPS_CFG_UP				UPLINK_DIRECTORY	"/EPSConfig"
#define FMG_CFG_UP				UPLINK_DIRECTORY	"/FMGConfig"
#define GPS_CFG_UP				UPLINK_DIRECTORY	"/GPSConfig"
#define PLD_CFG_UP				UPLINK_DIRECTORY	"/PLDConfig"
#define SCH_CFG_UP				UPLINK_DIRECTORY	"/SCHConfig"

#define ACS_CONFIG				CONFIG_DIRECTORY	"/ACSConfig"
#define CDH_CONFIG				CONFIG_DIRECTORY	"/CDHConfig"
#define CMD_CONFIG				CONFIG_DIRECTORY	"/CMDConfig"
#define COM_CONFIG				CONFIG_DIRECTORY	"/COMConfig"
#define EPS_CONFIG				CONFIG_DIRECTORY	"/EPSConfig"
#define FMG_CONFIG				CONFIG_DIRECTORY	"/FMGConfig"
#define GPS_CONFIG				CONFIG_DIRECTORY	"/GPSConfig"
#define PLD_CONFIG				CONFIG_DIRECTORY	"/PLDConfig"
#define SCH_CONFIG	 			CONFIG_DIRECTORY	"/SCHConfig"

#define SCH_UP					UPLINK_DIRECTORY	"/SCH"
#define SCHEDULE_FILE 			"/home/root/SCH"

#define UFTP_PATH				BINARIES_DIRECTORY	"/uftp"
#define UFTPD_PATH				BINARIES_DIRECTORY	"/uftpd"
#define KISSATTACH_PATH			BINARIES_DIRECTORY	"/kissattach"
#define KISSPARMS_PATH			BINARIES_DIRECTORY	"/kissparms"

#define ACP_FILE_PATH			"/SD_2/ACP"
#define CMD_FILE_PATH			"/SD_2/CMD"
#define DGN_FILE_PATH			"/SD_2/DGN"
#define ERR_FILE_PATH			"/SD_2/ERR"
#define GEN_FILE_PATH			"/SD_2/GEN"
#define FSS_FILE_PATH			"/SD_2/FSS"
#define HST_FILE_PATH			"/SD_2/HST"
#define MOD_FILE_PATH			"/SD_2/MOD"
#define SSS_FILE_PATH			"/SD_2/SSS"
#define SWP_FILE_PATH			"/SD_2/SWP"
#define RAD_FILE_PATH			"/SD_3/RAD"

#define UPLK_PASSWORD			"P0!@rCube\n"
#define UPLK_PASSWORD_SKIP		"P0!@rCube_1\n"
#define UPLK_PASSWORD_SEND		"P0!@rCube_2\n"

} // End of namespace Servers
} // End of namespace AllStar

#endif /* FILESYSTEM_H_ */
