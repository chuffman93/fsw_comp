#include "../../include/util/serialize.h"

class CDHStatus: public Serialize {
public:

  float cpu1;
  float cpu5;
  float cpu15;
  float memory;

  void serialize(void);
  void deserialize(void);

};
