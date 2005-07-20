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
   SOAPREFIX=/opt/soft1/local
   XMLPREFIX=/usr
   GSLPREFIX=/usr
   BZ2PREFIX=/usr
   ZLIBPREFIX=/usr
   GDBMPREFIX=/usr
   GTKPREFIX=/usr
   SDLPREFIX=/ufs/alcruz/mySoftware/SDL-1.2.8
   SDLTTFPREFIX=/ufs/alcruz/mySoftware/SDL_ttf-2.0.7
    ;;
  "wax.rhouse.local")
   SOAPREFIX=/usr/local
   XMLPREFIX=/usr
   GSLPREFIX=/usr
   BZ2PREFIX=/usr
   ZLIBPREFIX=/usr
   GDBMPREFIX=/usr
   GTKPREFIX=/usr
   SDLPREFIX=/usr
   SDLTTFPREFIX=/usr
    ;;
  "ns1.alcruz.com")
   SOAPREFIX=/home/webuser/mySoftware/csoap-1.0.3
   XMLPREFIX=/usr
   GSLPREFIX=/usr
   BZ2PREFIX=/usr
   ZLIBPREFIX=/usr
   GDBMPREFIX=/usr
   GTKPREFIX=/usr
   SDLPREFIX=/usr
   SDLTTFPREFIX=/usr
    ;;
  "miso")
   SOAPREFIX=/home/alcruz/mySoftware/csoap
   XMLPREFIX=/home/alcruz/mySoftware/xml2
   GSLPREFIX=/home/alcruz/mySoftware/gsl
   BZ2PREFIX=/home/alcruz/mySoftware/bzip2
   ZLIBPREFIX=/home/alcruz/mySoftware/zlib
   GDBMPREFIX=/home/alcruz/mySoftware/gdbm
   GTKPREFIX=/home/alcruz/mySoftware/gtk
   SDLPREFIX=/usr
   SDLTTFPREFIX=/usr
    ;;
  "anna-lh7zsgsv2e")
   SOAPREFIX=/usr/local
   XMLPREFIX=/usr
   GSLPREFIX=/usr/local
   BZ2PREFIX=/usr
   ZLIBPREFIX=/usr
   GDBMPREFIX=/usr
   GTKPREFIX=/usr
   SDLPREFIX=/usr
   SDLTTFPREFIX=/usr
    ;;
  *)
   SOAPREFIX=/usr/local
   XMLPREFIX=/usr
   GSLPREFIX=/usr
   BZ2PREFIX=/usr
   ZLIBPREFIX=/usr
   GDBMPREFIX=/usr
   GTKPREFIX=/usr
   SDLPREFIX=/usr
   SDLTTFPREFIX=/usr
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

if test ! -f ./src/complearn/aclconfig.h.in ; then
  autoheader
fi && \

automake && \
./configure --prefix=$HOME/tmp/complearntest \
            --with-csoap=$SOAPREFIX \
            --with-nanohttp=$SOAPREFIX \
            --with-xml2=$XMLPREFIX \
            --with-gtk=$GTKPREFIX \
            --with-sdl=$SDLPREFIX \
            --with-sdl_ttf=$SDLTTFPREFIX \
            "${EXTRAOPT}"
#            --with-zlib=$ZLIBPREFIX \
#            --with-bzip2=$BZ2PREFIX \
#            --with-gsl=$GSLPREFIX \
#            --with-gdbm=$GDBMPREFIX \
