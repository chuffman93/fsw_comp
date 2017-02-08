#!/bin/sh

python py/cpp.py ../dict.txt > ../../src/util/TLM.cpp
python py/hpp.py ../dict.txt > ../../include/util/TLM.h
