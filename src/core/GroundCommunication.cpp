/*
 * GroundCommunication.cpp
 *
 *  Created on: Jan 15, 2018
 *      Author: alpa3119
 */

#include "core/GroundCommunication.h"

GroundCommunication::GroundCommunication(std::vector<SubsystemBase*> subsystems, BeaconManager& beacon)
: stateDownlink(false), statePostPass(false), ComStartTime(0), ComTimeout(720), subsystems(subsystems), beacon(beacon),com(0)
{
	tags += LogTag("Name", "GroundCommunication");
	firstFile = true;
}

GroundCommunication::~GroundCommunication(){}

/*
 * This function should be called in SpinGround() whenever the stateDownlink flag is true
 * Places requested downlink files in the downlink folder one by one
 * Next file will be added to the folder when the previous file has been removed and downlinked by com system
 * Front of the queue should be the file we are currently trying to downlink
 */
void GroundCommunication::downlinkFiles(){
	// Continues downlink if the queue is not empty
	if (!DownlinkQueue.empty()){
		std::string file = DownlinkQueue.front();
		// Place first file in downlink directory
		if (firstFile == true){
			Logger::Stream(LEVEL_INFO,tags) << "Downlinking First File" << grabFileName(file).c_str();
			FileManager::copyFile(file, DOWNLINK_DIRECTORY + grabFileName(file));
			// If the first file is not longer in the downlink directory, ERROR
			if (!FileManager::checkExistance(DOWNLINK_DIRECTORY + grabFileName(file))){
				Logger::Stream(LEVEL_DEBUG,tags) << "Error copying: " << file << "to " << DOWNLINK_DIRECTORY + grabFileName(file);
			}
			firstFile = false;
		}else if (!FileManager::checkExistance(DOWNLINK_DIRECTORY + grabFileName(file))){
			DownlinkQueue.pop_front();
			if (!DownlinkQueue.empty()){
				file = DownlinkQueue.front();
				Logger::Stream(LEVEL_INFO,tags) << "Downlinking Next File: " << grabFileName(file).c_str();
				FileManager::copyFile(file, DOWNLINK_DIRECTORY + grabFileName(file));
			}
		}
	}
	// Concludes downlink if the queue is empty, should probably be an else statement
	if (DownlinkQueue.empty()){
		stateDownlink = false;
		statePostPass = true; // downlinking is always the last task in a com pass, so we must trigger post pass tasks
		firstFile = true; // resets flag for next time we downlink
		Logger::Stream(LEVEL_INFO,tags) << "Completed Downlink of All Files";
	}
}

/*
 * Clears the downlink queue
 * Should be called when the communication pass is over
 */
void GroundCommunication::clearDownlink(){
	Logger::Stream(LEVEL_INFO,tags) << "Communication Pass over, clearing downlink queue";
	DownlinkQueue.clear();
	firstFile = true;
}

/*
 * Helpful function for removing the newline in a string
 * Used when parsing the ground files
 */
std::string GroundCommunication::trimNewline(std::string buffer){
	  // Remove the newline at the end of a string
	buffer.erase(std::remove(buffer.begin(), buffer.end(), '\n'), buffer.end());
	return buffer;
}

/*
 * Method for parsing and handling the ground downlink requests
 * Parameters: F (single file), R (entire regex), RB (regex before specified com pass), RA (regex after specified com pass)
 * Adds files to downlink queue in the order that it appears in the ground file
 */
