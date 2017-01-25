#include "gtest/gtest.h"
#include "util/serialize.h"
#include <stdint.h>
#include <float.h>

TEST(TestSerialize, testInt8) {
  unsigned int err;
  int8_t test_val;
  int8_t deser_val;
  unsigned int buf_size = 50;
  uint8_t buf[buf_size];

  Serialize testSer(buf, buf_size); 

  // Test a value in range
  test_val = 1;
  err = testSer.serializeInt8(test_val);
  ASSERT_EQ(err, 1);
  deser_val = testSer.deserializeInt8();
  ASSERT_EQ(deser_val, test_val);


  // Test maximum value
  test_val = INT8_MAX;
  err = testSer.serializeInt8(test_val);
  ASSERT_EQ(err, 1);
  deser_val = testSer.deserializeInt8();
  ASSERT_EQ(deser_val, test_val);

  // Test minimum value
  test_val = INT_LEAST8_MIN;
  err = testSer.serializeInt8(test_val);
  ASSERT_EQ(err, 1);
  deser_val = testSer.deserializeInt8();
  ASSERT_EQ(deser_val, test_val);
  
  // Test serialize above range
  test_val = INT8_MAX+1;
  err = testSer.serializeInt8(test_val);
  ASSERT_EQ(err, -1);

  // Test serialize below range
  test_val = INT_LEAST8_MIN-1;
  err = testSer.serializeInt8(test_val);
  ASSERT_EQ(err, -1);
}

TEST(TestSerialize, testInt16) {
  unsigned int err;
  int16_t test_val;
  int16_t deser_val;
  unsigned int buf_size = 50;
  uint8_t buf[buf_size];

  Serialize testSer(buf, buf_size); 

  // Test a value in range
  test_val = 1;
  err = testSer.serializeInt16(test_val);
  ASSERT_EQ(err, 1);
  deser_val = testSer.deserializeInt16();
  ASSERT_EQ(deser_val, test_val);


  // Test maximum value
  test_val = INT16_MAX;
  err = testSer.serializeInt16(test_val);
  ASSERT_EQ(err, 1);
  deser_val = testSer.deserializeInt16();
  ASSERT_EQ(deser_val, test_val);

  // Test minimum value
  test_val = INT_LEAST16_MIN;
  err = testSer.serializeInt16(test_val);
  ASSERT_EQ(err, 1);
  deser_val = testSer.deserializeInt16();
  ASSERT_EQ(deser_val, test_val);
  
  // Test serialize above range
  test_val = INT16_MAX+1;
  err = testSer.serializeInt16(test_val);
  ASSERT_EQ(err, -1);

  // Test serialize below range
  test_val = INT_LEAST16_MIN-1;
  err = testSer.serializeInt16(test_val);
  ASSERT_EQ(err, -1);
}

TEST(TestSerialize, testInt32) {
  unsigned int err;
  int32_t test_val;
  int32_t deser_val;
  unsigned int buf_size = 50;
  uint8_t buf[buf_size];

  Serialize testSer(buf, buf_size); 

  // Test a value in range
  test_val = 1;
  err = testSer.serializeInt32(test_val);
  ASSERT_EQ(err, 1);
  deser_val = testSer.deserializeInt32();
  ASSERT_EQ(deser_val, test_val);


  // Test maximum value
  test_val = INT32_MAX;
  err = testSer.serializeInt32(test_val);
  ASSERT_EQ(err, 1);
  deser_val = testSer.deserializeInt32();
  ASSERT_EQ(deser_val, test_val);

  // Test minimum value
  test_val = INT_LEAST32_MIN;
  err = testSer.serializeInt32(test_val);
  ASSERT_EQ(err, 1);
  deser_val = testSer.deserializeInt32();
  ASSERT_EQ(deser_val, test_val);
  
  // Test serialize above range
  test_val = INT32_MAX+1;
  err = testSer.serializeInt32(test_val);
  ASSERT_EQ(err, -1);

  // Test serialize below range
  test_val = INT_LEAST32_MIN-1;
  err = testSer.serializeInt32(test_val);
  ASSERT_EQ(err, -1);
}

