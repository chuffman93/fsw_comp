
#include "struct.h"
#include <iostream>

using namespace std;

void CDHStatus::serialize(void) {
  serializeFloat(this->cpu1);
  this->serializeFloat(this->cpu5);
  this->serializeFloat(this->cpu15);
  this->serializeFloat(this->memory);
  for (int i = 0; i < 5; i++) {
    this->serializeInt32(this->arr[i]);
  }
}

void CDHStatus::deserialize(void) {
  this->cpu1 = this->deserializeFloat();
  this->cpu5 = this->deserializeFloat();
  this->cpu15 = this->deserializeFloat();
  this->memory = this->deserializeFloat();
  for (int i = 0; i < 5; i++) {
    this->arr[i] = this->deserializeInt32();
  }
}

int main() {
  CDHStatus stat;
  stat.cpu1 = 5;
  for (int i = 0; i < 5; i++) {
    stat.arr[i] = i;
  }
  cout << "stat.arr ";
  for (int i = 0; i < 5; i++) {
    cout << stat.arr[i] << " ";
  }
  cout << endl;

  uint8_t buf[100];
  stat.update(buf, 100);
  stat.serialize();

  CDHStatus stat2;
  stat2.cpu1 = 10;
  for (int i = 0; i < 5; i++) {
    stat2.arr[i] = i*2;
  }

  cout << "stat2.cpu1 " << stat2.cpu1 << endl;
  cout << "stat2.arr ";
  for (int i = 0; i < 5; i++) {
    cout << stat2.arr[i] << " ";
  }
  cout << endl;

  stat2.update(buf, 100);
  stat2.deserialize();

  cout << "stat2.cpu1 " << stat2.cpu1 << endl;
  cout << "stat2.arr ";
  for (int i = 0; i < 5; i++) {
    cout << stat2.arr[i] << " ";
  }
  cout << endl;


  return 0;
}
