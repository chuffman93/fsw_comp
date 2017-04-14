//#define __STDC_LIMIT_MACROS
//#include "gtest/gtest.h"
//#include "util/serialize.h"
//#include <stdint.h>
//#include <limits>
//#include <float.h>
//
//TEST(TestSerialize, testInt8) {
//  unsigned int err;
//  int8_t test_val;
//  int8_t deser_val;
//  unsigned int buf_size = 50;
//  uint8_t buf[buf_size];
//
//  Serialize testSer(buf, buf_size);
//
//  // Test a value in range
//  test_val = 1;
//  err = testSer.serializeInt8(test_val);
//  ASSERT_EQ(err, 1);
//  deser_val = testSer.deserializeInt8();
//  ASSERT_EQ(deser_val, test_val);
//
//
//  // Test maximum value
//  test_val = INT8_MAX;
//  err = testSer.serializeInt8(test_val);
//  ASSERT_EQ(err, 1);
//  deser_val = testSer.deserializeInt8();
//  ASSERT_EQ(deser_val, test_val);
//
//  // Test minimum value
//  test_val = INT_LEAST8_MIN;
//  err = testSer.serializeInt8(test_val);
//  ASSERT_EQ(err, 1);
//  deser_val = testSer.deserializeInt8();
//  ASSERT_EQ(deser_val, test_val);
//}
//
//TEST(TestSerialize, testInt16) {
//  unsigned int err;
//  int16_t test_val;
//  int16_t deser_val;
//  unsigned int buf_size = 50;
//  uint8_t buf[buf_size];
//
//  Serialize testSer(buf, buf_size);
//
//  // Test a value in range
//  test_val = 1;
//  err = testSer.serializeInt16(test_val);
//  ASSERT_EQ(err, 1);
//  deser_val = testSer.deserializeInt16();
//  ASSERT_EQ(deser_val, test_val);
//
//
//  // Test maximum value
//  test_val = INT16_MAX;
//  err = testSer.serializeInt16(test_val);
//  ASSERT_EQ(err, 1);
//  deser_val = testSer.deserializeInt16();
//  ASSERT_EQ(deser_val, test_val);
//
//  // Test minimum value
//  test_val = INT_LEAST16_MIN;
//  err = testSer.serializeInt16(test_val);
//  ASSERT_EQ(err, 1);
//  deser_val = testSer.deserializeInt16();
//  ASSERT_EQ(deser_val, test_val);
//}
//
//TEST(TestSerialize, testInt32) {
//  unsigned int err;
//  int32_t test_val;
//  int32_t deser_val;
//  unsigned int buf_size = 50;
//  uint8_t buf[buf_size];
//
//  Serialize testSer(buf, buf_size);
//
//  // Test a value in range
//  test_val = 1;
//  err = testSer.serializeInt32(test_val);
//  ASSERT_EQ(err, 1);
//  deser_val = testSer.deserializeInt32();
//  ASSERT_EQ(deser_val, test_val);
//
//
//  // Test maximum value
//  test_val = INT32_MAX;
//  err = testSer.serializeInt32(test_val);
//  ASSERT_EQ(err, 1);
//  deser_val = testSer.deserializeInt32();
//  ASSERT_EQ(deser_val, test_val);
//
//  // Test minimum value
//  test_val = INT_LEAST32_MIN;
//  err = testSer.serializeInt32(test_val);
//  ASSERT_EQ(err, 1);
//  deser_val = testSer.deserializeInt32();
//  ASSERT_EQ(deser_val, test_val);
//}
//
//TEST(TestSerialize, testInt64) {
//  unsigned int err;
//  int64_t test_val;
//  int64_t deser_val;
//  unsigned int buf_size = 50;
//  uint8_t buf[buf_size];
//
//  Serialize testSer(buf, buf_size);
//
//  // Test a value in range
//  test_val = 1;
//  err = testSer.serializeInt64(test_val);
//  ASSERT_EQ(err, 1);
//  deser_val = testSer.deserializeInt64();
//  ASSERT_EQ(deser_val, test_val);
//
//
//  // Test maximum value
//  test_val = INT64_MAX;
//  err = testSer.serializeInt64(test_val);
//  ASSERT_EQ(err, 1);
//  deser_val = testSer.deserializeInt64();
//  ASSERT_EQ(deser_val, test_val);
//
//  // Test minimum value
//  test_val = INT_LEAST64_MIN;
//  err = testSer.serializeInt64(test_val);
//  ASSERT_EQ(err, 1);
//  deser_val = testSer.deserializeInt64();
//  ASSERT_EQ(deser_val, test_val);
//}
//
//TEST(TestSerialize, testUInt8) {
//  unsigned int err;
//  uint8_t test_val;
//  uint8_t deser_val;
//  unsigned int buf_size = 50;
//  uint8_t buf[buf_size];
//
//  Serialize testSer(buf, buf_size);
//
//  // Test a value in range
//  test_val = 1;
//  err = testSer.serializeUInt8(test_val);
//  ASSERT_EQ(err, 1);
//  deser_val = testSer.deserializeUInt8();
//  ASSERT_EQ(deser_val, test_val);
//
//  // Test maximum value
//  test_val = UINT8_MAX;
//  err = testSer.serializeUInt8(test_val);
//  ASSERT_EQ(err, 1);
//  deser_val = testSer.deserializeUInt8();
//  ASSERT_EQ(deser_val, test_val);
//
//  // Test minimum value
//  test_val = 0;
//  err = testSer.serializeUInt8(test_val);
//  ASSERT_EQ(err, 1);
//  deser_val = testSer.deserializeUInt8();
//  ASSERT_EQ(deser_val, test_val);
//}
//
//TEST(TestSerialize, testUInt16) {
//  unsigned int err;
//  uint16_t test_val;
//  uint16_t deser_val;
//  unsigned int buf_size = 50;
//  uint8_t buf[buf_size];
//
//  Serialize testSer(buf, buf_size);
//
//  // Test a value in range
//  test_val = 1;
//  err = testSer.serializeUInt16(test_val);
//  ASSERT_EQ(err, 1);
//  deser_val = testSer.deserializeUInt16();
//  ASSERT_EQ(deser_val, test_val);
//
//
//  // Test maximum value
//  test_val = UINT16_MAX;
//  err = testSer.serializeUInt16(test_val);
//  ASSERT_EQ(err, 1);
//  deser_val = testSer.deserializeUInt16();
//  ASSERT_EQ(deser_val, test_val);
//
//  // Test minimum value
//  test_val = 0;
//  err = testSer.serializeUInt16(test_val);
//  ASSERT_EQ(err, 1);
//  deser_val = testSer.deserializeUInt16();
//  ASSERT_EQ(deser_val, test_val);
//}
//
//TEST(TestSerialize, testUInt32) {
//  unsigned int err;
//  uint32_t test_val;
//  uint32_t deser_val;
//  unsigned int buf_size = 50;
//  uint8_t buf[buf_size];
//
//  Serialize testSer(buf, buf_size);
//
//  // Test a value in range
//  test_val = 1;
//  err = testSer.serializeUInt32(test_val);
//  ASSERT_EQ(err, 1);
//  deser_val = testSer.deserializeUInt32();
//  ASSERT_EQ(deser_val, test_val);
//
//  // Test maximum value
//  test_val = UINT32_MAX;
//  err = testSer.serializeUInt32(test_val);
//  ASSERT_EQ(err, 1);
//  deser_val = testSer.deserializeUInt32();
//  ASSERT_EQ(deser_val, test_val);
//
//  // Test minimum value
//  test_val = 0;
//  err = testSer.serializeUInt32(test_val);
//  ASSERT_EQ(err, 1);
//  deser_val = testSer.deserializeUInt32();
//  ASSERT_EQ(deser_val, test_val);
//}
//
//TEST(TestSerialize, testUInt64) {
//  unsigned int err;
//  uint64_t test_val;
//  uint64_t deser_val;
//  unsigned int buf_size = 50;
//  uint8_t buf[buf_size];
//
//  Serialize testSer(buf, buf_size);
//
//  // Test a value in range
//  test_val = 1;
//  err = testSer.serializeUInt64(test_val);
//  ASSERT_EQ(err, 1);
//  deser_val = testSer.deserializeUInt64();
//  ASSERT_EQ(deser_val, test_val);
//
//  // Test maximum value
//  test_val = UINT64_MAX;
//  err = testSer.serializeUInt64(test_val);
//  ASSERT_EQ(err, 1);
//  deser_val = testSer.deserializeUInt64();
//  ASSERT_EQ(deser_val, test_val);
//
//  // Test minimum value
//  test_val = 0;
//  err = testSer.serializeUInt64(test_val);
//  ASSERT_EQ(err, 1);
//  deser_val = testSer.deserializeUInt64();
//  ASSERT_EQ(deser_val, test_val);
//}
//
//TEST(TestSerialize, testBool) {
//  unsigned int err;
//  bool test_val;
//  bool deser_val;
//  unsigned int buf_size = 50;
//  uint8_t buf[buf_size];
//
//  Serialize testSer(buf, buf_size);
//
//  // Test true
//  test_val = true;
//  err = testSer.serializeInt8(test_val);
//  ASSERT_EQ(err, 1);
//  deser_val = testSer.deserializeInt8();
//  ASSERT_EQ(deser_val, test_val);
//
//  // Test false
//  test_val = false;
//  err = testSer.serializeInt8(test_val);
//  ASSERT_EQ(err, 1);
//  deser_val = testSer.deserializeInt8();
//  ASSERT_EQ(deser_val, test_val);
//}
//
//TEST(TestSerialize, testFloat) {
//  unsigned int err;
//  float test_val;
//  float deser_val;
//  unsigned int buf_size = 50;
//  uint8_t buf[buf_size];
//
//  Serialize testSer(buf, buf_size);
//
//  // Test a value in range
//  test_val = 1.2;
//  err = testSer.serializeFloat(test_val);
//  ASSERT_EQ(err, 1);
//  deser_val = testSer.deserializeFloat();
//  ASSERT_EQ(deser_val, test_val);
//
//  // Test maximum value
//  test_val = FLT_MAX;
//  err = testSer.serializeFloat(test_val);
//  ASSERT_EQ(err, 1);
//  deser_val = testSer.deserializeFloat();
//  ASSERT_EQ(deser_val, test_val);
//
//  // Test minimum value
//  test_val = FLT_MIN;
//  err = testSer.serializeFloat(test_val);
//  ASSERT_EQ(err, 1);
//  deser_val = testSer.deserializeFloat();
//  ASSERT_EQ(deser_val, test_val);
//}
//
//TEST(TestSerialize, testDouble) {
//  unsigned int err;
//  double test_val;
//  double deser_val;
//  unsigned int buf_size = 50;
//  uint8_t buf[buf_size];
//
//  Serialize testSer(buf, buf_size);
//
//  // Test a value in range
//  test_val = 1.2;
//  err = testSer.serializeDouble(test_val);
//  ASSERT_EQ(err, 1);
//  deser_val = testSer.deserializeDouble();
//  ASSERT_EQ(deser_val, test_val);
//
//  // Test maximum value
//  test_val = DBL_MAX;
//  err = testSer.serializeDouble(test_val);
//  ASSERT_EQ(err, 1);
//  deser_val = testSer.deserializeDouble();
//  ASSERT_EQ(deser_val, test_val);
//
//  // Test minimum value
//  test_val = DBL_MIN;
//  err = testSer.serializeDouble(test_val);
//  ASSERT_EQ(err, 1);
//  deser_val = testSer.deserializeDouble();
//  ASSERT_EQ(deser_val, test_val);
//}
//
//TEST(TestSerialize, testBufOverflow) {
//  unsigned int err1;
//  unsigned int err2;
//  unsigned int err3;
//  int32_t test_val;
//  int32_t deser_val1;
//  int32_t deser_val2;
//  int32_t deser_val3;
//  unsigned int buf_size = 8;
//  uint8_t buf[buf_size];
//
//  Serialize testSer(buf, buf_size);
//
//  test_val = 12;
//  err1 = testSer.serializeInt32(test_val);
//  ASSERT_EQ(err1, 1);
//  deser_val1 = testSer.deserializeInt32();
//  ASSERT_EQ(deser_val1, test_val);
//
//  err2 = testSer.serializeInt32(test_val);
//  ASSERT_EQ(err2, 1);
//  deser_val2 = testSer.deserializeInt32();
//  ASSERT_EQ(deser_val2, test_val);
//
//  err3 = testSer.serializeInt32(test_val);
//  ASSERT_EQ(err3, -1);
//  deser_val3 = testSer.deserializeInt32();
//  ASSERT_EQ(deser_val3, -1);
//}
//
//
//// TODO
//TEST(TestSerialize, testMultipleTypes) {
//  unsigned int err;
//  unsigned int buf_size = 100;
//  uint8_t buf[buf_size];
//
//  Serialize testSer(buf, buf_size);
//
//  int32_t test_val1 = -1542;
//  int32_t deser_val1;
//  err = testSer.serializeInt32(test_val1);
//  ASSERT_EQ(err, 1);
//  deser_val1 = testSer.deserializeInt32();
//  ASSERT_EQ(deser_val1, test_val1);
//
//  uint8_t test_val2 = 94;
//  err = testSer.serializeUInt8(test_val2);
//  ASSERT_EQ(err, 1);
//
//  double test_val3 = 112.42455;
//  err = testSer.serializeDouble(test_val3);
//  ASSERT_EQ(err, 1);
//
//  bool test_val4 = true;
//  err = testSer.serializeBool(test_val4);
//  ASSERT_EQ(err, 1);
//
//  uint8_t deser_val2;
//  deser_val2 = testSer.deserializeUInt8();
//  ASSERT_EQ(deser_val2, test_val2);
//
//  double deser_val3;
//  deser_val3 = testSer.deserializeDouble();
//  ASSERT_EQ(deser_val3, test_val3);
//
//  bool deser_val4;
//  deser_val4 = testSer.deserializeDouble();
//  ASSERT_EQ(deser_val4, test_val4);
//}
//
//TEST(TestSerialize, testReset) {
//  unsigned int err;
//  unsigned int buf_size = 100;
//  uint8_t buf[buf_size];
//
//  Serialize testSer(buf, buf_size);
//
//  int32_t test_val1 = -1542;
//  int32_t deser_val1;
//  err = testSer.serializeInt32(test_val1);
//  ASSERT_EQ(err, 1);
//  deser_val1 = testSer.deserializeInt32();
//  ASSERT_EQ(deser_val1, test_val1);
//
//  testSer.reset();
//
//  deser_val1 = testSer.deserializeInt32();
//  ASSERT_EQ(deser_val1, test_val1);
//
//  testSer.reset();
//
//  uint8_t test_val2 = 94;
//  uint8_t deser_val2;
//  err = testSer.serializeUInt8(test_val2);
//  ASSERT_EQ(err, 1);
//  deser_val2 = testSer.deserializeUInt8();
//  ASSERT_EQ(deser_val2, test_val2);
//}
//
//TEST(TestSerialize, testUpdate) {
//  unsigned int err;
//  unsigned int buf_size1 = 100;
//  unsigned int buf_size2 = 30;
//  uint8_t buf1[buf_size1];
//  uint8_t buf2[buf_size2];
//
//  Serialize testSer1(buf1, buf_size1);
//  Serialize testSer2(buf2, buf_size2);
//
//  int32_t test_val1 = -1542;
//  int32_t deser_val1;
//  err = testSer1.serializeInt32(test_val1);
//  ASSERT_EQ(err, 1);
//  deser_val1 = testSer1.deserializeInt32();
//  ASSERT_EQ(deser_val1, test_val1);
//
//  uint8_t test_val2 = 94;
//  uint8_t deser_val2;
//  err = testSer2.serializeUInt8(test_val2);
//  ASSERT_EQ(err, 1);
//  deser_val2 = testSer2.deserializeUInt8();
//  ASSERT_EQ(deser_val2, test_val2);
//
//  testSer1.update(buf2, buf_size2);
//
//  err = testSer2.serializeUInt8(test_val2);
//  ASSERT_EQ(err, 1);
//  deser_val2 = testSer2.deserializeUInt8();
//  ASSERT_EQ(deser_val2, test_val2);
//}
