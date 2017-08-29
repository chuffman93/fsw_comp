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
	//look up dirent.h
	DIR *dp;
	struct dirent *entry;
	int count;

	//for comparing files names
	char fileID[5] = "EPS_";

	//open directory/folder
	dp  = opendir(dir.c_str());

	//checks that this is a valid directory
	if((dp  = opendir(dir.c_str())) == NULL) {
		cout << "Error opening " << dir << endl;
		return;
	}
	count= 0;
	//reads through directory
	while((entry = readdir(dp)) != NULL){
		//if file name matches EPS_, then it is a file we want to parse through, thus we add it to the vector
		if (strncmp(fileID,entry->d_name,4) == 0){
		files.push_back(entry->d_name);
		count++;
		}
	}
	closedir(dp);
}

void sortfiles(vector<string> &files){
	//sorts files in vector be name
	sort(files.begin(), files.end());
}

void parsebuffer(uint8_t *buffer, unsigned long size)
{
	//create instance of serialize to decode the buffer
	Serialize serializable(buffer, size);

	//location for txt file
	string data_file_location = "/home/fsw/FSW/cdh-fsw/GND/HST_EPS_data.txt";

	//open the txt file to append to
	FILE *writefile = fopen(data_file_location.c_str(), "a+");

	//array to hold labels for all the data
	string label[] = {"Timestamp: ", "current3v3: ", "voltage3v3: ", "currentVbat: ", "voltageVbat: ", "current12v: ", "voltage12v: ", "remainingCapacity: ", "battCurrent: ", "battVoltage: ", "battStatus: ", "frangCurrent: ", "frangVoltage: ", "convCurrentX: ", "convThreshX: ", "convCurrentY: ","convThreshY: ", "convCurrentW: ", "convThreshW: "};

	//decode timestamp
	int32_t val;
	val = serializable.deserialize_int32();

	//array large enough to hold decoded data
	char newval[20];

	//saves decoded data to newval
	sprintf(newval,"%d",val);

	//write the data label, data and newline
	fwrite(label[0].c_str(), strlen((label[0]).c_str()) , 1, writefile);
	fwrite(newval, strlen(newval), 1, writefile);
	fwrite("\n", sizeof("\n"), 1, writefile);

	int index = 1;
	int num_ts = size/40; //40 is size of each chunk of H&S data
	int ts_size = num_ts*4; //timestamp data size is 4 bytes since it is an int32, multiply it by the amount of chunks of data/ the amount of times a timestamp occurs

	//iterate through total data size minus the size of the combined timestamps, divided by two (two because the rest of the data is an int 16)
	for (int a=0;a<(size-ts_size)/2;a++){
		int16_t val;
		val = serializable.deserialize_int16();
		sprintf(newval,"%d",val);

		//write data label, data and newline
		fwrite(label[index].c_str(), strlen((label[index]).c_str()) , 1, writefile);
		fwrite(newval, strlen(newval), 1, writefile);
		fwrite("\n", sizeof("\n"), 1, writefile);
		index++;

		//if you have reached the end of the chuck of data reset the index and append the next timestamp
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
	//location for the files
	const char * directory_location = "/home/fsw/FSW/cdh-fsw/GND/";

	//location for text file to write to
	string data_file_location = "/home/fsw/FSW/cdh-fsw/GND/HST_EPS_data.txt";

	//this will clear the current file and begin file with new entry begin
	FILE *writefile = fopen(data_file_location.c_str(), "w");
	string newentrylabel = "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ NEW ENTRY BEGIN~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~";
	fwrite(newentrylabel.c_str(), strlen((newentrylabel).c_str()) , 1, writefile);
	fwrite("\n", sizeof("\n"), 1, writefile);
	fclose(writefile);

	//index through file names in the files vector
	for (int index=0; index<files.size(); ++index){
		//indicate that were writing data from the next file
		FILE *writefile = fopen(data_file_location.c_str(), "a+");
		string newentrylabel = "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ NEW FILE BEGIN~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~";
		fwrite(newentrylabel.c_str(), strlen((newentrylabel).c_str()) , 1, writefile);
		fwrite("\n", sizeof("\n"), 1, writefile);
		fclose(writefile);
		const char *loc = (directory_location + files[index]).c_str();

		//get file size and rewind
		FILE *readfile = fopen(loc, "r");
		fseek(readfile, 0, SEEK_END);
		unsigned long fSize = ftell(readfile);
		rewind(readfile);

		//create buffer for the data and parse/decode the data, then delete so there are no memory leaks
		uint8_t *buffer = new uint8_t[fSize];
		fread(buffer, 1, fSize, readfile);
		parsebuffer(buffer, fSize);
		delete buffer;
		fclose(readfile);
	}

}



int main(){
	//vector to store files names
	vector<string> files = vector<string>();

	//location of files we will want to parse through
	string dir = "/home/fsw/FSW/cdh-fsw/GND";

	//gets list of files to parse
	getHSTfileslist(dir, files);

	//sorts files
	sortfiles(files);

	//decodes files and adds them to text file
	parsefiles(files);
	return 0;

}




