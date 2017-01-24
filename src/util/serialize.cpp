#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <iostream>
#include <fstream>

using namespace std;

class Serialize {

public:

Serialize(uint8_t * buf, unsigned int size){
  buf_ptr = buf;
  buf_size = size;
  serialize_index = 0;
  deserialize_index = 0;
}

int reset() {
  serialize_index = 0;
  deserialize_index = 0;
}

int update(uint8_t * buf, unsigned int size) {
  reset();
  buf_ptr = buf;
  buf_size = size;
}

int32_t deserializeInt32() {
  int size = sizeof(int32_t);
  unsigned int max_index = deserialize_index + size;

  // Check for buffer overflow
  if (max_index >= buf_size) {
    return -1;
  }

  uint32_t ret;
  ret =  (uint32_t) (buf_ptr[deserialize_index]   << 24) |
         (uint32_t) (buf_ptr[deserialize_index+1] << 16) | 
         (uint32_t) (buf_ptr[deserialize_index+2] <<  8) | 
         (uint32_t) (buf_ptr[deserialize_index+3]      ) ;

  deserialize_index += size;
  return (int32_t) ret;

}

int64_t deserializeInt64() {
  int size = sizeof(int64_t);
  unsigned int max_index = deserialize_index + size;

  // Check for buffer overflow
  if (max_index >= buf_size) {
    return -1;
  }

  uint64_t ret;
  ret =  (uint64_t) (buf_ptr[deserialize_index]   << 56) |
         (uint64_t) (buf_ptr[deserialize_index+1] << 48) | 
         (uint64_t) (buf_ptr[deserialize_index+2] << 40) | 
         (uint64_t) (buf_ptr[deserialize_index+3] << 32) |
         (uint64_t) (buf_ptr[deserialize_index+4] << 24) |
         (uint64_t) (buf_ptr[deserialize_index+5] << 16) | 
         (uint64_t) (buf_ptr[deserialize_index+6] <<  8) | 
         (uint64_t) (buf_ptr[deserialize_index+7]      ) ;

  deserialize_index += size;
  return (int64_t) ret;
}

float deserializeFloat() {
  int size = sizeof(float);
  unsigned int max_index = deserialize_index + size;

  // Check for buffer overflow
  if (max_index > buf_size) {
    return -1;
  }

  uint32_t ret;
  ret =  (uint32_t) (buf_ptr[deserialize_index]   << 24) |
         (uint32_t) (buf_ptr[deserialize_index+1] << 16) | 
         (uint32_t) (buf_ptr[deserialize_index+2] <<  8) | 
         (uint32_t) (buf_ptr[deserialize_index+3]      ) ;

  deserialize_index += size;
  return *((float *) &ret);
}

int serializeInt32(int32_t data) {
  int size = sizeof(int32_t);
  unsigned int max_index = serialize_index + size;

  // Check for buffer overflow
  if (max_index > buf_size) {
    return -1;
  }

  buf_ptr[serialize_index] = (data & 0xFF000000) >> 24;
  serialize_index++;

  buf_ptr[serialize_index] = (data & 0x00FF0000) >> 16;
  serialize_index++;

  buf_ptr[serialize_index] = (data & 0x0000FF00) >> 8;
  serialize_index++;

  buf_ptr[serialize_index] = (data & 0x000000FF);
  serialize_index++;

  return 1;
}

int serializeInt64(int64_t data) {
  int size = sizeof(int64_t);
  unsigned int max_index = serialize_index + size;

  // Check for buffer overflow
  if (max_index >= buf_size) {
    return -1;
  }

  buf_ptr[serialize_index] = (data & 0xFF00000000000000) >> 56;
  serialize_index++;

  buf_ptr[serialize_index] = (data & 0x00FF000000000000) >> 48;
  serialize_index++;

  buf_ptr[serialize_index] = (data & 0x0000FF0000000000) >> 40;
  serialize_index++;

  buf_ptr[serialize_index] = (data & 0x000000FF00000000) >> 32;
  serialize_index++;

  buf_ptr[serialize_index] = (data & 0x00000000FF000000) >> 24;
  serialize_index++;

  buf_ptr[serialize_index] = (data & 0x0000000000FF0000) >> 16;
  serialize_index++;

  buf_ptr[serialize_index] = (data & 0x000000000000FF00) >> 8;
  serialize_index++;

  buf_ptr[serialize_index] = (data & 0x00000000000000FF);
  serialize_index++;

  return 1;
}

int serializeFloat(float fdata) {
  uint32_t data = *((uint32_t*)&fdata);
  int size = sizeof(float);
  unsigned int max_index = serialize_index + size;

  // Check for buffer overflow
  if (max_index >= buf_size) {
    return -1;
  }

  // Serialize the data into the buf
  buf_ptr[serialize_index] = (data & 0xFF000000) >> 24;
  serialize_index++;

  buf_ptr[serialize_index] = (data & 0x00FF0000) >> 16;
  serialize_index++;

  buf_ptr[serialize_index] = (data & 0x0000FF00) >> 8;
  serialize_index++;

  buf_ptr[serialize_index] = (data & 0x000000FF);
  serialize_index++;

  return 1;
}


//private:

uint8_t * buf_ptr;
unsigned int buf_size;
unsigned int serialize_index;
unsigned int deserialize_index;

};


int main() {
  int32_t myint32 = 10453;
  int buf_size = 10;
  uint8_t buf[buf_size];
  Serialize mySer(buf, buf_size);
  if (mySer.serializeInt32(myint32) == -1) {
    printf("issue32\n");
  }
  int ret = mySer.deserializeInt32();
  printf("int32: %d\n", ret);
  cout << "sindex: " << mySer.serialize_index << " dindex " << mySer.deserialize_index << endl;

  //mySer.reset();
  int buf2_size = 50;
  uint8_t buf2[buf2_size];
  mySer.update(buf2, buf2_size);

  cout << "sindex: " << mySer.serialize_index << " dindex " << mySer.deserialize_index << endl;



  int64_t myint64 = 30111;
  if (mySer.serializeInt64(myint64) == -1) {
    printf("issue64\n");
  }
  printf("int64: %ld\n", mySer.deserializeInt64());

  cout << "sindex: " << mySer.serialize_index << " dindex " << mySer.deserialize_index << endl;

  float myfloat = 2.1;
  if (mySer.serializeFloat(myfloat) == -1) {
    printf("issueFloat\n");
  }
  printf("float: %f\n", mySer.deserializeFloat());

  cout << "sindex: " << mySer.serialize_index << " dindex " << mySer.deserialize_index << endl;

  return 0;
}


// private member var that is char *
// index with another private variable

// check that buffer iusnt full
// use const
