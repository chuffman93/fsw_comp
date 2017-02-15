CC=g++
CFLAGS=-I../include

all: create_bit_stream decode_bit_stream

create_bit_stream: create_bit_stream.cpp serialize.o
	$(CC) $(CFLAGS) -o create_bit_stream create_bit_stream.cpp serialize.o

decode_bit_stream: decode_bit_stream.cpp serialize.o
	$(CC) $(CFLAGS) -o decode_bit_stream decode_bit_stream.cpp serialize.o

serialize.o: ../src/util/serialize.cpp
	$(CC) $(CFLAGS) -c ../src/util/serialize.cpp

clean:
	$(RM) create_bit_stream decode_bit_stream *.o *~
