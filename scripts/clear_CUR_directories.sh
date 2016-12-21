#!/bin/sh

#######################################
# clear_CUR_directories.sh
# Last Modified: Jack Dinkel 12/16/16
#
# Intended to be run everytim CDH boots.
# Moves all data in CUR directories to
# their data directory.
#
# Set this file to run on boot by putting
# it in your crontabs:
#     crontab -e
#     add the line: @reboot <filename>
#######################################

# All directories on /SD_2
TLM_DIRECTORIES="
GEN
HST
MOD
SWP
ERR
DGN
FSS
SSS
"

# All directories on /SD_3
SCI_DIRECTORIES="
RAD
"

for dir in $TLM_DIRECTORIES
do
  for file in `ls /SD_2/$dir/CUR`
  do
    tar -czf /SD_2/$dir/$file /SD_2/$dir/CUR/$file
    rm /SD_2/$dir/CUR/$file
  done
done

for dir in $SCI_DIRECTORIES
do
  for file in `ls /SD_2/$dir/CUR`
  do
    tar -czf /SD_2/$dir/$file /SD_2/$dir/CUR/$file
    rm /SD_3/$dir/CUR/$file
  done
done
