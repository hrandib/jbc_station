#! /usr/bin/sh

CHTOOLS="$HOME/Projects/ChibiOS-Contrib/tools"
MXPATH="$HOME/STM32CubeMX"

$CHTOOLS/mx2board.py -m MXPATH -p $1 -o pins_boardconf

