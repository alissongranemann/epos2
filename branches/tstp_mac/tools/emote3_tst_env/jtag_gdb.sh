#!/bin/bash

DIR=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )
BASE_SCRIPT="$DIR"/jlink_script_gdb

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

if [ "$(echo -n $APP | tail -c 4)" != ".hex" ]; then
	echo -e "ERROR!\nApplication name must end with \".hex\""
	exit 4
fi

SCRIPT="$BASE_SCRIPT"_$RANDOM

sed 's:APPLICATION_NAME:'"$APP"':g' $BASE_SCRIPT > $SCRIPT

#(sleep 3 && rm $SCRIPT) & 
JLinkExe -CommanderScript $SCRIPT 
rm $SCRIPT
#xterm -e "JLinkGDBServer -device CC2538SF53" &
JLinkGDBServer -device CC2538SF53 &
KILL_THIS=$!
sleep 1
echo $APP
echo $APP
arm-none-eabi-gdb -s ${APP%\.hex} -ex "target remote localhost:2331" -ex "mon halt" -ex "mon reset"
#gdb-arm-elf -s ${APP%\.hex} -ex "target remote localhost:2331" -ex "mon halt" -ex "mon reset"
kill $KILL_THIS
