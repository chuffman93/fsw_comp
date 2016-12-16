#!/bin/sh

######
# auto_mount.sh
# Last Modified: Jack Dinkel 12/15/16
#
# This script will configure CDH to automount all partitions of an SD card.
# The SD card must be preformatted and physically inserted into CDH.
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

fstab="/etc/fstab"

# auto-mount all found partitions
for partition in `ls -d -1 /dev/** | grep "$DEVICE" | grep -Eo '[1-9]'`
do
  mkdir /SD_$partition
  mount_string="${DEVICE}p$partition  /SD_$partition  auto  defaults,sync 0 0"

  if ! grep -q "$mount_string" $fstab
  then
    echo "$mount_string" >> $fstab
  else
    echo "ERROR: ${DEVICE}p$partition is already configured"
  fi
done

# Mount everything
mount -a
