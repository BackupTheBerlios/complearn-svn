#!/bin/zsh
BYTES=`/usr/bin/bzip2 -c -f - | wc -c`
echo $[$BYTES * 8]
