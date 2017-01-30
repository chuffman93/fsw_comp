#include "util/serialize.h"
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <string>

using namespace std;


void read_buf_from_file(uint8_t * buf, size_t buf_size, string filename){
  ifstream in(filename.c_str(), ios::in | ios::binary );
  in.read((char *)buf, buf_size);
  in.close();
}


int main() {

  string filename;
  cout << "Enter the name of the file you wish to create: ";
  cin >> filename;







  //string filename = "test.bin";

  size_t buf_size = 100;
  uint8_t buf[buf_size];
  Serialize serializable(buf, buf_size);

  int testint = 50;

  serializable.serializeInt32(testint);

  ofstream fp;
  fp.open(filename.c_str(), ios::out | ios::binary);
  fp.write((char *) buf, buf_size);
  fp.close();

  uint8_t file_buffer[buf_size];

  read_buf_from_file(file_buffer, buf_size, filename);

  serializable.update(buf, buf_size);
  int out = serializable.deserializeInt32();

  cout << out << endl;

  return 0;
}
