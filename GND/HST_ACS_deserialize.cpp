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
	//research dirent.h for finding file names in folder
	DIR *dp;
	struct dirent *entry;
	int count;

	//char array to compare file names
	char fileID[5] = "ACS_";

	//open input directory
	dp  = opendir(dir.c_str());

	//return if unable to open
	if((dp  = opendir(dir.c_str())) == NULL) {
		cout << "Error opening " << dir << endl;
		return;
	}
	count = 0;

	//add file to files vector if it begins with ACS_
	while((entry = readdir(dp)) != NULL){
		if (strncmp(fileID,entry->d_name,4) == 0){
		files.push_back(entry->d_name);
		count++;
		}
	}
	closedir(dp);
}

void sortfiles(vector<string> &files){
	//sorts files by name
	sort(files.begin(), files.end());
}

void parsebuffer(uint8_t *buffer, unsigned long size)
{
	//uses serialize file for decoding
	Serialize serializable(buffer, size);
	string data_file_location = "/home/fsw/FSW/cdh-fsw/GND/HST_ACS_data.txt";
	FILE *writefile = fopen(data_file_location.c_str(), "a+");

	//array for data labels
	string label[] = {"Timestamp: ", "Mode:", "st_mrp: ", "curr_mrp: ", "ref_mrp: ", "time_since_lock: ", "gyro0: ", "gyro1: ", "ang_rate: ", "mag0: ", "mag1: ", "rw_torque: ", "rw_pwn: ", "rw_speed: ", "tr_torque: ", "tr_pwn: "};

	int index = 2; //start with index to so we start with st_mrp in for loop
	int num_ts = size/165; //165 is size of one chunk of data
	int ts_size = num_ts*5; // 5 is size of timestamp and mode since they are int32 and int8
	//go through number of chucks of data
	for (int a=0;a<num_ts;a++)
	{
		fwrite("\n", sizeof("\n"), 1, writefile);
		//write first timestamp
		int32_t t;
		t = serializable.deserialize_int32();
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

		//go through the remaining data
		for (int b=0;b<14;b++){
			int32_t val;
			fwrite(label[index].c_str(), strlen((label[index]).c_str()) , 1, writefile);

			//all data after mode has 3 values besides time since lock
			for (int c=0;c<3;c++){
				val = serializable.deserialize_float();
				sprintf(newval,"%d ",val);
				fwrite(newval, strlen(newval), 1, writefile);
				if (label[index]=="time_since_lock: "){
					break;
				}
			}
			index++;
			//if we have gone through it all reset the index
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
	//file locations
	const char * directory_location = "/home/fsw/FSW/cdh-fsw/GND/";
	string data_file_location = "/home/fsw/FSW/cdh-fsw/GND/HST_ACS_data.txt";

	//here we are clearing the file and rewriting it
	FILE *writefile = fopen(data_file_location.c_str(), "w");
	string newentrylabel = "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ NEW ENTRY BEGIN~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~";
	fwrite(newentrylabel.c_str(), strlen((newentrylabel).c_str()) , 1, writefile);
	fwrite("\n", sizeof("\n"), 1, writefile);
	fclose(writefile);

	//iterate through number of files in the vector to pase through them
	for (int index=0; index<files.size(); ++index){
		//indicates that we are writing from a new file
		FILE *writefile = fopen(data_file_location.c_str(), "a+");
		string newentrylabel = "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ NEW FILE BEGIN~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~";
		fwrite(newentrylabel.c_str(), strlen((newentrylabel).c_str()) , 1, writefile);
		fwrite("\n", sizeof("\n"), 1, writefile);
		fclose(writefile);
		const char *loc = (directory_location + files[index]).c_str();

		//get size of file we are reading from and rewind
		FILE *readfile = fopen(loc, "r");
		fseek(readfile, 0, SEEK_END);
		unsigned long fSize = ftell(readfile);
		rewind(readfile);

		//create buffer for data, decode data and delete after so there are no memory leaks
		uint8_t *buffer = new uint8_t[fSize];
		fread(buffer, 1, fSize, readfile);
		parsebuffer(buffer, fSize);
		delete buffer;
		fclose(readfile);
	}

}



int main(){
	//create vector to hold all the file names
	vector<string> files = vector<string>();
	//file(s) location
	string dir = "/home/fsw/FSW/cdh-fsw/GND";

	//get file names
	getHSTfileslist(dir, files);

	//sorts the files
	sortfiles(files);

	//decodes the files
	parsefiles(files);
	return 0;

}




