#!/bin/sh

BASEDIR=`dirname "$0"`
python $BASEDIR/py/cpp.py $BASEDIR/../dict.txt > $BASEDIR/../../src/util/TLM.cpp
python $BASEDIR/py/hpp.py $BASEDIR/../dict.txt > $BASEDIR/../../include/util/TLM.h

rm $BASEDIR/py/*.pyc
