#define __STDC_LIMIT_MACROS
#include "util/serialize.h"
#include <iostream>
#include <fstream>
#include <string>
#include <limits>
#include <stdint.h>
#include <float.h>

using namespace std;

int main() {

  string filename;
  cout << "Enter the name of the file you wish to create: ";
  cin >> filename;

  size_t buf_size = 1000;
  uint8_t buf[buf_size];
  Serialize serializable(buf, buf_size);

  unsigned int size;

  bool done = false;
  while (!done) {
  
    int type_num;
    cout << "1. Int8 \t5. UInt8 \t 9. Float"  << endl;
    cout << "2. Int16\t6. UInt16\t10. Double" << endl;
    cout << "3. Int32\t7. UInt32\t11. Bool"   << endl;
    cout << "4. Int64\t8. UInt64"             << endl;
    cout << "0. Done"                         << endl;
    cout << "Enter the corresponding number for the type you wish to serialize: ";
    cin >> type_num;
  
    switch (type_num) {
  
      case (0): {
        done = true;
        break;
      }
      case (1): {
        int8_t val;
        int i_val;
        cout << "Enter the value you wish to serialize: ";
        while ( !(cin >> i_val) || i_val < INT8_MIN || i_val > INT8_MAX ) {
          cout << "Invalid Input. Please try again with your input between " << INT8_MIN << " and " << INT8_MAX << ": ";
          cin.clear();
          cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
        val = i_val;
        serializable.serializeInt8(val);
        size += sizeof(int8_t);
        break;
      }
      case (2): {
        int16_t val;
        cout << "Enter the value you wish to serialize: ";
        while ( !(cin >> val) ) {
          cout << "Invalid Input. Please try again with your input between " << INT16_MIN << " and " << INT16_MAX << ": ";
          cin.clear();
          cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
        serializable.serializeInt16(val);
        size += sizeof(int16_t);
        break;
      }
      case (3): {
        int32_t val;
        cout << "Enter the value you wish to serialize: ";
        while ( !(cin >> val) ) {
          cout << "Invalid Input. Please try again with your input between " << INT32_MIN << " and " << INT32_MAX << ": ";
          cin.clear();
          cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
        serializable.serializeInt32(val);
        size += sizeof(int32_t);
        break;
      }
      case (4): {
        int64_t val;
        cout << "Enter the value you wish to serialize: ";
        while ( !(cin >> val) ) {
          cout << "Invalid Input. Please try again with your input between " << INT64_MIN << " and " << INT64_MAX << ": ";
          cin.clear();
          cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
        serializable.serializeInt64(val);
        size += sizeof(int64_t);
        break;
      }
      case (5): {
        unsigned int i_val;
        uint8_t val;
        cout << "Enter the value you wish to serialize: ";
        while ( !(cin >> i_val) || i_val < 0 || i_val > UINT8_MAX ) {
          cout << "Invalid Input. Please try again with your input between " << 0 << " and " << UINT8_MAX << ": ";
          cin.clear();
          cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
        val = i_val;
        serializable.serializeUInt8(val);
        size += sizeof(uint8_t);
        break;
      }
      case (6): {
        unsigned int i_val;
        uint16_t val;
        cout << "Enter the value you wish to serialize: ";
        while ( !(cin >> i_val) || (i_val < 0) || (i_val > UINT16_MAX) ) {
          cout << "Invalid Input. Please try again with your input between " << 0 << " and " << UINT16_MAX << ": ";
          cin.clear();
          cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
        val = i_val;
        serializable.serializeUInt16(val);
        size += sizeof(uint16_t);
        break;
      }
      case (7): { // TODO: Does not actually stop negative numbers from being input
        uint32_t val;
        cout << "Enter the value you wish to serialize: ";
        while ( !(cin >> val) || (val < 0) || (val > UINT32_MAX) ) {
          cout << "Invalid Input. Please try again with your input between " << 0 << " and " << UINT32_MAX << ": ";
          cin.clear();
          cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
        serializable.serializeUInt32(val);
        size += sizeof(uint32_t);
        break;
      }
      case (8): { // TODO: Does not actually stop negative numbers from being input
        uint64_t val;
        cout << "Enter the value you wish to serialize: ";
        while ( !(cin >> val) || (val < 0) || (val > UINT64_MAX) ) {
          cout << "Invalid Input. Please try again with your input between " << 0 << " and " << UINT64_MAX << ": ";
          cin.clear();
          cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
        serializable.serializeUInt64(val);
        size += sizeof(uint64_t);
        break;
      }
      case (9): {
        float val;
        cout << "Enter the value you wish to serialize: ";
        while ( !(cin >> val) ) {
          cout << "Invalid Input. Please try again with your input between " << FLT_MIN << " and " << FLT_MAX << ": ";
          cin.clear();
          cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
        serializable.serializeFloat(val);
        size += sizeof(float);
        break;
      }
      case (10): {
        double val;
        cout << "Enter the value you wish to serialize: ";
        while ( !(cin >> val) ) {
          cout << "Invalid Input. Please try again with your input between " << DBL_MIN << " and " << DBL_MAX << ": ";
          cin.clear();
          cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
        serializable.serializeDouble(val);
        size += sizeof(double);
        break;
      }
      case (11): {
        bool val;
        cout << "Enter the value you wish to serialize: ";
        while ( !(cin >> val) ) {
          cout << "Invalid Input. Please try again with your input between " << "0 (false)" << " and " << "1 (true)" << ": ";
          cin.clear();
          cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
        serializable.serializeBool(val);
        size += sizeof(bool);
        break;
      }
      default: {
        cout << endl << "Invalid Input." << endl;
        break;
      }
    }
  }

  uint8_t final_buf[size];
  copy(buf, buf+size, final_buf);

  ofstream fp;
  fp.open(filename.c_str(), ios::out | ios::binary);
  fp.write((char *) final_buf, size);
  fp.close();

  return 0;
}
