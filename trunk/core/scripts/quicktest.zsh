#!/bin/zsh
CUR=$0
SCRIPTPATH=`dirname $CUR`
COREDIR="$SCRIPTPATH/.."

cd $COREDIR
#make clean
rm distmatrix.clb
rm treefile.dot
make && ./src/ncd -b -d examples/10-mammals examples/10-mammals && \
        ./src/maketree distmatrix.clb && \
        ./src/maketree -R distmatrix.clb && \
          ruby scripts/ncdtest.rb
