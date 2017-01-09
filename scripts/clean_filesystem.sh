#!/bin/sh

#######################################
# clean_filesystem.sh
# Last Modified: Jack Dinkel 01/09/17
#
# Removes all data files inside the CDH
#   filesystem
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
  echo "Cleanning /SD_2/$dir"
  for file in `ls -p /SD_2/$dir | grep -v /`
    do
      rm /SD_2/$dir/$file
    done

  for file in `ls -p /SD_2/$dir/CUR | grep -v /`
    do
      rm /SD_2/$dir/CUR/$file
    done
done

for dir in $SCI_DIRECTORIES
do
  echo "Cleanning /SD_3/$dir"
  for file in `ls -p /SD_3/$dir | grep -v /`
    do
      rm /SD_3/$dir/$file
    done

  for file in `ls -p /SD_3/$dir/CUR | grep -v /`
    do
      rm /SD_3/$dir/CUR/$file
    done
done
