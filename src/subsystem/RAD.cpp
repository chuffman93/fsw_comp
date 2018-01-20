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
	health.sync = RAD_SYNC;
	health.fileSize = MAX_FILE_SIZE;
	health.basePath = HEALTH_DIRECTORY RAD_PATH "/RAD";
}



RAD::~RAD(){}

//Handle power on and initialization routine
void RAD::initialize(){
	//TODO: error handling

	//read data number

	//boot config

}

//Handles any mode transition needs as well as any needs for tasks to be done in a mode.
void RAD::handleMode(FSWMode transition){
	switch (transition){
	case Mode_Reset:
		//TODO: reset
		break;
	case Trans_BusToPayload:
		commandCollectionBegin();
		break;
	case Trans_PayloadToBus:
		commandCollectionEnd();
		break;
	default:
		break;
	}
}

//Handles the capturing and storing of the health and status for a subsystem (Maybe find someway to implement the autocoding stuff?)
void RAD::getHealthStatus(){

	LockGuard l(lock);
	ACPPacket acpPacket(health.sync, OP_HEALTHSTATUS);
	ACPPacket acpReturn;
	acp.transaction(acpPacket,acpReturn);

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


//Various configurations for the data collection
void RAD::configMotor(){

}

void RAD::configData(){

}

//Command the beginning of data collection
void RAD::commandCollectionBegin(){
	//TODO: error handling

	LockGuard l(lock);

	Logger::Stream(LEVEL_INFO,tags) << "Beginning RAD Science Collection";
	ACPPacket acpPacket(RAD_SYNC, OP_TESTALIVE);
	ACPPacket acpReturn;
	acp.transaction(acpPacket,acpReturn);

	acpPacket.opcode = OP_TESTLED;
	acp.transaction(acpPacket,acpReturn);

	acpPacket.opcode = OP_TESTCONFIG;
	acp.transaction(acpPacket,acpReturn);

	acpPacket.opcode = OP_STARTSCIENCE;
	acp.transaction(acpPacket, acpReturn);
}


void RAD::commandCollectionEnd(){
	//TODO: error handling

	LockGuard l(lock);

	Logger::Stream(LEVEL_INFO,tags) << "Ending RAD Science Collection";
	ACPPacket acpPacket(RAD_SYNC, OP_SUBSYSTEMRESET);
	ACPPacket acpReturn;
	acp.transaction(acpPacket, acpReturn);

	//subPower.powerOff();

	char archiveName[100];
	char chunksize[10];
	char dataPath[100];
	char chunk[100];

	// TODO:  change random to proper rad naming convention
	sprintf(dataFile,"%s","Random");

	//create archive name string for path to dataFile
	sprintf(dataPath,MOCK_RAD_PATH"/%s",dataFile);
	// get how many files it was split into by dividing the dataFile size by the number of bytes per chunk
	std::ifstream in(dataPath, std::ifstream::ate | std::ifstream::binary);
	int f_bytes = in.tellg();
	int n_splits = f_bytes/cs.chunkSize;
	in.close();


	// split the file within the location using the same name (tags on 000,001,002,ect.)
	sprintf(chunksize,"%d",cs.chunkSize);
	char * sh_cmd[] = {(char *)"/usr/bin/split", (char*) "-b", (char*)chunksize, (char*) "-d",(char*) "-a", (char*)"3",(char*)dataPath,(char*)dataPath,NULL};

	// runs the split command in the system
	ExternalProcess tar;
	ExternalProcess split;


	split.launchProcess(sh_cmd);
	sleep(1);
	split.closeProcess();
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
		sleep(1);
		tar.closeProcess();

		// create a different archiveName referencing just the individual chunks
		sprintf(archiveName, MOCK_RAD_PATH "/%s0%s",dataFile,num);
		// removes the chunks to save some space
		remove(archiveName);
	}

	// removes the dataFile to save space
	remove(dataPath);

}

//Handles the initialization of the TFTP thread -> may not need with external process manager
void RAD::TFTPLaunch(){
	char * argv[] = {"/usr/bin/tftp", "-g", "-r", dataFile, "10.14.134.207", NULL};
	tftp.launchProcess(argv);

}


