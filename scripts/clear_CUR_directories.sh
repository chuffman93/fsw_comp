#!/bin/sh

#######################################
# clear_CUR_directories.sh
# Last Modified: Jack Dinkel 01/12/17
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

SCRIPTS="/home/root/scripts"
FS_VARS="./filesystem.sh"

cd $SCRIPTS
source $FS_VARS
cd $OLDPWD

for dir in $TLM_DIRECTORIES
do
  for file in `ls /SD_2/$dir/CUR`
  do
    mv /SD_2/$dir/CUR/$file /SD_2/$dir/$file
  done
done

for dir in $HST_DIRECTORIES
do
  for file in `ls /SD_2/HST/$dir/CUR`
  do
    mv /SD_2/HST/$dir/CUR/$file /SD_2/HST/$dir/$file
  done
done

for dir in $SCI_DIRECTORIES
do
  for file in `ls /SD_3/$dir/CUR`
  do
    mv /SD_3/$dir/CUR/$file /SD_3/$dir/$file
  done
done