TEST(TestSerialize, testInt64) {
  unsigned int err;
  int64_t test_val;
  int64_t deser_val;
  unsigned int buf_size = 50;
  uint8_t buf[buf_size];

  Serialize testSer(buf, buf_size); 

  // Test a value in range
  test_val = 1;
  err = testSer.serializeInt64(test_val);
  ASSERT_EQ(err, 1);
  deser_val = testSer.deserializeInt64();
  ASSERT_EQ(deser_val, test_val);


  // Test maximum value
  test_val = INT64_MAX;
  err = testSer.serializeInt64(test_val);
  ASSERT_EQ(err, 1);
  deser_val = testSer.deserializeInt64();
  ASSERT_EQ(deser_val, test_val);

  // Test minimum value
  test_val = INT_LEAST64_MIN;
  err = testSer.serializeInt64(test_val);
  ASSERT_EQ(err, 1);
  deser_val = testSer.deserializeInt64();
  ASSERT_EQ(deser_val, test_val);
  
  // Test serialize above range
  test_val = INT64_MAX+1;
  err = testSer.serializeInt64(test_val);
  ASSERT_EQ(err, -1);

  // Test serialize below range
  test_val = INT_LEAST64_MIN-1;
  err = testSer.serializeInt32(test_val);
  ASSERT_EQ(err, -1);
}

TEST(TestSerialize, testUInt8) {
  unsigned int err;
  uint8_t test_val;
  uint8_t deser_val;
  unsigned int buf_size = 50;
  uint8_t buf[buf_size];

  Serialize testSer(buf, buf_size); 

  // Test a value in range
  test_val = 1;
  err = testSer.serializeUInt8(test_val);
  ASSERT_EQ(err, 1);
  deser_val = testSer.deserializeUInt8();
  ASSERT_EQ(deser_val, test_val);


  // Test maximum value
  test_val = UINT8_MAX;
  err = testSer.serializeUInt8(test_val);
  ASSERT_EQ(err, 1);
  deser_val = testSer.deserializeUInt8();
  ASSERT_EQ(deser_val, test_val);

  // Test minimum value
  test_val = 0;
  err = testSer.serializeUInt8(test_val);
  ASSERT_EQ(err, 1);
  deser_val = testSer.deserializeUInt8();
  ASSERT_EQ(deser_val, test_val);
  
  // Test serialize above range
  test_val = UINT8_MAX+1;
  err = testSer.serializeUInt8(test_val);
  ASSERT_EQ(err, -1);

  // Test serialize below range
  test_val = -1; // TODO: Does this even work?
  err = testSer.serializeUInt8(test_val);
  ASSERT_EQ(err, -1);
}

TEST(TestSerialize, testUInt16) {
  unsigned int err;
  uint16_t test_val;
  uint16_t deser_val;
  unsigned int buf_size = 50;
  uint8_t buf[buf_size];

  Serialize testSer(buf, buf_size); 

  // Test a value in range
  test_val = 1;
  err = testSer.serializeUInt16(test_val);
  ASSERT_EQ(err, 1);
  deser_val = testSer.deserializeUInt16();
  ASSERT_EQ(deser_val, test_val);


  // Test maximum value
  test_val = UINT16_MAX;
  err = testSer.serializeUInt16(test_val);
  ASSERT_EQ(err, 1);
  deser_val = testSer.deserializeUInt16();
  ASSERT_EQ(deser_val, test_val);

  // Test minimum value
  test_val = 0;
  err = testSer.serializeUInt16(test_val);
  ASSERT_EQ(err, 1);
  deser_val = testSer.deserializeUInt16();
  ASSERT_EQ(deser_val, test_val);
  
  // Test serialize above range
  test_val = UINT16_MAX+1;
  err = testSer.serializeUInt16(test_val);
  ASSERT_EQ(err, -1);

  // Test serialize below range
  test_val = -1; // TODO: Does this even work?
  err = testSer.serializeUInt16(test_val);
  ASSERT_EQ(err, -1);
}

