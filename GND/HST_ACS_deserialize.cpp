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
	char fileID[5] = "ACS_";
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
	string data_file_location = "/home/fsw/FSW/cdh-fsw/GND/HST_ACS_data.txt";
	FILE *writefile = fopen(data_file_location.c_str(), "a+");
	string label[] = {"Timestamp: ", "Mode:", "st_mrp: ", "curr_mrp: ", "ref_mrp: ", "time_since_lock: ", "gyro0: ", "gyro1: ", "ang_rate: ", "mag0: ", "mag1: ", "rw_torque: ", "rw_pwn: ", "rw_speed: ", "tr_torque: ", "tr_pwn: "};
	cout << size << endl;
	//write data
	int index = 2;
	int num_ts = size/165;
	int ts_size = num_ts*5;
	for (int a=0;a<num_ts;a++)
	{
		fwrite("\n", sizeof("\n"), 1, writefile);
		//write first timestamp
		uint32_t t;
		t = serializable.deserialize_uint32();
		char newval[20];
		sprintf(newval,"%d",t);
		fwrite(label[0].c_str(), strlen((label[0]).c_str()) , 1, writefile);
		fwrite(newval, strlen(newval), 1, writefile);
		fwrite("\n", sizeof("\n"), 1, writefile);
		//write first mode
		uint8_t val;
		val = serializable.deserialize_uint8();
		sprintf(newval,"%d",val);
		fwrite(label[1].c_str(), strlen((label[1]).c_str()) , 1, writefile);
		fwrite(newval, strlen(newval), 1, writefile);
		fwrite("\n", sizeof("\n"), 1, writefile);
		for (int b=0;b<14;b++){
			int32_t val;
			fwrite(label[index].c_str(), strlen((label[index]).c_str()) , 1, writefile);
			for (int c=0;c<3;c++){
				val = serializable.deserialize_float();
				sprintf(newval,"%d ",val);
				fwrite(newval, strlen(newval), 1, writefile);
				if (label[index]=="time_since_lock: "){
					break;
				}
			}
			index++;
			if (index == 16){
				index = 2;
			}
			fwrite("\n", sizeof("\n"), 1, writefile);
			}
		}

	fwrite("\n", sizeof("\n"), 1, writefile);
	fclose(writefile);
	cout << endl << endl;
}

void parsefiles(vector<string> &files){
	const char * directory_location = "/home/fsw/FSW/cdh-fsw/GND/";
	string data_file_location = "/home/fsw/FSW/cdh-fsw/GND/HST_ACS_data.txt";
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




