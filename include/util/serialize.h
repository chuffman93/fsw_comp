#ifndef SERIALIZE_H_
#define SERIALIZE_H_

#include <stdint.h>
#include <stdlib.h>

class Serialize {

public:
    Serialize();
	Serialize(uint8_t * buf, size_t size);
	void reset();
	void setIndex(unsigned int ser_index, unsigned int deser_index);
	void update(uint8_t * buf, size_t size);
	void update(uint8_t * buf, size_t size, unsigned int ser_index, unsigned int deser_index);

	unsigned int get_serialize_index();
	unsigned int get_deserialize_index();

	// ---- Deserialize ----------------------------------
	int8_t   deserialize_int8(void);
	int16_t  deserialize_int16(void);
	int32_t  deserialize_int32(void);
	int64_t  deserialize_int64(void);
	uint8_t  deserialize_uint8(void);
	uint16_t deserialize_uint16(void);
	uint32_t deserialize_uint32(void);
	uint64_t deserialize_uint64(void);
	bool     deserialize_bool(void);
	float    deserialize_float(void);
	double   deserialize_double(void);

	// ---- Serialize ----------------------------------
	int serialize_int8(int8_t data);
	int serialize_int16(int16_t data);
	int serialize_int32(int32_t data);
	int serialize_int64(int64_t data);
	int serialize_uint8(uint8_t data);
	int serialize_uint16(uint16_t data);
	int serialize_uint32(uint32_t data);
	int serialize_uint64(uint64_t data);
	int serialize_bool(bool bdata);
	int serialize_float(float fdata);
	int serialize_double(double ddata);

protected:
	uint8_t * buf_ptr; // TODO: Make this static
	unsigned int buf_size;
	unsigned int serialize_index;
	unsigned int deserialize_index;

};

#endif
