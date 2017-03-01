
#include "struct.h"
#include <iostream>

using namespace std;

void CDHStatus::serialize(void) {
  serializeFloat(this->cpu1);
  this->serializeFloat(this->cpu5);
  this->serializeFloat(this->cpu15);
  this->serializeFloat(this->memory);
}

void CDHStatus::deserialize(void) {
  this->cpu1 = this->deserializeFloat();
  this->cpu5 = this->deserializeFloat();
  this->cpu15 = this->deserializeFloat();
  this->memory = this->deserializeFloat();
}

int main() {
  CDHStatus stat;
  stat.cpu1 = 5;

  uint8_t buf[100];
  stat.update(buf, 100);
  stat.serialize();

  CDHStatus stat2;
  stat2.cpu1 = 10;
  cout << "stat2.cpu1 " << stat2.cpu1 << endl;
  stat2.update(buf, 100);
  stat2.deserialize();
  cout << "stat2.cpu1 " << stat2.cpu1 << endl;


  return 0;
}
