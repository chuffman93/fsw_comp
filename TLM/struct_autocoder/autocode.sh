#!/bin/sh

BASEDIR=`dirname "$0"`
python $BASEDIR/py/cpp.py $BASEDIR/../struct.txt > $BASEDIR/../../src/servers/Structs.cpp
python $BASEDIR/py/hpp.py $BASEDIR/../struct.txt > $BASEDIR/../../include/servers/Structs.h

rm $BASEDIR/py/*.pyc
