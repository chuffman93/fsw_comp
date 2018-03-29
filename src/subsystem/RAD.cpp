/*
 * RAD.cpp
 *
 *  Created on: Dec 22, 2017
 *      Author: alex
 */

#include "subsystem/RAD.h"

RAD::RAD(ACPInterface& acp, SubPowerInterface& subPower)
: acp(acp), subPower(subPower){
	hsAvailable = false;
	tags += LogTag("Name", "RAD");
	health.fileSize = FileManager::MAX_FILE_SIZE;
	health.basePath = HEALTH_DIRECTORY RAD_PATH "/RAD";
	RADDataNum = 0;
}



RAD::~RAD(){}

bool RAD::initialize(){
	return true;
}


//! Handles any mode transition needs as well as any needs for tasks to be done in a mode.
void RAD::handleMode(FSWMode transition){
	LockGuard l(lock);
	bool success;
	switch (transition){
	case Mode_Reset:
		success = resetRAD();
		break;
	case Trans_BusToPayload:
		success = commandCollectionBegin();
		break;
	case Trans_PayloadToBus:
		success = commandCollectionEnd();
		break;
	default:
		break;
	}

}

void RAD::handleConfig(){}

void RAD::updateConfig(){}

//! Handles the capturing and storing of the health and status for a subsystem (Maybe find someway to implement the autocoding stuff?)
void RAD::getHealthStatus(){
	if(hsAvailable){
		LockGuard l(lock);
		std::vector<uint8_t> buff;
		ACPPacket acpReturn = sendOpcode(OP_HEALTHSTATUS, buff);
		health.recordBytes(acpReturn.message);
	}
}

/*!
 * Handles the sending of opcodes
 * \param opcode to be sent
 * \param buffer to be sent if need be
 */
ACPPacket RAD::sendOpcode(uint8_t opcode, std::vector<uint8_t> buffer){
	if (buffer.empty()){
		ACPPacket acpPacket(RAD_SYNC, opcode);
		ACPPacket acpReturn;
		acp.transaction(acpPacket,acpReturn);
		return acpReturn;
	}else {
		ACPPacket acpPacket(RAD_SYNC, opcode, buffer);
		ACPPacket acpReturn;
		acp.transaction(acpPacket,acpReturn);
		return acpReturn;
	}
}

//! checks if the PLD opcode being sent was done so successfully
bool RAD::isSuccess(PLDOpcode opcode, ACPPacket retPacket){
	if (opcode == retPacket.opcode){
		return true;
	}
	return false;
}

//! checks if the subsystem opcode sent was done so successfully
bool RAD::isSuccess(SubsystemOpcode opcode, ACPPacket retPacket){
	if (opcode == retPacket.opcode){
		return true;
	}
	return false;
}


//! Handles sending the Motor config to RAD
void RAD::configMotor(){
	if (FileManager::checkExistance(RAD_MOTOR_CONFIG_UP)){
		std::vector<uint8_t> buff = FileManager::readFromFile(RAD_MOTOR_CONFIG_UP);
		if (buff.size() == CONFIG_MOTOR_SIZE){
			Logger::Stream(LEVEL_INFO,tags) << "Sending RAD Motor Config Update";
			ACPPacket acpReturn = sendOpcode(OP_MOTORCONFIG,buff);
			if (!isSuccess(OP_MOTORCONFIG,acpReturn)){
				Logger::Stream(LEVEL_ERROR,tags) << "RAD did not reeieve RAD Motor Config Update";
				return;
			}
		}else{
			Logger::Stream(LEVEL_ERROR,tags) << "Incorrect RAD Motor Config Update file Size";
			return;
		}
		FileManager::moveFile(RAD_MOTOR_CONFIG_UP,RAD_MOTOR_CONFIG);
		FileManager::deleteFile(RAD_MOTOR_CONFIG_UP);
	}else if (FileManager::checkExistance(RAD_MOTOR_CONFIG)){
		std::vector<uint8_t> buff = FileManager::readFromFile(RAD_MOTOR_CONFIG);
		if (buff.size() == CONFIG_MOTOR_SIZE){
			Logger::Stream(LEVEL_INFO,tags) << "Sending RAD Motor Config";
			ACPPacket acpReturn = sendOpcode(OP_MOTORCONFIG,buff);
			if (!isSuccess(OP_MOTORCONFIG,acpReturn)){
				Logger::Stream(LEVEL_ERROR,tags) << "RAD did not receive RAD Motor Config";
			}
		}else{
			Logger::Stream(LEVEL_ERROR,tags) << "Incorrect RAD Motor Config file Size";
		}
	}else{
		Logger::Stream(LEVEL_ERROR,tags) << "RAD Motor Config file does not exist";
	}
}


