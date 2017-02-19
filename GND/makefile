CC=g++
CFLAGS=
INCLUDES=-I../include

SERIALIZE=serialize
SERIALIZE_PATH=../src/util/$(SERIALIZE)
CREATE=create_bit_stream
DECODE=decode_bit_stream

SRCS=$(SERIALIZE_PATH).cpp $(CREATE).cpp $(DECODE).cpp

all: create_bit_stream decode_bit_stream

$(CREATE): $(CREATE).cpp $(SERIALIZE).o
	$(CC) $(CFLAGS) $(INCLUDES) -o $(CREATE) $(CREATE).cpp $(SERIALIZE).o

$(DECODE): $(DECODE).cpp $(SERIALIZE).o
	$(CC) $(CFLAGS) $(INCLUDES) -o $(DECODE) $(DECODE).cpp $(SERIALIZE).o

$(SERIALIZE).o: $(SERIALIZE_PATH).cpp
	$(CC) $(CFLAGS) $(INCLUDES) -c $(SERIALIZE_PATH).cpp

clean:
	$(RM) $(CREATE) $(DECODE) *.o *~

depend: $(SRCS)
	makedepend $(INCLUDES) $^

