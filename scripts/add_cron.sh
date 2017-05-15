#!/bin/sh

add_cron() {
  job=$1

  if sudo -S crontab -l 2>/dev/null | grep -qF "$job"
  then
    echo "The specified job is already implemented: $job"
  else
    (sudo -S crontab -l 2>/dev/null; echo "$job") | sudo -S crontab -
  fi
}

if [ -e $1 ]
then
  scripts="/home/root/scripts"
else
  scripts=$1
fi

add_cron "@reboot $scripts/build_file_system.sh >/dev/null"
add_cron "@reboot $scripts/clear_CUR_directories.sh >/dev/null"
add_cron "@reboot $scripts/update_time.sh >/dev/null"
