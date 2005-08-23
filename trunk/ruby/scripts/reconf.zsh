#!/bin/zsh

# switch case which sets the path where the soap library are installed
EXTRAOPT=$*
if test ! -f ./configure && test -f ../configure ; then
  cd ..
fi
HOST=`hostname -f` 2>/dev/null || HOST=`hostname`
echo "Compiling for ${HOST}"
case $HOST in
  *.ins.cwi.nl)
   echo "CWI-specific config enabled."
    ;;
  "wax.rhouse.local")
    ;;
  "ns1.alcruz.com")
    RUBYLOC=/home/webuser/bin/ruby
    ;;
  "miso.rhouse.local")
   echo "miso-specific config enabled."
    ;;
  "anna-lh7zsgsv2e")
    ;;
  *)
    ;;
esac

if test ! -f ./install-sh -o ! -f ./config.sub -o ! -f ./config.guess ; then
  automake -a
fi

# set prefix to the location where you want complearn to be installed; remove
# option if you are installing system-wide
#
# set the path for the with-xml2 option to where the xml2 header files are
# located.

aclocal && autoconf && \
automake && \
./configure --with-complearn=$HOME/tmp/complearntest \
            --with-rubycmd=$RUBYLOC \
            "${EXTRAOPT}"
