#!/bin/bash

SCRIPT=/tmp/jlink_script

if [ "$#" -ne 1 ]; then
	echo -e "ERROR!\nUsage: $0 <application.hex>"
	exit 1
fi

APP=$1

touch $SCRIPT

if [ ! -w $SCRIPT ]; then
	echo -e "ERROR!\nFile $SCRIPT is not writable!"
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

echo "selemu
exec device = CC2538SF53
h
r" > $SCRIPT

if [ "$BOOTLOADER" == "1" ]; then
    echo "erase" >> $SCRIPT
fi

echo "loadbin $APP,0
h
r" >> $SCRIPT

if [ "$GDB" == "1" ]; then
    echo "exit" >> $SCRIPT
else
    echo "go" >> $SCRIPT
fi

#(sleep 3 && rm $SCRIPT) & 
JLinkExe -CommanderScript $SCRIPT 

rm $SCRIPT

if [ "$GDB" == "1" ]; then
    JLinkGDBServer -device CC2538SF53 &
    KILL_THIS=$!
    sleep 1
    echo $APP
    arm-none-eabi-gdb -s ${APP%\.hex} -ex "target remote localhost:2331" -ex "mon halt" -ex "mon reset"
    #gdb-arm-elf -s ${APP%\.hex} -ex "target remote localhost:2331" -ex "mon halt" -ex "mon reset"
    kill $KILL_THIS
fi