//! Handles sending the DATA config to RAD
void RAD::configData(){
	if (FileManager::checkExistance(RAD_DATA_CONFIG_UP)){
		std::vector<uint8_t> buff = FileManager::readFromFile(RAD_DATA_CONFIG_UP);
		if (buff.size() == CONFIG_DATA_SIZE){
			Logger::Stream(LEVEL_INFO,tags) << "Sending RAD Data Config Update";
			ACPPacket acpReturn = sendOpcode(OP_DATACONFIG,buff);
			if (!isSuccess(OP_DATACONFIG,acpReturn)){
				Logger::Stream(LEVEL_ERROR,tags) << "RAD did not receive RAD Data Config Update";
				return;
			}
		}else{
			Logger::Stream(LEVEL_ERROR,tags) << "Incorrect RAD Data Config Update file Size";
			return;
		}
		FileManager::moveFile(RAD_DATA_CONFIG_UP,RAD_DATA_CONFIG);
		FileManager::deleteFile(RAD_DATA_CONFIG_UP);
	}else if(FileManager::checkExistance(RAD_DATA_CONFIG)){
		std::vector<uint8_t> buff = FileManager::readFromFile(RAD_DATA_CONFIG);
		if (buff.size() == CONFIG_DATA_SIZE){
			Logger::Stream(LEVEL_INFO,tags) << "Sending RAD Data Config";
			ACPPacket acpReturn = sendOpcode(OP_DATACONFIG,buff);
			if (!isSuccess(OP_DATACONFIG,acpReturn)){
				Logger::Stream(LEVEL_ERROR,tags) << "RAD did not receive RAD Data Config";
			}
		}else{
			Logger::Stream(LEVEL_ERROR,tags) << "Incorrect RAD Data Config file Size";
		}
	}else{
		Logger::Stream(LEVEL_ERROR,tags) << "RAD Data Config file does not exist";
	}
}


//! Command the beginning of data collection
bool RAD::commandCollectionBegin(){

	//1. Turn on Rad
	subPower.powerOn();


	Logger::Stream(LEVEL_INFO,tags) << "Beginning RAD Science Collection";
	std::vector<uint8_t> buff;

	//2. Aliveness Test
	ACPPacket retPacket1 = sendOpcode(OP_TESTALIVE, buff);
	if (!isSuccess(OP_TESTALIVE,retPacket1)){
		Logger::Stream(LEVEL_FATAL,tags) << "Opcode Test Alive: RAD is not alive. Opcode Received: " << retPacket1.opcode;
		return false;
	}

	ACPPacket retPacket2 = sendOpcode(OP_TESTLED, buff);
	if (!isSuccess(OP_TESTLED,retPacket2)){
		Logger::Stream(LEVEL_FATAL,tags) << "Opcode Test Alive: RAD is not alive. Opcode Received: " << retPacket2.opcode;
		return false;
	}

	ACPPacket retPacket3 = sendOpcode(OP_TESTCONFIG, buff);
	if (!isSuccess(OP_TESTCONFIG,retPacket3)){
		Logger::Stream(LEVEL_FATAL,tags) << "Opcode Test Alive: RAD is not alive. Opcode Received: " << retPacket3.opcode;
		return false;
	}



	//3. Initialize TFTP
	dataFile = FileManager::createFileName(RAD_FILE_PATH);
	Logger::Stream(LEVEL_DEBUG,tags) << "File path for transfer of RAD data: " << dataFile.c_str();
	char* argv[] = {(char *)"/usr/bin/tftp",(char *)"-g",(char*)"-r",(char*)RAD_TMP_DATA,(char*)"10.14.134.207",NULL};
	tftp.launchProcess(argv,FALSE);

	//4. Configure MiniRAD
	hsAvailable = true;
	configData();
	configMotor();

	//5. Command Start of Science
	ACPPacket retPacket4 = sendOpcode(OP_STARTSCIENCE, buff);
	if (!isSuccess(OP_STARTSCIENCE,retPacket4)){
		Logger::Stream(LEVEL_FATAL,tags) << "Opcode Start Science: RAD unable to collect data. Opcode Received: " << retPacket4.opcode;
		return false;
	}

	return true;
}

