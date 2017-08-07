#!/bin/sh

#######################################
# filesystem.sh
# Last Modified: Jack Dinkel 01/11/17
#
# Variables for the file system
#######################################

export SD1="/SD_1" # Kernel backup partition
export SD2="/SD_2" # Log Data Storage partition
export SD3="/SD_3" # Science Data Storage partition

# All directories to go on /SD_2
export TLM_DIRECTORIES="

CMD
ERR
GEN
HST
MOD
PWR
SPI
SWP
THM
"

# All health and status directories
export HST_DIRECTORIES="
ACS
CDH
COM
EPS
PLD
"

# All directories to go on /SD_3
export SCI_DIRECTORIES="
RAD
"

# All directories to go in /home/root
export HR_DIRECTORIES="
.config
.downlink
.immed
.vhs
.uplink
"
