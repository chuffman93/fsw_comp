#include "util/serialize.h"

namespace AllStar{
namespace Servers{

class SerializeHST: Serialize {

public:
	SerializeHST(uint8_t * buf, unsigned int size);

	void serializeCDHStatus(CDHStatus * strct);
	void deserializeCDHStatus(CDHStatus * strct);
};

}
}
