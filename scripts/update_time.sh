#!/bin/sh

EPOCHFILE=/home/root/.config/PolarCube_Epoch
REBOOTFILE=/home/root/.config/Reboot_Time

# set time to epoch
date -s "@0"

# increment epoch counter if the file exists
if [ -f $EPOCHFILE ]
then
  echo `cat $EPOCHFILE | awk '{print $1+1}'` > $EPOCHFILE
else
  echo 1 > $EPOCHFILE
fi

# update the system time from the reboot time file if it exists
if [ -f $REBOOTFILE ]
then
  date -s "@`cat $REBOOTFILE`"
  rm $REBOOTFILE
fi