void GroundCommunication::parseDownlinkRequest(std::string line){
	char downlinkRequest[100];
	strcpy(downlinkRequest, line.c_str());

	char * savePtr;
	char * dwl = strtok_r(downlinkRequest,",", &savePtr);

	char * type = strtok_r(NULL, ",", &savePtr);
	if (type == NULL){
		//log error
		return;
	}
	char * file = strtok_r(NULL, ",", &savePtr);
	if (file == NULL){
		//log error
		return;
	}
	if (strcmp(type,"F") == 0){
		Logger::Stream(LEVEL_INFO,tags) << "Adding file(s): " << line.c_str() << " to downlink queue";
		std::string path = trimNewline(std::string(file));
		DownlinkQueue.push_back(path);
		file = strtok_r(NULL,",", &savePtr);
	}else if (strcmp(type,"R") == 0){
		Logger::Stream(LEVEL_INFO,tags) << "Fetching R regex for: " << line.c_str();
		Logger::Stream(LEVEL_DEBUG,tags) << "Processing R regex for: " << file;
		std::string regex = trimNewline(std::string(file));
		std::vector<std::string> reg = FileManager::packageFiles(regex,"R");
		std::vector<std::string>::iterator it;
		for (it = reg.begin(); it < reg.end(); it++){
			Logger::Stream(LEVEL_DEBUG,tags) << "Adding: " <<  (*it).c_str()  << " to downlink queue";
			DownlinkQueue.push_back(*it);
		}
	}else if (strcmp(type,"RB") == 0){
		Logger::Stream(LEVEL_INFO,tags) << "Adding regex(s) before " << line << " to downlink queue";
		std::string regex = trimNewline(std::string(file));
		std::vector<std::string> reg = FileManager::packageFiles(regex,"RB");
		std::vector<std::string>::iterator it;
		for (it = reg.begin(); it < reg.end(); it++){
			Logger::Stream(LEVEL_DEBUG,tags) << "Adding: " <<  (*it).c_str()  << " to downlink queue";
			DownlinkQueue.push_back(*it);
		}
	}else if (strcmp(type,"RA") == 0){
		Logger::Stream(LEVEL_INFO,tags) << "Adding regex after(s) " << line << " to downlink queue";
		std::string regex = trimNewline(std::string(file));
		std::vector<std::string> reg = FileManager::packageFiles(regex,"RA");
		std::vector<std::string>::iterator it;
		for (it = reg.begin(); it < reg.end(); it++){
			Logger::Stream(LEVEL_DEBUG,tags) << "Adding: " << (*it).c_str() << " to downlink queue";
			DownlinkQueue.push_back(*it);
		}
	}else {
		//log error
	}
}

/*
 * Method for parsing and handling deletion requests
 * Parameters: F (single file), R (entire regex), RB (regex before specified com pass), RA (regex after specified com pass)
 */
void GroundCommunication::parseDeletionRequest(std::string line){
	char deleteRequest[100];
	strcpy(deleteRequest, line.c_str());

	char * savePtr;
	char * del = strtok_r(deleteRequest, ",", &savePtr);

	char * type = strtok_r(NULL, ",", &savePtr);
	if (type == NULL){
		//log error
		return;
	}
	char * file = strtok_r(NULL, ",", &savePtr);
	if (file == NULL){
		//log error
		return;
	}
	if (strcmp(type,"F") == 0){
		Logger::Stream(LEVEL_INFO,tags) << "Deleting file(s) " << line ;
		std::string path = trimNewline(std::string(file));
		FileManager::deleteFile(path);
	}if (strcmp(type,"R") == 0){
		Logger::Stream(LEVEL_INFO,tags) << "Deleting regex(s) " << line ;
		std::string regex = trimNewline(std::string(file));
		FileManager::regexDelete(regex,"R");
	}if (strcmp(type,"RB") == 0){
		Logger::Stream(LEVEL_INFO,tags) << "Deleting regex(s) before " << line ;
		std::string regex = trimNewline(std::string(file));
		FileManager::regexDelete(regex,"RB");
	}if (strcmp(type,"RA") == 0){
		Logger::Stream(LEVEL_INFO,tags) << "Deleting regex(s) after " << line ;
		std::string regex = trimNewline(std::string(file));
		FileManager::regexDelete(regex,"RA");
	}
}

/*
 * Method for parsing and executing ground requests
 * Parameters: SYS (system/run through the CDH command line), ACS (opcode), COM (opcode), EPS (opcode), RAD (opcode)
 * Adds command acknowledgment to the queue, either S for success or F for failure
 */
