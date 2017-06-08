#!/bin/sh

#######################################
# clean_filesystem.sh
# Last Modified: Jack Dinkel 01/12/17
#
# Removes all data files inside the CDH
#   filesystem
#######################################

SCRIPTS="/home/root/scripts"
FS_VARS="./filesystem.sh"

cd $SCRIPTS
source $FS_VARS
cd $OLDPWD

for dir in $TLM_DIRECTORIES
do
  echo "Cleaning /SD_2/$dir"
  for file in `ls -p /SD_2/$dir | grep -v /`
  do
    rm /SD_2/$dir/$file
  done

  for file in `ls -p /SD_2/$dir/CUR | grep -v /`
  do
    rm /SD_2/$dir/CUR/$file
  done
done

for dir in $HST_DIRECTORIES
do
  echo "Cleaning /SD_2/HST/$dir"
  for file in `ls -p /SD_2/HST/$dir | grep -v /`
  do
    rm /SD_2/HST/$dir/$file
  done

  for file in `ls -p /SD_2/HST/$dir/CUR | grep -v /`
  do
    rm /SD_2/HST/$dir/CUR/$file
  done
done

for dir in $SCI_DIRECTORIES
do
  echo "Cleaning /SD_3/$dir"
  for file in `ls -p /SD_3/$dir | grep -v /`
  do
    rm /SD_3/$dir/$file
  done

  for file in `ls -p /SD_3/$dir/CUR | grep -v /`
  do
    rm /SD_3/$dir/CUR/$file
  done
done
