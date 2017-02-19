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
  cout << "Enter the name of the file you wish to deserialize: ";
  cin >> filename;

  size_t buf_size = 500;
  uint8_t buf[buf_size];
  ifstream in(filename.c_str(), ios::in | ios::binary );
  in.read((char *)buf, buf_size);
  in.close();
  Serialize serializable(buf, buf_size);

  bool done = false;
  while (!done) {
    size_t size;
  
    int type_num;
    cout << "1. Int8 \t5. UInt8 \t 9. Float"  << endl;
    cout << "2. Int16\t6. UInt16\t10. Double" << endl;
    cout << "3. Int32\t7. UInt32\t11. Bool"   << endl;
    cout << "4. Int64\t8. UInt64"             << endl;
    cout << "0. Done"                         << endl;
    cout << "Enter the corresponding number for the type you wish to deserialize: ";
    cin >> type_num;
  
    switch (type_num) {
  
      case (0): {
        done = true;
        break;
      }
      case (1): {
        int8_t val;
        val = serializable.deserializeInt8();
        printf("Value: %d\n", val);
        break;
      }
      case (2): {
        int16_t val;
        val = serializable.deserializeInt16();
        cout << "Value: " << val << endl;
        break;
      }
      case (3): {
        int32_t val;
        val = serializable.deserializeInt32();
        cout << "Value: " << val << endl;
        break;
      }
      case (4): {
        int64_t val;
        val = serializable.deserializeInt64();
        cout << "Value: " << val << endl;
        break;
      }
      case (5): {
        uint8_t val;
        val = serializable.deserializeUInt8();
        printf("Value: %d\n", val);
        break;
      }
      case (6): {
        uint16_t val;
        val = serializable.deserializeUInt16();
        cout << "Value: " << val << endl;
        break;
      }
      case (7): {
        uint32_t val;
        val = serializable.deserializeUInt32();
        cout << "Value: " << val << endl;
        break;
      }
      case (8): {
        uint64_t val;
        val = serializable.deserializeUInt64();
        cout << "Value: " << val << endl;
        break;
      }
      case (9): {
        float val;
        val = serializable.deserializeFloat();
        cout << "Value: " << val << endl;
        break;
      }
      case (10): {
        double val;
        val = serializable.deserializeDouble();
        cout << "Value: " << val << endl;
        break;
      }
      case (11): {
        bool val;
        val = serializable.deserializeBool();
        cout << "Value: " << val << endl;
        break;
      }
      case (12): {
        done = true;
        break;
      }
      default: {
        cout << endl << "Invalid Input." << endl;
        break;
      }
    }
  }


  return 0;
}