void GroundCommunication::parseCommandRequest(std::string line){
	//TODO: error handling for opcodes numbers in between min and max that don't exist
	char commandRequest[100];
	strcpy(commandRequest, line.c_str());

	char * savePtr;
	char * c = strtok_r(commandRequest,",",&savePtr);

	char * sys = strtok_r(NULL,",",&savePtr);
	if (sys == NULL){
		Logger::Stream(LEVEL_ERROR,tags) << "ParseCommandRequest: No system given for command execution";
		CommandAcknowledgements.push_back(line+",F\n");
		return;
	}

	char * command = strtok_r(NULL, ",",&savePtr);
	if (command == NULL){
		Logger::Stream(LEVEL_ERROR,tags) << "ParseCommandRequest: No command given for command execution";
		CommandAcknowledgements.push_back(line+",F\n");
		return;
	}

	if (strcmp(sys,"SYS") == 0){
		std::string cmd = trimNewline(std::string(command));
		command = (char*) cmd.c_str();
		Logger::Stream(LEVEL_INFO,tags) << "Executing System Command: " << command;
		int16_t ret = system(command);
		if (ret == 0 || ret == -1){
			CommandAcknowledgements.push_back("SYS," + cmd + ",F\n");
		}else{
			CommandAcknowledgements.push_back("SYS," + cmd + ",S\n");
		}
		Logger::Stream(LEVEL_INFO,tags) << "System Command Return: " << ret;
	}else if (strcmp(sys, "ACS") == 0){
		std::string tempCmd = trimNewline(std::string(command));
		uint8_t cmd = (uint8_t)atoi((char*)tempCmd.c_str());
		if ((cmd >= OP_ACS_MIN && cmd < OP_ACS_MAX) || (cmd >= OP_MIN && cmd < OP_MAX)){
			Logger::Stream(LEVEL_INFO,tags) << "Executing ACS command: " << command;
			std::vector<uint8_t> buff;
			ACPPacket ret = subsystems[2]->sendOpcode(cmd,buff);
			if (ret.opcode == cmd){
				CommandAcknowledgements.push_back("ACS," + tempCmd + ",S\n");
			}else{
				CommandAcknowledgements.push_back("ACS," + tempCmd + ",F\n");
			}
		}else{
			Logger::Stream(LEVEL_ERROR,tags) << "Unable to Execute ACS Command, command does not exist";
			CommandAcknowledgements.push_back("ACS," + tempCmd + ",F\n");
		}
	}else if (strcmp(sys, "COM") == 0){
		std::string tempCmd = trimNewline(std::string(command));
		uint8_t cmd = (uint8_t)atoi((char*)tempCmd.c_str());
		//SHHHHHHH... secret things
		if (cmd == OP_TX_ARM){
			Logger::Stream(LEVEL_INFO,tags) << "Executing COM command: ARMING KILL COMMAND";
			com->setKillCom(true);
		}else if (cmd == OP_TX_KILL){
			if(com->getKillCom()){
				Logger::Stream(LEVEL_INFO,tags) << "Executing COM command: TRANSMISSION OFF";

				//Kill comm daemon
				ExternalProcess da_off;
				char * cm_da_off[] = {(char*)"/usr/bin/pkill",(char*)"comm_daemon_arm",NULL};
				da_off.launchProcess(cm_da_off);

				//Update TX file
				std::string farewell = "Goodbye PolarCube!\n";
				std::vector<std::string> buff;
				buff.push_back(farewell);
				FileManager::writeToStringFile(TX_FILE,buff);
			}else{
				Logger::Stream(LEVEL_ERROR,tags) << "Executing COM command: END TRANSMISSION NOT ARMED";
			}

		}else if ((cmd >= OP_COM_MIN && cmd < OP_COM_MAX) || (cmd >= OP_MIN && cmd < OP_MAX)){
			Logger::Stream(LEVEL_INFO,tags) << "Executing COM command: " << command;
			std::vector<uint8_t> buff;
			ACPPacket ret = subsystems[1]->sendOpcode(cmd,buff);
			if (ret.opcode == cmd){
				CommandAcknowledgements.push_back("COM," + tempCmd + ",S\n");
			}else{
				CommandAcknowledgements.push_back("COM," + tempCmd + ",F\n");
			}
		}else{
			Logger::Stream(LEVEL_ERROR,tags) << "Unable to Execute COM Command, command does not exist";
			CommandAcknowledgements.push_back("COM," + tempCmd + ",F\n");
		}
	}else if (strcmp(sys, "EPS") == 0){
		std::string tempCmd = trimNewline(std::string(command));
		uint8_t cmd = (uint8_t)atoi((char*)tempCmd.c_str());
		if ((cmd >= OP_EPS_MIN && cmd < OP_EPS_MAX) || (cmd >= OP_MIN && cmd < OP_MAX)){
			Logger::Stream(LEVEL_INFO,tags) << "Executing EPS command: " << command;
			std::vector<uint8_t> buff;
			ACPPacket ret = subsystems[0]->sendOpcode(cmd,buff);
			if (ret.opcode == cmd){
				CommandAcknowledgements.push_back("EPS," + tempCmd + ",S\n");
			}else{
				CommandAcknowledgements.push_back("EPS," + tempCmd + ",F\n");
			}
		}else{
			Logger::Stream(LEVEL_ERROR,tags) << "Unable to Execute EPS Command, command does not exist";
			CommandAcknowledgements.push_back("EPS," + tempCmd + ",F\n");
		}
	}else if (strcmp(sys, "RAD") == 0){
		std::string tempCmd = trimNewline(std::string(command));
		uint8_t cmd = (uint8_t)atoi((char*)tempCmd.c_str());
		if ((cmd >= OP_PLD_CMD_MIN && cmd < OP_PLD_CMD_MAX) || (cmd >= OP_MIN && cmd < OP_MAX)){
			Logger::Stream(LEVEL_INFO,tags) << "Executing RAD command: " << command;
			std::vector<uint8_t> buff;
			ACPPacket ret = subsystems[3]->sendOpcode(cmd,buff);
			if (ret.opcode == cmd){
				CommandAcknowledgements.push_back("RAD," + tempCmd + ",S\n");
			}else{
				CommandAcknowledgements.push_back("RAD," + tempCmd + ",F\n");
			}
		}else{
			Logger::Stream(LEVEL_ERROR,tags) << "Unable to Execute RAD Command, command does not exist";
			CommandAcknowledgements.push_back("RAD," + tempCmd + ",F\n");
		}
	}
}

