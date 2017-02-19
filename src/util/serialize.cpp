#include <util/serialize.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

using namespace std;

Serialize::Serialize(uint8_t * buf, unsigned int size){
  buf_ptr = buf;
  buf_size = size;
  serialize_index = 0;
  deserialize_index = 0;
}

void Serialize::reset(void) {
  serialize_index = 0;
  deserialize_index = 0;
}

void Serialize::update(uint8_t * buf, unsigned int size) {
  reset();
  buf_ptr = buf;
  buf_size = size;
}

// ---- Deserialize ----------------------------------

int8_t Serialize::deserializeInt8(void) {
  size_t size = sizeof(int8_t);
  unsigned int max_index = deserialize_index + size;

  // Check for buffer overflow
  if (max_index > buf_size) {
    return -1;
  }

  uint8_t ret;
  ret = ( (uint8_t) (buf_ptr[deserialize_index]) ) ;

  deserialize_index += size;
  return (int8_t) ret;
}

int16_t Serialize::deserializeInt16(void) {
  size_t size = sizeof(int16_t);
  unsigned int max_index = deserialize_index + size;

  // Check for buffer overflow
  if (max_index > buf_size) {
    return -1;
  }

  uint16_t ret;
  ret =  ( (uint16_t) (buf_ptr[deserialize_index]   ) <<  8) |
         ( (uint16_t) (buf_ptr[deserialize_index+1] )      ) ;

  deserialize_index += size;
  return (int16_t) ret;
}

int32_t Serialize::deserializeInt32(void) {
  size_t size = sizeof(int32_t);
  unsigned int max_index = deserialize_index + size;

  // Check for buffer overflow
  if (max_index > buf_size) {
    return -1;
  }

  uint32_t ret;
  ret =  ( (uint32_t) (buf_ptr[deserialize_index]   ) << 24) |
         ( (uint32_t) (buf_ptr[deserialize_index+1] ) << 16) | 
         ( (uint32_t) (buf_ptr[deserialize_index+2] ) <<  8) | 
         ( (uint32_t) (buf_ptr[deserialize_index+3] )      ) ;

  deserialize_index += size;
  return (int32_t) ret;
}

int64_t Serialize::deserializeInt64(void) {
  size_t size = sizeof(int64_t);
  unsigned int max_index = deserialize_index + size;

  // Check for buffer overflow
  if (max_index > buf_size) {
    return -1;
  }

  uint64_t ret;
  ret =  ( (uint64_t) (buf_ptr[deserialize_index]   ) << 56) |
         ( (uint64_t) (buf_ptr[deserialize_index+1] ) << 48) | 
         ( (uint64_t) (buf_ptr[deserialize_index+2] ) << 40) | 
         ( (uint64_t) (buf_ptr[deserialize_index+3] ) << 32) |
         ( (uint64_t) (buf_ptr[deserialize_index+4] ) << 24) |
         ( (uint64_t) (buf_ptr[deserialize_index+5] ) << 16) | 
         ( (uint64_t) (buf_ptr[deserialize_index+6] ) <<  8) | 
         ( (uint64_t) (buf_ptr[deserialize_index+7] )      ) ;

  deserialize_index += size;
  return (int64_t) ret;
}

uint8_t Serialize::deserializeUInt8(void) {
  size_t size = sizeof(uint8_t);
  unsigned int max_index = deserialize_index + size;

  // Check for buffer overflow
  if (max_index > buf_size) {
    return -1;
  }

  uint8_t ret;
  ret = ( (uint8_t) (buf_ptr[deserialize_index]) ) ;

  deserialize_index += size;
  return (uint8_t) ret;
}

uint16_t Serialize::deserializeUInt16(void) {
  size_t size = sizeof(uint16_t);
  unsigned int max_index = deserialize_index + size;

  // Check for buffer overflow
  if (max_index > buf_size) {
    return -1;
  }

  uint16_t ret;
  ret =  ( (uint16_t) (buf_ptr[deserialize_index]   ) <<  8) |
         ( (uint16_t) (buf_ptr[deserialize_index+1] )      ) ;

  deserialize_index += size;
  return (uint16_t) ret;
}

uint32_t Serialize::deserializeUInt32(void) {
  size_t size = sizeof(uint32_t);
  unsigned int max_index = deserialize_index + size;

  // Check for buffer overflow
  if (max_index > buf_size) {
    return -1;
  }

  uint32_t ret;
  ret =  ( (uint32_t) (buf_ptr[deserialize_index]   ) << 24) |
         ( (uint32_t) (buf_ptr[deserialize_index+1] ) << 16) |
         ( (uint32_t) (buf_ptr[deserialize_index+2] ) <<  8) |
         ( (uint32_t) (buf_ptr[deserialize_index+3] )      ) ;

  deserialize_index += size;
  return (uint32_t) ret;
}

uint64_t Serialize::deserializeUInt64(void) {
  size_t size = sizeof(uint64_t);
  unsigned int max_index = deserialize_index + size;

  // Check for buffer overflow
  if (max_index > buf_size) {
    return -1;
  }

  uint64_t ret;
  ret =  ( (uint64_t) (buf_ptr[deserialize_index]   ) << 56) |
         ( (uint64_t) (buf_ptr[deserialize_index+1] ) << 48) |
         ( (uint64_t) (buf_ptr[deserialize_index+2] ) << 40) |
         ( (uint64_t) (buf_ptr[deserialize_index+3] ) << 32) |
         ( (uint64_t) (buf_ptr[deserialize_index+4] ) << 24) |
         ( (uint64_t) (buf_ptr[deserialize_index+5] ) << 16) |
         ( (uint64_t) (buf_ptr[deserialize_index+6] ) <<  8) |
         ( (uint64_t) (buf_ptr[deserialize_index+7] )      ) ;

  deserialize_index += size;
  return (uint64_t) ret;
}

