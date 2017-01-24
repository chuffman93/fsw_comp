#include <stdint.h>


class Serialize {

public:
	Serialize(uint8_t * buf, unsigned int size);
	void reset();
	void update(uint8_t * buf, unsigned int size);

	// ---- Deserialize ----------------------------------
	int8_t deserializeInt8(void);
	int16_t deserializeInt16(void);
	int32_t deserializeInt32(void);
	int64_t deserializeInt64(void);
	uint8_t deserializeInt8(void);
	uint16_t deserializeUInt16(void);
	uint32_t deserializeUInt32(void);
	uint64_t deserializeUInt64(void);
	bool deserializeBool(void);
	float deserializeFloat(void);
	double deserializeDouble(void);

	// ---- Serialize ----------------------------------
	int serializeInt8(int16_t data);
	int serializeInt16(int16_t data);
	int serializeInt32(int32_t data);
	int serializeInt64(int64_t data);
	int serializeUInt8(uint16_t data);
	int serializeUInt16(uint16_t data);
	int serializeUInt32(uint32_t data);
	int serializeUInt64(uint64_t data);
	int serializeBool(bool data);
	int serializeFloat(float fdata);
	int deserializeDouble(double ddata);

private:

	uint8_t * buf_ptr;
	unsigned int buf_size;
	unsigned int serialize_index;
	unsigned int deserialize_index;

};