/*
 * Method for parsing and handling the file list requests
 * Must be given and directory path
 */
void GroundCommunication::parseFileListRequest(std::string line){
	char downlinkRequest[100];
	strcpy(downlinkRequest, line.c_str());

	char * savePtr;
	char * request = strtok_r(downlinkRequest,",",&savePtr);
	char * dir = strtok_r(NULL, ",",&savePtr);
	if (dir == NULL){
		//log error
	}
	while (dir != NULL){
		std::string directory = trimNewline(std::string(dir));
		FileManager::generateFilesList(directory);
		dir = strtok_r(NULL, ",",&savePtr);
	}
	DownlinkQueue.push_back(DFL_PATH);
}

/*
 * Adds command acknowledgments to the front of the downlink queue since they have priority over all other files
 * This should be called after the IEF has been parsed
 */
void GroundCommunication::sendCommandAcknowledgements(){
	std::string filePath = FileManager::createFileName(COMMAND_DIRECTORY);
	FileManager::writeToStringFile(filePath,CommandAcknowledgements);

	DownlinkQueue.push_front(filePath);
	CommandAcknowledgements.clear();
}

/*
 * Helpful method for downlinking files
 * Splits the file name from the entire path so it can be moved to the downlink directory
 */
std::string GroundCommunication::grabFileName(std::string path){
	int i = path.length()-1;
	std::string filename = "";
	while(path[i]!='/'){
		filename = path[i]+filename;
		i--;
	}
	Logger::Stream(LEVEL_DEBUG,tags) << "Grabbed file name for downlink: " << filename.c_str();
	return filename;
}


/*
 * Method for handling IEF requests
 * Parameters: CMD (system/subsytem commands), DWL (file/regex downlinks), DLT (file/regex deletions), DFL (downlink list of files within a directory)
 */