TEST(TestSerialize, testUInt32) {
  unsigned int err;
  uint32_t test_val;
  uint32_t deser_val;
  unsigned int buf_size = 50;
  uint8_t buf[buf_size];

  Serialize testSer(buf, buf_size); 

  // Test a value in range
  test_val = 1;
  err = testSer.serializeUInt32(test_val);
  ASSERT_EQ(err, 1);
  deser_val = testSer.deserializeUInt32();
  ASSERT_EQ(deser_val, test_val);


  // Test maximum value
  test_val = UINT32_MAX;
  err = testSer.serializeUInt32(test_val);
  ASSERT_EQ(err, 1);
  deser_val = testSer.deserializeUInt32();
  ASSERT_EQ(deser_val, test_val);

  // Test minimum value
  test_val = 0;
  err = testSer.serializeUInt32(test_val);
  ASSERT_EQ(err, 1);
  deser_val = testSer.deserializeUInt32();
  ASSERT_EQ(deser_val, test_val);
  
  // Test serialize above range
  test_val = UINT32_MAX+1;
  err = testSer.serializeUInt32(test_val);
  ASSERT_EQ(err, -1);

  // Test serialize below range
  test_val = -1;
  err = testSer.serializeUInt32(test_val);
  ASSERT_EQ(err, -1);
}

TEST(TestSerialize, testUInt64) {
  unsigned int err;
  uint64_t test_val;
  uint64_t deser_val;
  unsigned int buf_size = 50;
  uint8_t buf[buf_size];

  Serialize testSer(buf, buf_size); 

  // Test a value in range
  test_val = 1;
  err = testSer.serializeUInt64(test_val);
  ASSERT_EQ(err, 1);
  deser_val = testSer.deserializeUInt64();
  ASSERT_EQ(deser_val, test_val);


  // Test maximum value
  test_val = UINT64_MAX;
  err = testSer.serializeUInt64(test_val);
  ASSERT_EQ(err, 1);
  deser_val = testSer.deserializeUInt64();
  ASSERT_EQ(deser_val, test_val);

  // Test minimum value
  test_val = 0;
  err = testSer.serializeUInt64(test_val);
  ASSERT_EQ(err, 1);
  deser_val = testSer.deserializeUInt64();
  ASSERT_EQ(deser_val, test_val);
  
  // Test serialize above range
  test_val = UINT64_MAX+1;
  err = testSer.serializeUInt64(test_val);
  ASSERT_EQ(err, -1);

  // Test serialize below range
  test_val = -1;
  err = testSer.serializeUInt64(test_val);
  ASSERT_EQ(err, -1);
}

TEST(TestSerialize, testBool) {
  unsigned int err;
  bool test_val;
  bool deser_val;
  unsigned int buf_size = 50;
  uint8_t buf[buf_size];

  Serialize testSer(buf, buf_size); 

  // Test true
  test_val = true;
  err = testSer.serializeInt8(test_val);
  ASSERT_EQ(err, 1);
  deser_val = testSer.deserializeInt8();
  ASSERT_EQ(deser_val, test_val);

  // Test false
  test_val = false;
  err = testSer.serializeInt8(test_val);
  ASSERT_EQ(err, 1);
  deser_val = testSer.deserializeInt8();
  ASSERT_EQ(deser_val, test_val);

  // Test serialize above range
  test_val = 2;
  err = testSer.serializeInt8(test_val);
  ASSERT_EQ(err, -1);

  // Test serialize below range
  test_val = -1;
  err = testSer.serializeInt8(test_val);
  ASSERT_EQ(err, -1);
}

