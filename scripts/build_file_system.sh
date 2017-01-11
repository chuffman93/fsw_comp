#bin/sh

#######################################
# build_file_system.sh
# Last Modified: Jack Dinkel 01/11/17
#
# Creates the directory structure for
# telemetry and science data on CDH
#######################################

source filesystem.sh

mkdir $SD1 # Kernel backup partition
mkdir $SD2 # Log Data Storage partition
mkdir $SD3 # Science Data Storage partition

for dir in $TLM_DIRECTORIES
do
  mkdir /SD_2/$dir
  mkdir /SD_2/$dir/CUR
done

for dir in $SCI_DIRECTORIES
do
  mkdir /SD_3/$dir
  mkdir /SD_3/$dir/CUR
done
