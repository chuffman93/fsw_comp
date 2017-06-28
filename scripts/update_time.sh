#!/bin/sh

EPOCHFILE=/home/root/.config/PolarCube_Epoch

# set time to epoch
date -s "1 JAN 1970 00:00:00"

# increment epoch counter if the file exists
if [ -f $EPOCHFILE ]
then
  echo `cat $EPOCHFILE | awk '{print $1+1}'` > $EPOCHFILE
else
  echo 1 > $EPOCHFILE
fi
