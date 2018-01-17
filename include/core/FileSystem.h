
#define MAX_FILE_SIZE		5000
//max number of files in tarball
#define MAX_NUM_FILES		100

#ifdef CDHBUILD
#define HOME_DIRECTORY		"/home/root"
#endif

#ifdef UBUNTU
#define HOME_DIRECTORY		"/home"
#endif

#define UPLINK_DIRECTORY	HOME_DIRECTORY		"/.uplink"
#define DOWNLINK_DIRECTORY	HOME_DIRECTORY		"/.downlink"
#define BEACON_DIRECTORY	HOME_DIRECTORY		"/.beacon"
#define HEALTH_DIRECTORY	HOME_DIRECTORY		"/SD_2/HST"
#define REBOOT_FILE			HOME_DIRECTORY		"/RBT.txt"


#define NEW_SCH				UPLINK_DIRECTORY	"/SCH"
#define DEFAULT_SCH			UPLINK_DIRECTORY	"/DSCH"
#define NEW_DEFAULT_SCH		UPLINK_DIRECTORY	"/NDSCH"

// TODO: Change from HOME_DIRECTORY to DOWNLINK_DIRECTORY
#define DFL_PATH			HOME_DIRECTORY	"/DLF.txt"
#define BCN_GPS				HOME_DIRECTORY	"/GPS"
#define BCN_SYS				HOME_DIRECTORY	"/SYS"

#define ACS_PATH			"/ACS"
#define CDH_PATH			"/CDH"
#define COM_PATH			"/COM"
#define EPS_PATH			"/EPS"
#define GPS_PATH			"/GPS"
#define RAD_PATH			"/RAD"

#define SOT_PATH			UPLINK_DIRECTORY	"/SOT"
#define IEF_PATH			UPLINK_DIRECTORY	"/IEF"
#define PPE_PATH			UPLINK_DIRECTORY	"/PPE"
