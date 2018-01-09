
#define MAX_FILE_SIZE		4800

#ifdef CDHBUILD
#define HOME_DIRECTORY		"/home/root"
#endif

#ifdef UBUNTU
#define HOME_DIRECTORY		"home"
#endif

#define UPLINK_DIRECTORY	HOME_DIRECTORY		"/.uplink"
#define DOWNLINK_DIRECTORY	HOME_DIRECTORY		"/.downlink"
#define HEALTH_DIRECTORY	HOME_DIRECTORY		"/SD_2/HST"
#define REBOOT_FILE			HOME_DIRECTORY		"/RBT.txt"

#define NEW_SCH				UPLINK_DIRECTORY	"SCH"
#define DEFAULT_SCH			UPLINK_DIRECTORY	"DSCH"
#define NEW_DEFAULT_SCH		UPLINK_DIRECTORY	"NDSCH"

#define ACS_PATH			"/ACS"
#define CDH_PATH			"/CDH"
#define COM_PATH			"/COM"
#define EPS_PATH			"/EPS"
#define GPS_PATH			"/GPS"
#define RAD_PATH			"/RAD"
