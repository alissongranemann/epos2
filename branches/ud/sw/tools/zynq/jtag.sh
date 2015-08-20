#!/bin/bash

DIR=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )
BASE_INI="$DIR"/base_xmd.ini
INI="$DIR"/xmd.ini
TMP="$DIR"/tmp

APP=$1
GDB=$2

# Check if xmd is on $PATH
hash xmd > /dev/null 2>/dev/null

if [ $? -ne 0 ] 
then
    echo xmd not found in '$PATH'
    read -n1 -r -p "Press any key to exit"
    exit 1
fi

# Running xmd -tcl will execute the commands and exit immediately closing the
# connection with GDB that's why we rely on the xmd.ini file. xmd.ini doesn't
# support arguments so we use sed to insert the arguments in it.
sed 's:APP:'"$APP"':g' $BASE_INI > $TMP

# Check if $GDB is set
if [ -e $GDB 1 ]
    then sed 's:GDB:'1':g' $TMP > $INI
else 
    sed 's:GDB:'0':g' $TMP > $INI
fi

cd $DIR
xmd

rm $INI $TMP
