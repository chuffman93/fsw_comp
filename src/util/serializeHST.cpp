#include "util/serializeHST.h"
#include "util/serialize.h"
#include "servers/HSTStructs.h"
#include <stdlib.h>

using namespace std;

namespace AllStar{
namespace Servers{

SerializeHST::SerializeHST(uint8_t * buf, unsigned int size)
							: Serialize(buf, size) {

}

void SerializeHST::serializeCDHStatus(CDHStatus * strct) {
  serializeFloat(strct->cpu1);
  serializeFloat(strct->cpu5);
  serializeFloat(strct->cpu15);
  serializeFloat(strct->memory);
}

void SerializeHST::deserializeCDHStatus(CDHStatus * strct) {
  strct->cpu1 = deserializeFloat();
  strct->cpu5 = deserializeFloat();
  strct->cpu15 = deserializeFloat();
  strct->memory = deserializeFloat();
}

}
}
