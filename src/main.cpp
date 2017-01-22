/*
                                 ___..........__
           _,...._           _."'_,.++8n.n8898n.`"._        _....._
         .'       `".     _.'_.'" _.98n.68n. `"88n. `'.   ,"       `.
        /        .   `. ,'. "  -'" __.68`""'""=._`+8.  `.'     .     `.
       .       `   .   `.   ,d86+889" 8"""+898n, j8 9 ,"    .          \
      :     '       .,   ,d"'"   _..d88b..__ `"868' .'  . '            :
      :     .      .    _    ,n8""88":8"888."8.  "               '     :
       \     , '  , . .88" ,8P'     ,d8. _   `"8n  `+.      `.   .     '
        `.  .. .     d89' "  _..n689+^'8n88n.._ `+  . `  .  , '      ,'
          `.  . , '  8'    .d88+"    j:""' `886n.    b`.  ' .' .   ."
           '       , .j            ,d'8.         `  ."8.`.   `.  ':
            .    .' n8    ,_      .f A 6.      ,..    `8b, '.   .'_
          .' _    ,88'   :8"8    6'.d`i.`b.   d8"8     688.  ".    `'
        ," .88  .d868  _         ,9:' `8.`8   "'  ` _  8+""      b   `,
      _.  d8P  d'  .d :88.     .8'`j   ;+. "     n888b 8  .     ,88.   .
     `   :68' ,8   88     `.   '   :   l `     .'   `" jb  .`   688b.   ',
    .'  .688  6P   98  =+""`.      `   '       ,-"`+"'+88b 'b.  8689  `   '
   ;  .'"888 .8;  ."+b. : `" ;               .: "' ; ,n  `8 q8, '88:       \
   .   . 898  8:  :    `.`--"8.              d8`--' '   .d'  ;8  898        '
  ,      689  9:  8._       ,68 .        .  :89    ..n88+'   89  689,' `     .
  :     ,88'  88  `+88n  -   . .           .        " _.     6:  `868     '   '
  , '  .68h.  68      `"    . . .        .  . .             ,8'   8P'      .   .
  .      '88  'q.    _.f       .          .  .    '  .._,. .8"   .889        ,
 .'     `898   _8hnd8p'  ,  . ..           . .    ._   `89,8P    j"'  _   `
  \  `   .88, `q9868' ,9      ..           . .  .   8n .8 d8'   +'   n8. ,  '
  ,'    ,+"88n  `"8 .8'     . ..           . .       `8688P"   9'  ,d868'   .  .
  .      . `86b.    " .       .            ..          68'      _.698689;      :
   . '     ,889_.n8. ,  ` .   .___      ___.     .n"  `86n8b._  `8988'b      .,6
    '       q8689'`68.   . `  `:. `.__,' .:'  ,   +   +88 `"688n  `q8 q8.     88
    , .   '  "     `+8 n    .   `:.    .;'   . '    . ,89           "  `q,    `8
   .   .   ,        .    + c  ,   `:.,:"        , "   d8'               d8.    :
    . '  8n           ` , .         ::    . ' "  .  .68h.             .8'`8`.  6
     ,    8b.__. ,  .n8688b., .    .;:._     .___nn898868n.         n868b "`   8
      `.  `6889868n8898886888688898"' "+89n88898868868889'         688898b    .8
       :    q68   `""+8688898P ` " ' . ` '  ' `+688988P"          d8+8P'  `. .d8
       ,     88b.       `+88.     `   ` '     .889"'           ,.88'        .,88
        :    '988b        "88b.._  ,_      . n8p'           .d8"'      '     689
        '.     "888n._,      `"8"+88888n.8,88:`8 .     _ .n88P'   .  `      ;88'
         :8.     "q888.  .            "+888P"  "+888n,8n8'"      .  .     ,d986
         :.`8:     `88986                          `q8"           ,      :688"
         ;.  '8,      "88b .d                        '                  ,889'
         :..   `6n      '8988                                         b.89p
         :. .    '8.      `88b                                        988'
         :. .      8b       `q8.        '                     . '   .d89      '
         . .        `8:       `86n,.       " . ,        , "        ,98P      ,
         .. .         '6n.       +86b.        .      .         _,.n88'     .
           .            `"8b.      'q98n.        ,     .  _..n868688'          .
          ' . .            `"98.     `8868.       .  _.n688868898p"            d
           . .                '88.      "688.       q89888688868"            ,86
         mh '. .                 88.     `8898        " .889"'              .988


*/
#include "POSIX.h"
#include "servers/EPSServer.h"
#include "servers/ACSServer.h"
#include "servers/COMServer.h"
#include "servers/CMDServer.h"
#include "servers/SCHServer.h"
#include "servers/GPSServer.h"
#include "servers/PLDServer.h"
#include "servers/CDHServer.h"
#include "servers/FMGServer.h"
#include "servers/SubsystemServer.h"
#include "servers/ERRServer.h"
#include "servers/ErrorQueue.h"
#include "servers/DispatchStdTasks.h"
#include "HAL/Ethernet_Server.h"
#include "HAL/SPI_Server.h"
#include "core/StdTypes.h"
#include "core/WatchdogManager.h"
#include "core/Singleton.h"
#include "core/Factory.h"
#include "core/Dispatcher.h"
#include "core/ModeManager.h"
#include "util/Logger.h"

