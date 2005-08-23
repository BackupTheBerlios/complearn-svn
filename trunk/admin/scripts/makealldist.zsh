#!/bin/zsh

CLLOC=$HOME/src/complearn

cd $CLLOC/ruby;
./scripts/reconf.zsh
make dist
cp libcomplearn-ruby1.8* /home/svn/public_html/complearn/downloads/
chown svn:svn /home/svn/public_html/complearn/downloads/libcomplearn-ruby1.8*