bool Serialize::deserializeBool(void) {
  size_t size = sizeof(bool);
  unsigned int max_index = deserialize_index + size;

  // Check for buffer overflow
  if (max_index > buf_size) {
    return -1;
  }

  uint8_t ret;
  ret = (uint8_t) (buf_ptr[deserialize_index]);

  deserialize_index += size;
  return *((bool *) &ret);
}

float Serialize::deserializeFloat(void) {
  size_t size = sizeof(float);
  unsigned int max_index = deserialize_index + size;

  // Check for buffer overflow
  if (max_index > buf_size) {
    return -1;
  }

  uint32_t ret;
  ret =  ( (uint32_t) (buf_ptr[deserialize_index]   ) << 24) |
         ( (uint32_t) (buf_ptr[deserialize_index+1] ) << 16) | 
         ( (uint32_t) (buf_ptr[deserialize_index+2] ) <<  8) | 
         ( (uint32_t) (buf_ptr[deserialize_index+3] )      ) ;

  deserialize_index += size;
  return *((float *) &ret);
}

double Serialize::deserializeDouble(void) {
  size_t size = sizeof(double);
  unsigned int max_index = deserialize_index + size;

  // Check for buffer overflow
  if (max_index > buf_size) {
    return -1;
  }

  uint64_t ret;
  ret =  ( (uint64_t) (buf_ptr[deserialize_index]   ) << 56) |
         ( (uint64_t) (buf_ptr[deserialize_index+1] ) << 48) |
         ( (uint64_t) (buf_ptr[deserialize_index+2] ) << 40) |
         ( (uint64_t) (buf_ptr[deserialize_index+3] ) << 32) |
         ( (uint64_t) (buf_ptr[deserialize_index+4] ) << 24) |
         ( (uint64_t) (buf_ptr[deserialize_index+5] ) << 16) |
         ( (uint64_t) (buf_ptr[deserialize_index+6] ) <<  8) |
         ( (uint64_t) (buf_ptr[deserialize_index+7] )      ) ;

  deserialize_index += size;
  return *((double *) &ret);
}

// ---- Serialize ----------------------------------

int Serialize::serializeInt8(int8_t data) {
  size_t size = sizeof(int8_t);
  unsigned int max_index = serialize_index + size;

  // Check for buffer overflow
  if (max_index > buf_size) {
    return -1;
  }

  buf_ptr[serialize_index] = (data & 0xFF);
  serialize_index++;

  return 1;
}

int Serialize::serializeInt16(int16_t data) {
  size_t size = sizeof(int16_t);
  unsigned int max_index = serialize_index + size;

  // Check for buffer overflow
  if (max_index > buf_size) {
    return -1;
  }

  buf_ptr[serialize_index] = (data & 0xFF00) >> 8;
  serialize_index++;

  buf_ptr[serialize_index] = (data & 0x00FF);
  serialize_index++;

  return 1;
}

int Serialize::serializeInt32(int32_t data) {
  size_t size = sizeof(int32_t);
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

int Serialize::serializeInt64(int64_t data) {
  size_t size = sizeof(int64_t);
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

int Serialize::serializeUInt8(uint8_t data) {
  size_t size = sizeof(uint8_t);
  unsigned int max_index = serialize_index + size;

  // Check for buffer overflow
  if (max_index > buf_size) {
    return -1;
  }

  buf_ptr[serialize_index] = (data & 0xFF);
  serialize_index++;

  return 1;
}

int Serialize::serializeUInt16(uint16_t data) {
  size_t size = sizeof(uint16_t);
  unsigned int max_index = serialize_index + size;

  // Check for buffer overflow
  if (max_index > buf_size) {
    return -1;
  }

  buf_ptr[serialize_index] = (data & 0xFF00) >> 8;
  serialize_index++;

  buf_ptr[serialize_index] = (data & 0x00FF);
  serialize_index++;

  return 1;
}

int Serialize::serializeUInt32(uint32_t data) {
  size_t size = sizeof(uint32_t);
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

int Serialize::serializeUInt64(uint64_t data) {
  size_t size = sizeof(uint64_t);
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

int Serialize::serializeBool(bool bdata) {
  uint8_t data = *((uint8_t*)&bdata);
  size_t size = sizeof(bool);
  unsigned int max_index = serialize_index + size;

  // Check for buffer overflow
  if (max_index >= buf_size) {
    return -1;
  }

  // Serialize the data into the buf
  buf_ptr[serialize_index] = (data & 0xFF);
  serialize_index++;

  return 1;

}

int Serialize::serializeFloat(float fdata) {
  uint32_t data = *((uint32_t*)&fdata);
  size_t size = sizeof(float);
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

int Serialize::serializeDouble(double ddata) {
  uint64_t data = *((uint64_t*)&ddata);
  size_t size = sizeof(double);
  unsigned int max_index = serialize_index + size;

  // Check for buffer overflow
  if (max_index >= buf_size) {
    return -1;
  }

  // Serialize the data into the buf
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
