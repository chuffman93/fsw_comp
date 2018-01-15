/*
 * BiteStream.h
 *
 *  Created on: Jan 7, 2018
 *      Author: alpa3119
 */

#ifndef BYTESTREAM_H_
#define BYTESTREAM_H_

class ByteStream {
public:
	ByteStream(){
		index = data.begin();
	}
	ByteStream(std::vector<uint8_t> data) : data(data) {
		index = this->data.begin();
	}
	~ByteStream(){}
	//serialize
    template <typename T> inline ByteStream& operator<<(T& t)
    {
    	size_t size = sizeof(T);
    	uint8_t * start = (uint8_t*)&t;
    	for (uint8_t * byte = start+size-1; byte>=start; byte--){
    		data.push_back(*byte);
    	}

        return * this;
    }
    //deserialize
    template <typename T> inline ByteStream& operator>>(T& t)
    {
    	size_t size = sizeof(T);
    	uint8_t * start = (uint8_t*)&t;
    	for (uint8_t * byte = start+size-1; byte>=start; byte--){
    		if (index == data.end()){
    			t = 0;
    			return *this;
    		}
    		*byte = *index++;
    	}
        return * this;
    }

    std::vector<uint8_t> vec(){
    	return data;
    }

    ByteStream& seek(size_t i){
    	if (i <= data.size()){
    		index = data.begin() + i;
    	}
    	return *this;
    }

private:
    std::vector<uint8_t> data;
    std::vector<uint8_t>::iterator index;
};



#endif /* BYTESTREAM_H_ */
