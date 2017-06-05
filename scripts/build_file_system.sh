#bin/sh

#######################################
# build_file_system.sh
# Last Modified: Jack Dinkel 01/12/17
#
# Creates the directory structure for
# telemetry and science data on CDH
#######################################

SCRIPTS="/home/root/scripts"
FS_VARS="./filesystem.sh"

cd $SCRIPTS
source $FS_VARS
cd $OLDPWD

mkdir $SD1 # Kernel backup partition
mkdir $SD2 # Log Data Storage partition
mkdir $SD3 # Science Data Storage partition

for dir in $TLM_DIRECTORIES
do
  mkdir /SD_2/$dir
  mkdir /SD_2/$dir/CUR
done

for dir in $HST_DIRECTORIES
do
  mkdir /SD_2/HST/$dir
  mkdir /SD_2/HST/$dir/CUR
done

for dir in $SCI_DIRECTORIES
do
  mkdir /SD_3/$dir
  mkdir /SD_3/$dir/CUR
done

for dir in $HR_DIRECTORIES
do
  mkdir /home/root/$dir
done
