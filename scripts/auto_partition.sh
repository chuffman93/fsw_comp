#!/bin/sh

######
# auto_partition.sh
# Last Modified: Jack Dinkel 12/15/16
#
# This script will format an SD card with three partitions as follows:
# 1. 512mb EXT3
# 2. 8GB   EXT3
# 3. ~6GB  EXT3
#
# The script assumes the SD card is located at /dev/mmcblk0. If this is
# not the case, simply provide the location as a command line argument.
######

# Allow optional command line argument, otherwise default to /dev/mmcblk0
if [ -z $1 ]
then
  DEVICE="/dev/mmcblk0"
else
  DEVICE=$1
fi

DISK_SIZE=`parted -s $DEVICE print | awk '/^Disk/ {print $3}' | sed 's/[Mm][Bb]//'`


make_partition()
{
  start=$1
  end=$2

  parted -s $DEVICE mkpart primary $start $end
}


# Remove all partitions
for partition in `parted -s $DEVICE print | awk '/^ / {print $1}'`
do
  parted -s $DEVICE rm $partition
done

# Create partitions
make_partition 2048s 512
make_partition 538 9128
make_partition 9128 $DISK_SIZE

# Format the new partitions
for partition in `parted -s $DEVICE print | awk '/^ / {print $1}'`
do
  mkfs.ext3 -F ${DEVICE}p$partition
done