using namespace std;
using namespace AllStar::Core;
using namespace AllStar::Servers;

int main(int argc, char * argv[])
{
	Logger * logger = dynamic_cast<Logger *> (Factory::GetInstance(LOGGER_SINGLETON));
	logger->Log(LOGGER_LEVEL_FATAL, "Entered Flight Software");

	mq_unlink("/queueHandleSPITX");
	mq_unlink("/queueHandleRX");

	Factory::GetInstance(DISPATCHER_SINGLETON);
	Factory::GetInstance(FILE_HANDLER_SINGLETON);
	Factory::GetInstance(ERROR_QUEUE_SINGLETON);
	Factory::GetInstance(WATCHDOG_MANAGER_SINGLETON);
	ModeManager * modeManager = dynamic_cast<ModeManager *> (Factory::GetInstance(MODE_MANAGER_SINGLETON));

	modeManager->SetMode(MODE_BUS_PRIORITY);

	logger->Log(LOGGER_LEVEL_FATAL, "Flight software initialization complete! Starting servers!");

	// ----------------------------- Grab Server Instances ---------------------------------------------------------
	bool threadsCreated = true;
	ACSServer * acsServer = dynamic_cast<ACSServer *> (Factory::GetInstance(ACS_SERVER_SINGLETON));
	CDHServer * cdhServer = dynamic_cast<CDHServer *> (Factory::GetInstance(CDH_SERVER_SINGLETON));
	CMDServer * cmdServer = dynamic_cast<CMDServer *> (Factory::GetInstance(CMD_SERVER_SINGLETON));
	COMServer * comServer = dynamic_cast<COMServer *> (Factory::GetInstance(COM_SERVER_SINGLETON));
	EPSServer * epsServer = dynamic_cast<EPSServer *> (Factory::GetInstance(EPS_SERVER_SINGLETON));
	ERRServer * errServer = dynamic_cast<ERRServer *> (Factory::GetInstance(ERR_SERVER_SINGLETON));
	FMGServer * fmgServer = dynamic_cast<FMGServer *> (Factory::GetInstance(FMG_SERVER_SINGLETON));
	GPSServer * gpsServer = dynamic_cast<GPSServer *> (Factory::GetInstance(GPS_SERVER_SINGLETON));
	PLDServer * pldServer = dynamic_cast<PLDServer *> (Factory::GetInstance(PLD_SERVER_SINGLETON));
	SCHServer * schServer = dynamic_cast<SCHServer *> (Factory::GetInstance(SCH_SERVER_SINGLETON));
	SPI_HALServer * spiServer = dynamic_cast<SPI_HALServer *> (Factory::GetInstance(SPI_HALSERVER_SINGLETON));

	// ----------------------------- Start Servers -----------------------------------------------------------------
	threadsCreated &= WatchdogManager::StartServer(acsServer, 50,	false);	 //ACS
	threadsCreated &= WatchdogManager::StartServer(cdhServer, 20,	true);	 //CDH
	threadsCreated &= WatchdogManager::StartServer(cmdServer, 50,	true);	 //CMD
	threadsCreated &= WatchdogManager::StartServer(comServer, 10,	false);	 //COM
	threadsCreated &= WatchdogManager::StartServer(epsServer, 10,	false);	 //EPS
	threadsCreated &= WatchdogManager::StartServer(errServer, 0,	false);	 //ERR
	threadsCreated &= WatchdogManager::StartServer(fmgServer, 0,	false);	 //FMG
	threadsCreated &= WatchdogManager::StartServer(gpsServer, 50,	false);	 //GPS
	threadsCreated &= WatchdogManager::StartServer(pldServer, 50,	false);	 //PLD
	threadsCreated &= WatchdogManager::StartServer(schServer, 0,	true);	 //SCH
	threadsCreated &= WatchdogManager::StartServer(spiServer, 0,	false);	 //SPI

	if(!threadsCreated){
		logger->Log(LOGGER_LEVEL_FATAL, "Not all threads were created on startup!");
		// TODO: what to do?
	}else{
		logger->Log(LOGGER_LEVEL_INFO, "All servers created!");
	}

	WatchdogManager::WatchdogManagerTask();

	logger->Log(LOGGER_LEVEL_FATAL, "Flight Software exiting from main! All relevant threads have exited!");

	return 42;
}
