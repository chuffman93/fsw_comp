#!/bin/sh

#######################################
# post_flash_config.sh
# Last Modified: Jack Dinkel 01/11/17
#
# Run this script after a flash to
# automatically configure the CDH
# filesystem
#######################################

SCRIPTS="/home/root/scripts"
CRON="$SCRIPTS/add_cron.sh"
BUILD="$SCRIPTS/build_file_system.sh"
MOUNT="$SCRIPTS/auto_mount.sh"

$CRON $SCRIPTS
$BUILD
$MOUNT
