#!/bin/sh

BASEDIR=`dirname "$0"`
python $BASEDIR/py/cpp.py $BASEDIR/../struct.txt > $BASEDIR/../../src/servers/HSTStructs.cpp
python $BASEDIR/py/hpp.py $BASEDIR/../struct.txt > $BASEDIR/../../include/servers/HSTStructs.h

rm $BASEDIR/py/*.pyc
