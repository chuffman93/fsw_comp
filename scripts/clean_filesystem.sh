#!/bin/sh

#######################################
# clean_filesystem.sh
# Last Modified: Jack Dinkel 01/11/17
#
# Removes all data files inside the CDH
#   filesystem
#######################################

source filesystem.sh

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
