#!/bin/sh

#############################################
# insert_tlm.sh
#
# Last modified: Jack Dinkel 01/10/17
#
# This script will insert a new tlm value
# into the dictionary called dict.txt in
# the proper order
#
#############################################

echo -n "Enter a hex code of format 0x00: "
read hex
echo -n "Enter the tlm type: "
read typ
echo -n "Enter the location of the tlm in software: "
read loc
echo -n "Enter the format string of the tlm: "
read fmt

csplit dict.txt '/^\^\^\^\^\^\^\ BEGIN TELEMETRY\ \^\^\^\^\^\^/' > /dev/null
    
cat xx01 | awk -v hex=$hex -v typ=$typ -v loc=$loc -v fmt=$fmt '
done=0
/^#/ {print;}

/^0x/ {
  if ($1 < hex) {
    print $0; for(i=1; i<=4; i++) {getline; print}
    print done;
  }
  else if (done == 1){
    print "done!"
    print done;
  }
  else {
    print hex "\n" typ "\n" loc "\n" fmt "\n";
    done=1;
    print "setting done";
    print done;
  }
}
{print "other";}

'
