
#define MAX_FILE_SIZE		5000

#ifdef CDHBUILD
#define ROOT_DIRECTORY	 	"/"
#endif

#ifdef UBUNTU
#define ROOT_DIRECTORY		"./"
#endif

#define HOME_DIRECTORY		ROOT_DIRECTORY 		"home/root"
#define UPLINK_DIRECTORY	ROOT_DIRECTORY		"opt/uplink"
#define DOWNLINK_DIRECTORY	ROOT_DIRECTORY		"opt/downlink"
#define BEACON_DIRECTORY	ROOT_DIRECTORY		"opt/beacon"
#define HEALTH_DIRECTORY	ROOT_DIRECTORY		"SD_2/HST"
#define LOG_MESSAGES		ROOT_DIRECTORY  	"SD_3/LOG/LOG"
#define RAD_FILE_PATH		ROOT_DIRECTORY		"SD_3/RAD/RAD" // Science data

#define COM_PASS_COUNT		HOME_DIRECTORY		"/CPC.txt"
#define REBOOT_FILE			HOME_DIRECTORY		"/RBT.txt"

#define NEW_SCH				UPLINK_DIRECTORY	"/SCH"
#define SOT_PATH			UPLINK_DIRECTORY	"/SOT"
#define IEF_PATH			UPLINK_DIRECTORY	"/IEF.txt"
#define PPE_PATH			UPLINK_DIRECTORY	"/PPE.txt"

#define DFL_PATH			DOWNLINK_DIRECTORY	"/DFL.txt"

#define BEACON				BEACON_DIRECTORY	"/beacon"

#define ACS_PATH			"/ACS"
#define CDH_PATH			"/CDH"
#define COM_PATH			"/COM"
#define EPS_PATH			"/EPS"
#define GPS_PATH			"/GPS"
#define RAD_PATH			"/RAD"