//! Command the ending of data collection
bool RAD::commandCollectionEnd(){
	//TODO: error handling
	hsAvailable = false;

	Logger::Stream(LEVEL_INFO,tags) << "Ending RAD Science Collection";
	std::vector<uint8_t> buff;
	//1. Command end of science mode
	ACPPacket retPacket = sendOpcode(OP_SUBSYSTEMRESET,buff);
	if (!isSuccess(OP_SUBSYSTEMRESET,retPacket)){
		Logger::Stream(LEVEL_FATAL,tags) << "Opcode Subsystem Reset: Unable to power of RAD. Opcode Received: " << retPacket.opcode;
		return false;
	}


	//2. Waitfor TFTP to finish and close tftp
	usleep(3*1000*1000);
	tftp.closeProcess();


	//3. Turn off RAD
	std::string s(RAD_TMP_DATA);
	FileManager::moveFile(s,dataFile);
	subPower.powerOff();

	//4. Split Data Into Chunks
	int splits = splitData();

	//5. Tar Each File Chunk
	if(splits > 0){
		tarBallData(splits);
	}

	return true;
}

//! resets RAD
bool RAD::resetRAD(){

	Logger::Stream(LEVEL_INFO,tags) << "Preparing RAD for Reset";

	std::vector<uint8_t> buff;
	ACPPacket retPacket = sendOpcode(OP_SUBSYSTEMRESET,buff);
	if (!isSuccess(OP_SUBSYSTEMRESET,retPacket)){
		Logger::Stream(LEVEL_FATAL,tags) << "Opcode Subsystem Reset: Unable to reset subsystem. Opcode Received: " << retPacket.opcode;
		return false;
	}
	return true;
}

//! Splits the data collected into smaller chunks for easier/quicker downlink
int RAD::splitData(){
	ExternalProcess split;

	char dataPath[100];
	char chunksize[10];
	//create archive name string for path to dataFile
	sprintf(dataPath,"%s",dataFile.c_str());
	// get how many files it was split into by dividing the dataFile size by the number of bytes per chunk
	std::ifstream in(dataPath, std::ifstream::ate | std::ifstream::binary);
	long f_bytes = in.tellg();
	int n_splits = f_bytes/RAD_CHUNK_SIZE;
	in.close();
	// split the file within the location using the same name (tags on 000,001,002,ect.)
	if(n_splits > 0){
		sprintf(chunksize,"%d",RAD_CHUNK_SIZE);
		char * sh_cmd[] = {(char *)"/usr/bin/split", (char*) "-b", (char*)chunksize, (char*) "-d",(char*) "-a", (char*)"3",(char*)dataPath,(char*)dataPath,NULL};
		split.launchProcess(sh_cmd);
		Logger::Stream(LEVEL_INFO,tags) << "Data file successfully split";
		remove(dataPath);
		return n_splits;
	}else{
		Logger::Stream(LEVEL_WARN,tags) << "No Data file to be split/collected";
		return 0;
	}
}

/*!
 * Tar balls the split files from the data collection
 * \param number of splits to look for and to be tar balled
 */
void RAD::tarBallData(int splits){
	ExternalProcess tar;
	char archiveName[100];
	char chunk[100];
	// tftp.closeProcess();
	Logger::Stream(LEVEL_INFO,tags) << "Beginning of tar-balling of data file splits";
	// for loop through the number of splits created
	for(int i = 0; i <= splits; i++){
		// gets archive name we wish to create a .tar.gz compressed file for each chunk
		sprintf(archiveName,"%s%03d.tar.gz",dataFile.c_str(),i);
		sprintf(chunk,"%s%03d",dataFile.c_str(),i);

		char * sh_cm[] = {(char*)"/bin/tar", (char*)"-czf",(char*)archiveName,(char*)chunk,(char*)"-P",NULL};
		// runs the command on the system
		tar.launchProcess(sh_cm);

		// create a differenPLDUpdateDataNumbert archiveName referencing just the individual chunks
		sprintf(archiveName,"%s%03d",dataFile.c_str(),i);
		// removes the chunks to save some space
		remove(archiveName);
	}
	Logger::Stream(LEVEL_INFO,tags) << "End of tar-balling of data file splits";
}