TEST(TestSerialize, testFloat) {
  unsigned int err;
  float test_val;
  float deser_val;
  unsigned int buf_size = 50;
  uint8_t buf[buf_size];

  Serialize testSer(buf, buf_size); 

  // Test a value in range
  test_val = 1.2;
  err = testSer.serializeFloat(test_val);
  ASSERT_EQ(err, 1);
  deser_val = testSer.deserializeFloat();
  ASSERT_EQ(deser_val, test_val);


  // Test maximum value
  test_val = FLT_MAX;
  err = testSer.serializeFloat(test_val);
  ASSERT_EQ(err, 1);
  deser_val = testSer.deserializeFloat();
  ASSERT_EQ(deser_val, test_val);

  // Test minimum value
  test_val = FLT_MIN;
  err = testSer.serializeFloat(test_val);
  ASSERT_EQ(err, 1);
  deser_val = testSer.deserializeFloat();
  ASSERT_EQ(deser_val, test_val);
  
  // Test serialize above range
  test_val = FLT_MAX+1;
  err = testSer.serializeFloat(test_val);
  ASSERT_EQ(err, -1);

  // Test serialize below range
  test_val = FLT_MIN-1;
  err = testSer.serializeFloat(test_val);
  ASSERT_EQ(err, -1);
}

TEST(TestSerialize, testDouble) {
  unsigned int err;
  double test_val;
  double deser_val;
  unsigned int buf_size = 50;
  uint8_t buf[buf_size];

  Serialize testSer(buf, buf_size); 

  // Test a value in range
  test_val = 1.2;
  err = testSer.serializeDouble(test_val);
  ASSERT_EQ(err, 1);
  deser_val = testSer.deserializeDouble();
  ASSERT_EQ(deser_val, test_val);


  // Test maximum value
  test_val = DBL_MAX;
  err = testSer.serializeDouble(test_val);
  ASSERT_EQ(err, 1);
  deser_val = testSer.deserializeDouble();
  ASSERT_EQ(deser_val, test_val);

  // Test minimum value
  test_val = DBL_MIN;
  err = testSer.serializeDouble(test_val);
  ASSERT_EQ(err, 1);
  deser_val = testSer.deserializeDouble();
  ASSERT_EQ(deser_val, test_val);
  
  // Test serialize above range
  test_val = DBL_MAX+1;
  err = testSer.serializeDouble(test_val);
  ASSERT_EQ(err, -1);

  // Test serialize below range
  test_val = DBL_MIN-1;
  err = testSer.serializeDouble(test_val);
  ASSERT_EQ(err, -1);
}

TEST(TestSerialize, testBufOverflow) {
  unsigned int err;
  int32_t test_val;
  int32_t deser_val;
  unsigned int buf_size = 8;
  uint8_t buf[buf_size];

  Serialize testSer(buf, buf_size); 

  test_val = 1;
  err = testSer.serializeInt32(test_val);
  ASSERT_EQ(err, 1);
  deser_val = testSer.deserializeInt32();
  ASSERT_EQ(deser_val, test_val);

  err = testSer.serializeInt32(test_val);
  ASSERT_EQ(err, 1);
  deser_val = testSer.deserializeInt32();
  ASSERT_EQ(deser_val, test_val);

  err = testSer.serializeInt32(test_val);
  ASSERT_EQ(err, -1);
  deser_val = testSer.deserializeInt32();
  ASSERT_EQ(deser_val, -1);
}


// TODO
TEST(TestSerialize, testMultipleTypes) {
  unsigned int err;
  unsigned int buf_size = 100;
  uint8_t buf[buf_size];

  Serialize testSer(buf, buf_size); 

  int32_t test_val1 = 12;
  int32_t deser_val1;
  err = testSer.serializeInt32(test_val1);
  ASSERT_EQ(err, 1);
  deser_val = testSer.deserializeInt32();
  ASSERT_EQ(deser_val1, test_val1);

  err = testSer.serializeInt32(test_val);
  ASSERT_EQ(err, 1);
  deser_val = testSer.deserializeInt32();
  ASSERT_EQ(deser_val, test_val);

  err = testSer.serializeInt32(test_val);
  ASSERT_EQ(err, -1);
  deser_val = testSer.deserializeInt32();
  ASSERT_EQ(deser_val, -1);
}

// test reset
// test update
