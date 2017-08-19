CC=g++
CFLAGS=
INCLUDES=-I../include

SERIALIZE=serialize
SERIALIZE_PATH=../src/util/$(SERIALIZE)
CREATE=create_bit_stream
DECODE=decode_bit_stream
EPS=HST_EPS_deserialize
ACS = HST_ACS_deserialize

SRCS=$(SERIALIZE_PATH).cpp $(CREATE).cpp $(DECODE).cpp

all: create_bit_stream decode_bit_stream

$(CREATE): $(CREATE).cpp $(SERIALIZE).o
	$(CC) $(CFLAGS) $(INCLUDES) -o $(CREATE) $(CREATE).cpp $(SERIALIZE).o
	
$(EPS): $(EPS).cpp $(SERIALIZE).o
	$(CC) $(CFLAGS) $(INCLUDES) -o $(EPS) $(EPS).cpp $(SERIALIZE).o
	
$(ACS): $(ACS).cpp $(SERIALIZE).o
	$(CC) $(CFLAGS) $(INCLUDES) -o $(ACS) $(ACS).cpp $(SERIALIZE).o

$(DECODE): $(DECODE).cpp $(SERIALIZE).o
	$(CC) $(CFLAGS) $(INCLUDES) -o $(DECODE) $(DECODE).cpp $(SERIALIZE).o

$(SERIALIZE).o: $(SERIALIZE_PATH).cpp
	$(CC) $(CFLAGS) $(INCLUDES) -c $(SERIALIZE_PATH).cpp

clean:
	$(RM) $(CREATE) $(DECODE) *.o *~

depend: $(SRCS)
	makedepend $(INCLUDES) $^
	
	


