/*
 * RAD.cpp
 *
 *  Created on: Dec 22, 2017
 *      Author: alex
 */

#include "subsystem/RAD.h"

RAD::RAD(ACPInterface& acp, SubPowerInterface& subPower)
: acp(acp), subPower(subPower){
	tags += LogTag("Name", "RAD");
	health.fileSize = 0;
	health.basePath = HEALTH_DIRECTORY RAD_PATH "/RAD";
	RADDataNum = 0;
}



RAD::~RAD(){}

bool RAD::initialize(){
	return true;
}


//Handles any mode transition needs as well as any needs for tasks to be done in a mode.
void RAD::handleMode(FSWMode transition){
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

//Handles the capturing and storing of the health and status for a subsystem (Maybe find someway to implement the autocoding stuff?)
void RAD::getHealthStatus(){

	LockGuard l(lock);
	std::vector<uint8_t> buff;
	ACPPacket acpReturn = sendOpcode(OP_HEALTHSTATUS, buff);

	std::string healthFile;
	size_t messageSize = acpReturn.message.size();
	if ((health.fileSize+messageSize) < MAX_FILE_SIZE){
		healthFile = health.currentFile;
		health.fileSize += messageSize;
	}else{
		healthFile = FileManager::createFileName(health.basePath);
		health.fileSize = messageSize;
	}
	FileManager::writeToFile(healthFile,acpReturn.message);
}

ACPPacket RAD::sendOpcode(uint8_t opcode, std::vector<uint8_t> buffer){
	//LockGuard l(lock);
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

bool RAD::isSuccess(PLDOpcode opcode, ACPPacket retPacket){
	if (opcode == retPacket.opcode){
		return true;
	}
	return false;
}

bool RAD::isSuccess(SubsystemOpcode opcode, ACPPacket retPacket){
	if (opcode == retPacket.opcode){
		return true;
	}
	return false;
}


//Various configurations for the data collection
void RAD::configMotor(){

}

void RAD::configData(){

}

//Command the beginning of data collection
bool RAD::commandCollectionBegin(){
	//TODO: error handling

	LockGuard l(lock);
	//subPower.powerOn();

	Logger::Stream(LEVEL_INFO,tags) << "Beginning RAD Science Collection";
	std::vector<uint8_t> buff;

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

	ACPPacket retPacket4 = sendOpcode(OP_STARTSCIENCE, buff);
	if (!isSuccess(OP_STARTSCIENCE,retPacket4)){
		Logger::Stream(LEVEL_FATAL,tags) << "Opcode Start Science: RAD unable to collect data. Opcode Received: " << retPacket4.opcode;
		return false;
	}


	sprintf(dataFile, "RAD_%u", RADDataNum);
	RADDataNum = updateDataNumber();

	// TODO: get correct IP and make sure this runs as intended
	// char* argv[] = {(char *)"/usr/bin/tftp",(char *)"-g",(char*)"-r",dataFile,(char*)"10.14.134.207",NULL};
	// tftp.launchProcess(argv);

	return true;
}


bool RAD::commandCollectionEnd(){
	//TODO: error handling
	//TODO: SPLIT DATA PROCESSING AND TARBALLING INTO THIER OWN FUNCTIONS

	LockGuard l(lock);

	Logger::Stream(LEVEL_INFO,tags) << "Ending RAD Science Collection";
	std::vector<uint8_t> buff;
	ACPPacket retPacket = sendOpcode(OP_SUBSYSTEMRESET,buff);
	if (!isSuccess(OP_SUBSYSTEMRESET,retPacket)){
		Logger::Stream(LEVEL_FATAL,tags) << "Opcode Subsystem Reset: Unable to power of RAD. Opcode Received: " << retPacket.opcode;
		return false;
	}

	//subPower.powerOff();

	char archiveName[100];
	char chunksize[10];
	char dataPath[100];
	char chunk[100];
	// tftp.closeProcess();

	//create archive name string for path to dataFile
	sprintf(dataPath,MOCK_RAD_PATH"/%s",dataFile);
	// get how many files it was split into by dividing the dataFile size by the number of bytes per chunk
	std::ifstream in(dataPath, std::ifstream::ate | std::ifstream::binary);
	int f_bytes = in.tellg();
	int n_splits = f_bytes/RAD_CHUNK_SIZE;
	in.close();


	// split the file within the location using the same name (tags on 000,001,002,ect.)
	sprintf(chunksize,"%d",RAD_CHUNK_SIZE);
	char * sh_cmd[] = {(char *)"/usr/bin/split", (char*) "-b", (char*)chunksize, (char*) "-d",(char*) "-a", (char*)"3",(char*)dataPath,(char*)dataPath,NULL};

	// runs the split command in the system
	ExternalProcess tar;
	ExternalProcess split;


	split.launchProcess(sh_cmd);

	// for loop through the number of splits created
	for(int i = 0; i <= n_splits; i++){
		int i2 = 0; // middle 0 if i < 10
		char num[2]; // string to assign the numbers too, to reference each split file
		if(i >= 10){
			//num = to_string(i); // if i >=10 only need i to create the "000" tag on the file
			sprintf(num,"%d",i);
		}
		else{
			//num = to_string(i2) + to_string(i); // other wise need i2 and i
			sprintf(num,"%d%d",i2,i);
		}
		// gets archive name we wish to create a .tar.gz compressed file for each chunk
		sprintf(archiveName, MOCK_RAD_PATH"/%s0%s.tar.gz",dataFile,num);
		sprintf(chunk, MOCK_RAD_PATH"/%s0%s",dataFile,num);

		char * sh_cm[] = {(char*)"/bin/tar", (char*)"-czf",(char*)archiveName,(char*)chunk,(char*)"-P",NULL};
		// runs the command on the system
		tar.launchProcess(sh_cm);

		// create a differenPLDUpdateDataNumbert archiveName referencing just the individual chunks
		sprintf(archiveName, MOCK_RAD_PATH "/%s0%s",dataFile,num);
		// removes the chunks to save some space
		remove(archiveName);

		return true;
	}

	// removes the dataFile to save space
	remove(dataPath);
}

uint16_t RAD::readDataNumber(){
	uint16_t dataNumber;
	FILE * fp;
	int char1, char2;
	bool success = true;

	if(access(RAD_NUM_FILE,F_OK) != -1){
		fp = fopen(RAD_NUM_FILE,"rb");
		if(fp != NULL){
			success &= (char1 = fgetc(fp)) != EOF;
			success &= (char2 = fgetc(fp)) != EOF;
			if(success){
				dataNumber = ((uint16_t) char1) << 8 | ((uint16_t) char2);
			}
			fclose(fp);
		}
	}
	return dataNumber;
}
uint16_t RAD::updateDataNumber(){
	uint16_t dataNumber;
	FILE * fp;
	uint8_t buffer[2];
	bool success = true;

	dataNumber = readDataNumber();
	dataNumber++;

	buffer[0] = (dataNumber & 0xFF00) >> 8;
	buffer[1] = (dataNumber & 0x00FF);

	fp =fopen(RAD_NUM_FILE,"wb");
	if(fp!=NULL){
		success &= (fputc(buffer[0], fp) != EOF);
		success &= (fputc(buffer[1], fp) != EOF);
		fclose(fp);
	}
	if (!success) {
		remove(RAD_NUM_FILE);
	}
	return dataNumber;
}

bool RAD::resetRAD(){

	Logger::Stream(LEVEL_INFO,tags) << "Preparing COM for Reset";

	std::vector<uint8_t> buff;
	ACPPacket retPacket = sendOpcode(OP_SUBSYSTEMRESET,buff);
	if (!isSuccess(OP_SUBSYSTEMRESET,retPacket)){
		Logger::Stream(LEVEL_FATAL,tags) << "Opcode Subsystem Reset: Unable to reset subsystem. Opcode Received: " << retPacket.opcode;
		return false;
	}
	return true;
}


