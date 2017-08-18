#include "util/serialize.h"
#include "servers/Structs.h"
#include <sys/types.h>
#include <unistd.h>
#include <dirent.h>
#include <vector>
#include <string.h>
#include <string>
#include <iostream>
#include <fstream>
#include <typeinfo>
#include <stdlib.h>
#include <stdio.h>
#include <algorithm>
#include <sstream>

using namespace std;


void getHSTfileslist(string dir, vector<string> &files){
	DIR *dp;
	struct dirent *entry;
	int count;
	char fileID[5] = "EPS_";
	dp  = opendir(dir.c_str());
	if((dp  = opendir(dir.c_str())) == NULL) {
		cout << "Error opening " << dir << endl;
		return;
	}
	count= 0;
	while((entry = readdir(dp)) != NULL){
		if (strncmp(fileID,entry->d_name,4) == 0){
		files.push_back(entry->d_name);
		count++;
		}
	}
	closedir(dp);
}

void sortfiles(vector<string> &files){
	sort(files.begin(), files.end());
}

void parsebuffer(uint8_t *buffer, unsigned long size)
{
	Serialize serializable(buffer, size);
	string data_file_location = "/home/fsw/FSW/cdh-fsw/GND/HST_EPS_data.txt";
	FILE *writefile = fopen(data_file_location.c_str(), "a+");
	string label[] = {"Timestamp: ", "current3v3: ", "voltage3v3: ", "currentVbat: ", "voltageVbat: ", "current12v: ", "voltage12v: ", "remainingCapacity: ", "battCurrent: ", "battVoltage: ", "battStatus: ", "frangCurrent: ", "frangVoltage: ", "convCurrentX: ", "convThreshX: ", "convCurrentY: ","convThreshY: ", "convCurrentW: ", "convThreshW: "};

	uint32_t val;
	val = serializable.deserialize_int32();
	char newval[20];
	sprintf(newval,"%d",val);
	fwrite(label[0].c_str(), strlen((label[0]).c_str()) , 1, writefile);
	fwrite(newval, strlen(newval), 1, writefile);
	fwrite("\n", sizeof("\n"), 1, writefile);

	int index = 1;
	int num_ts = size/40;
	int ts_size = num_ts*4;
	for (int a=0;a<(size-ts_size)/2;a++){
		int16_t val;
		val = serializable.deserialize_int16();
		sprintf(newval,"%d",val);
		fwrite(label[index].c_str(), strlen((label[index]).c_str()) , 1, writefile);
		fwrite(newval, strlen(newval), 1, writefile);
		fwrite("\n", sizeof("\n"), 1, writefile);
		index++;
		if (index == 19 && a!=((size-ts_size)/2)-1){
			index = 1;
			val = serializable.deserialize_int32();
			char newval[20];
			sprintf(newval,"%d",val);
			fwrite(label[0].c_str(), strlen((label[0]).c_str()) , 1, writefile);
			fwrite(newval, strlen(newval), 1, writefile);
			fwrite("\n", sizeof("\n"), 1, writefile);
		}

	}
	fwrite("\n", sizeof("\n"), 1, writefile);
	fclose(writefile);
	cout << endl << endl;
}

void parsefiles(vector<string> &files){
	const char * directory_location = "/home/fsw/FSW/cdh-fsw/GND/";
	string data_file_location = "/home/fsw/FSW/cdh-fsw/GND/HST_EPS_data.txt";
	FILE *writefile = fopen(data_file_location.c_str(), "w");
	string newentrylabel = "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ NEW ENTRY BEGIN~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~";
	fwrite(newentrylabel.c_str(), strlen((newentrylabel).c_str()) , 1, writefile);
	fwrite("\n", sizeof("\n"), 1, writefile);
	fclose(writefile);
	for (int index=0; index<files.size(); ++index){
		FILE *writefile = fopen(data_file_location.c_str(), "a+");
		string newentrylabel = "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ NEW FILE BEGIN~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~";
		fwrite(newentrylabel.c_str(), strlen((newentrylabel).c_str()) , 1, writefile);
		fwrite("\n", sizeof("\n"), 1, writefile);
		fclose(writefile);
		const char *loc = (directory_location + files[index]).c_str();
		FILE *readfile = fopen(loc, "r");
		fseek(readfile, 0, SEEK_END);
		unsigned long fSize = ftell(readfile);
		rewind(readfile);
		//cout << "Adding: " << loc  << " with file size :" << fSize << endl;
		uint8_t *buffer = new uint8_t[fSize];
		fread(buffer, 1, fSize, readfile);
		parsebuffer(buffer, fSize);
		delete buffer;
		fclose(readfile);
	}

}



int main(){
	vector<string> files = vector<string>();
	string dir = "/home/fsw/FSW/cdh-fsw/GND";
	getHSTfileslist(dir, files);
	sortfiles(files);
	parsefiles(files);
	return 0;

}




