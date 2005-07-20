#!/usr/bin/zsh

DEMOVER=$1
DEMOEXE="/cygdrive/c/Program Files/NSIS/makensis.exe"
CURDIR=`dirname $0`
SCRIPT="$CURDIR/complearndemo.nsi"

$DEMOEXE $SCRIPT

CLDEMO=`ls $CURDIR/cldemo-$DEMOVER`
scp -v $CLDEMO svn@asiaquake.org:public_html/complearn/downloads
