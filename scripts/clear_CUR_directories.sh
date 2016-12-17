#!/bin/sh

#######################################
# clear_CUR_directories.sh
# Last Modified: Jack Dinkel 12/16/16
#
# Intended to be run everytim CDH boots.
# Moves all data in CUR directories to
# their data directory.
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
  mv /SD_2/$dir/CUR/* /SD_2/$dir
done

for dir in $SCI_DIRECTORIES
do
  mv /SD_3/$dir/CUR/* /SD_3/$dir
done
