#! /usr/bin/sh

CHTOOLS="$HOME/Projects/ChibiOS-Contrib/tools"
MXPATH="$HOME/STM32CubeMX"

# To copy to the board.h part
$CHTOOLS/mx2board.py -m MXPATH -p $1 -o pins_boardconf

# Doesn't update clocks config
$CHTOOLS/mx2conf.py $1 .
