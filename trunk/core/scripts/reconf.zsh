#!/bin/zsh

# switch case which sets the path where the soap library are installed
EXTRAOPT=($*)
BIGOPT=""
if test ! -f ./configure && test -f ../configure ; then
  cd ..
fi
HOST=`hostname -f` 2>/dev/null || HOST=`hostname`
echo "Compiling for ${HOST}"

if test ! -f ./install-sh -o ! -f ./config.sub -o ! -f ./config.guess ; then
  automake -a
fi

# set prefix to the location where you want complearn to be installed; remove
# option if you are installing system-wide
#
# set the path for the with-xml2 option to where the xml2 header files are
# located.

rm -rf $HOME/tmp/complearntest
if test -f Makefile ; then
  make clean
fi
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
            --with-gsl=$GSLPREFIX "${BIGOPT}" \
            ${EXTRAOPT}
#            --with-zlib=$ZLIBPREFIX \
#            --with-bzip2=$BZ2PREFIX \
#            --with-gdbm=$GDBMPREFIX \