void GroundCommunication::parseIEF(){
	std::vector<std::string> requests = FileManager::parseGroundFile(IEF_PATH);
	char line[100];
	char * type;
	char * savePtr;

	std::vector<std::string>::iterator it;
	for (it = requests.begin(); it != requests.end(); it++){
		strcpy(line, (*it).c_str());
		type = strtok_r(line,",",&savePtr);
		if (type == NULL){
			continue;
		}else if (strcmp(type,"CMD") == 0){
			parseCommandRequest((*it).c_str());
		}else if (strcmp(type,"DWL") == 0){
			parseDownlinkRequest((*it).c_str());
		}else if (strcmp(type,"DLT") == 0){
			parseDeletionRequest((*it).c_str());
		}else if (strcmp(type, "DFL") == 0){
			parseFileListRequest((*it).c_str());
		}
	}
	//Adds command acknowledgments to the front of the downlink queue post IEF processing
	if (!CommandAcknowledgements.empty()){
		sendCommandAcknowledgements();
	}
}



/*!
 * Heart of the GroundComm. Checks for SOT.txt for start of transmission and runs through the process for a COM pass.
 * Also sends beacon.
 * \param A pointer to the watchdog to allow spin to let watchdog know ground is not dead
 */
bool GroundCommunication::spinGround(Watchdog* watchdog){

	// Send the beacon, no communication has been established with the ground
	if (!FileManager::checkExistance(SOT_PATH)){
		Logger::Stream(LEVEL_INFO,tags) << "Sending beacon...";
		beacon.sendBeacon();
		Logger::Stream(LEVEL_INFO,tags) << "Beacon has been sent";
		for(int i = 0; i <= 60; i++){
			if(FileManager::checkExistance(SOT_PATH)){break;}
			watchdog->KickWatchdog();
			sleep(1);
		}
		return false;
	// Ground has communicated with the satellite to begin communication pass
	}else{
		if (ComStartTime == 0){
			Logger::Stream(LEVEL_INFO,tags) << "Beginning Communication Pass Timer";
			ComStartTime = getCurrentTime();
			return true;
		}
		Logger::Stream(LEVEL_DEBUG,tags) << "Start Time: " << ComStartTime << " ComTimeOut: " << ComTimeout+ComStartTime << " Current Time: " << getCurrentTime();
		//check if the communication pass has exceeded its time limit
		if ((ComStartTime + ComTimeout) < getCurrentTime()){
			Logger::Stream(LEVEL_INFO,tags) << "Communication Pass Timeout";
			if (FileManager::checkExistance(IEF_PATH)){
				FileManager::deleteFile(IEF_PATH);
			}
			ComStartTime = 0;
			stateDownlink = false;
			clearDownlink();
			statePostPass = true;
			if(com->getKillCom()){
				com->setKillCom(false);
				Logger::Stream(LEVEL_WARN,tags) << "End Transmission disarmed";
			}
			FileManager::deleteFile(COM_MODE);
			return false;

		}
		//begin IEF processing if IEF has been uplinked
		if (FileManager::checkExistance(IEF_PATH)){
			Logger::Stream(LEVEL_INFO,tags) << "Received IEF";
			parseIEF();
			stateDownlink = true;
		//begin downlink if IEF processing has ended
		}else if (stateDownlink){
			Logger::Stream(LEVEL_INFO,tags) << "Beginning Requested Downlink";
			downlinkFiles();
		//begin post pass processes if downlink has concluded or if com has timed out
		}else if (statePostPass){
			Logger::Stream(LEVEL_INFO,tags) << "Entering Communication Post Pass";
			FileManager::deleteFile(SOT_PATH);
			FileManager::deleteFile(DFL_PATH);
			statePostPass = false;
			ComStartTime = 0;
			clearDownlink();
			if(com->getKillCom()){
				com->setKillCom(false);
				Logger::Stream(LEVEL_WARN,tags) << "End Transmission disarmed";
			}
			FileManager::deleteFile(COM_MODE);
			return false;
		}
	}
	return true;
}

