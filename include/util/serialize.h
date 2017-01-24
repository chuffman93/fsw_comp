#include <stdint.h>


class Serialize {

public:
	Serialize(uint8_t * buf, unsigned int size);
	void reset();
	void update(uint8_t * buf, unsigned int size);
	int32_t deserializeInt32(void);
	int64_t deserializeInt64(void);
	float deserializeFloat(void);
	int serializeInt32(int32_t data);
	int serializeInt64(int64_t data);
	int serializeFloat(float fdata);

private:

	uint8_t * buf_ptr;
	unsigned int buf_size;
	unsigned int serialize_index;
	unsigned int deserialize_index;

};
