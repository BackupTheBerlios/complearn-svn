#!/bin/zsh
BYTES=`/bin/bzip2 -c -f - | wc -c`
echo $[$BYTES * 8]
