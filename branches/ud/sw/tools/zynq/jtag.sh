#!/bin/bash

DIR=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )
BASE_SCRIPT="$DIR"/xmd_script.tcl

if [ "$#" -ne 1 ]; then
	echo -e "ERROR!\nUsage: $0 <application.hex>"
	exit 1
fi

APP=$1

if [ ! -f $BASE_SCRIPT ]; then
	echo -e "ERROR!\nFile $BASE_SCRIPT not found"
	exit 2
fi
if [ ! -f $APP ]; then
	echo -e "ERROR!\nFile $APP not found"
	exit 3
fi

SCRIPT="$BASE_SCRIPT"_$RANDOM

sed 's:APPLICATION_NAME:'"$APP"':g' $BASE_SCRIPT > $SCRIPT

xmd -tcl $SCRIPT 

rm $SCRIPT
