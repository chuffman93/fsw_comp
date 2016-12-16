#bin/sh

#######################################
# build_file_system.sh
# Last Modified: Jack Dinkel 12/15/16
#
# Creates the directory structure for
# telemetry and science data on CDH
#######################################

mkdir /SD_1 # Kernel backup partition
mkdir /SD_2 # Log Data Storage partition
mkdir /SD_3 # Science Data Storage partition

# All directories to go on /SD_2
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

# All directories to go on /SD_3
SCI_DIRECTORIES="
RAD
"

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
