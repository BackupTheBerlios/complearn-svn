# Copyright (c) 2006 Rudi Cilibrasi, Rulers of the RHouse
# All rights reserved.     cilibrar@cilibrar.com
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
#     # Redistributions of source code must retain the above copyright
#       notice, this list of conditions and the following disclaimer.
#     # Redistributions in binary form must reproduce the above copyright
#       notice, this list of conditions and the following disclaimer in the
#       documentation and/or other materials provided with the distribution.
#     # Neither the name of the RHouse nor the
#       names of its contributors may be used to endorse or promote products
#       derived from this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE RULERS AND CONTRIBUTORS "AS IS" AND ANY
# EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
# WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL THE RULERS AND CONTRIBUTORS BE LIABLE FOR ANY
# DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
# (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
# LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
# ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
# SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

AC_INIT(libcomplearn,0.9.7,cilibrar@cilibrar.com)
AC_CONFIG_SRCDIR(src/ncd.c)
AM_INIT_AUTOMAKE
AC_PROG_RANLIB
AC_CANONICAL_HOST
AC_PROG_LIBTOOL
AM_MAINTAINER_MODE
EXTRABUILTINS=
dnl
dnl release and version information
dnl
libcomplearn_major=`echo $VERSION | $AWK -F. '{print $1}'`
libcomplearn_minor=`echo $VERSION | $AWK -F. '{print $2}'`
libcomplearn_patch=`echo $VERSION | $AWK -F. '{print $3}'`
AC_SUBST(libcomplearn_major)
AC_SUBST(libcomplearn_minor)
AC_SUBST(libcomplearn_patch)

libcomplearn_release="$libcomplearn_major.$libcomplearn_minor"
libcomplearn_version=`expr $libcomplearn_major + $libcomplearn_minor`":$libcomplearn_patch:$libcomplearn_minor"
AC_SUBST(libcomplearn_release)
AC_SUBST(libcomplearn_version)

AC_DEFUN([AC_DEFINE_DIR], [
  prefix_NONE=
  exec_prefix_NONE=
  test "x$prefix" = xNONE && prefix_NONE=yes && prefix=$ac_default_prefix
  test "x$exec_prefix" = xNONE && exec_prefix_NONE=yes && exec_prefix=$prefix
dnl In Autoconf 2.60, ${datadir} refers to ${datarootdir}, which in turn
dnl refers to ${prefix}.  Thus we have to use `eval' twice.
  eval ac_define_dir="\"[$]$2\""
  eval ac_define_dir="\"$ac_define_dir\""
  AC_SUBST($1, "$ac_define_dir")
  AC_DEFINE_UNQUOTED($1, "$ac_define_dir", [$3])
  test "$prefix_NONE" && prefix=NONE
  test "$exec_prefix_NONE" && exec_prefix=NONE
])
pkgdatadir="${datadir}/${PACKAGE}"
AC_DEFINE_DIR([PKGDATADIR], [pkgdatadir], [where package data are placed])
AC_ARG_ENABLE(default-paths,
[  --disable-default-paths  Disable default /usr and /usr/local search paths
],
[case "${enableval}" in
   yes) ua_defpaths=true ;;
   no)  ua_defpaths=false ;;
    *) AC_MSG_ERROR(bad value "${enableval}" for --enable-optimize) ;;
esac], [ua_defpaths=true])

if test x$ua_defpaths = xtrue ; then
  CPPFLAGS="$CPPFLAGS -I/usr/local/include -I/usr/include"
  LDFLAGS="$LDFLAGS -L/usr/local/lib -L/usr/lib -L/lib -L/usr/X11R6/lib"
else
  echo -n
fi

#glib-2.0 gdk-pixbuf-xlib-2.0 cairo pango 
#gdk-x11-2.0 gdk-2.0 gtk+-2.0 gtk+-x11-2.0 
fgsl_lib=false

AC_CHECK_PROGS(PKGCONFIG, pkg-config)
if ! test "x$PKGCONFIG" = "x" ; then
for i in gsl libxml-2.0 nanohttp libcsoap ; do
  if $PKGCONFIG --exists $i ; then
    if test "x$i" = "xgsl" ; then fgsl_lib=true ; fi
    CPPFLAGS="$CPPFLAGS `pkg-config $i --cflags`"
    LDFLAGS="$LDFLAGS `pkg-config $i --libs`"
  else
    echo -n
  fi
done
fi

case "${host}" in
  *-*-linux*)
  AC_DEFINE(LINUX, 1, "Linux")
  ;;
  *-*-cygwin*)
    echo "Warning: Functionality limitted on this OS; text-mode distance"
    echo "matrixes are the only ones that work.  You can make these using"
    echo "ncd and > redirection of its output, as in:"
    echo "ncd -d 10-mammals 10-mammals >distmatrix.txt"
    echo "This distmatrix.txt file may then be fed to maketree like this:"
    echo "maketree distmatrix.txt"
    echo "Compiling ppmdx and lzmax modules in statically for Cygwin to"
    echo "compensate for limitted DLL functionality in this OS."
    echo "For complete dynamic module support and binary distance matrix,"
    echo "please consider using a Unix style OS"
    AC_DEFINE(STATICLOADEXTRAMODS, 1)
    EXTRABUILTINS='libppmdx.lo liblzmax.lo'
#    LDFLAGS="$LDFLAGS -lz"
    LIBS="$LIBS -lz"
if test -e /usr/lib/libgslcblas.a -a ! -e /usr/lib/libblas.a ; then
cp /usr/lib/libgslcblas.a /usr/lib/libblas.a
fi
;;
  *)
  echo "Building for host: ${host}"
esac

AC_ARG_ENABLE(dmalloc,
[  --enable-dmalloc  Enable dmalloc memory leak detection subsystem
],
[case "${enableval}" in
   yes) ua_dmalloc=true ;;
   no)  ua_dmalloc=false ;;
    *) AC_MSG_ERROR(bad value "${enableval}" for --enable-dmalloc) ;;
esac], [ua_dmalloc=false])

if test x$ua_dmalloc = xtrue ; then
  CPPFLAGS="$CPPFLAGS -DNOGALLOC"
  LDFLAGS="$LDFLAGS -ldmalloc"
  echo "Enabling dmalloc subsystem."
fi

AC_ARG_ENABLE(optimize,
[  --enable-optimize  Enable performance optimizations
],
[case "${enableval}" in
   yes) ua_optimize=true ;;
   no)  ua_optimize=false ;;
    *) AC_MSG_ERROR(bad value "${enableval}" for --enable-optimize) ;;
esac], [ua_optimize=true])

if test x$ua_optimize = xtrue ; then
  CPPFLAGS="$CPPFLAGS -DNDEBUG -Wall"
  CFLAGS="$CFLAGS -O3 -DNDEBUG -Wall"
  echo "Enabling performance optimizations."
else
  CFLAGS="$CFLAGS -g -Wall"
fi

# base dependencies
AC_CHECK_LIB(m, pow, [LIBS="$LIBS -lm"])
if test "x$fgsl_lib" = "xfalse" ; then
AC_ARG_WITH(gsl,
  [  --with-gsl=PREFIX       where gsl is installed (required for NGD)],
  [ CPPFLAGS="$CPPFLAGS -I$withval/include" ;
    LDFLAGS="$LDFLAGS -L$withval/lib"]
)
AC_CHECK_HEADERS([gsl/gsl_matrix.h math.h])

AC_CHECK_LIB(gslcblas,cblas_sdot,
  [LIBS="-lgslcblas $LIBS"

AC_CHECK_LIB(gsl,gsl_hypot,
    [ LIBS="-lgsl $LIBS" ; fgsl_lib=true])
])
fi

if test $fgsl_lib = false ; then
  echo "#################################################"
  echo "## Sorry, GSL is required to compile $PACKAGE ##"
  echo "#################################################"
  exit 1
fi

# optional dependencies
fdl_lib=false
AC_CHECK_HEADERS([dlfcn.h])
AC_CHECK_LIB(dl, dlopen, [LIBS="$LIBS -ldl" ; fdl_lib=true])

fbz2_lib=false
AC_CHECK_HEADERS([bzlib.h], [
  AC_CHECK_LIB(bz2, BZ2_blockSort, [LIBS="$LIBS -lbz2" ; fbz2_lib=true],
                   [ tail -100 config.log ; AC_MSG_ERROR([bzlib.h header found but no bz2 library]) ])
]
)

fzlib_lib=false
AC_CHECK_HEADERS([zlib.h], [
  AC_CHECK_LIB(z, compress2, [LIBS="$LIBS -lz" ; fzlib_lib=true],
                   [ AC_MSG_ERROR([zlib.h header found but no zlib library]) ])
]
)

# ngd dependencies
AC_ARG_WITH(xml2,
  [  --with-xml2=PREFIX      where xml2 is installed (required for NGD)],
  [ CPPFLAGS="$CPPFLAGS -I$withval/include/libxml2 -I$withval/include" ;
    LDFLAGS="$LDFLAGS -L$withval/lib"]
)
AC_ARG_WITH(nanohttp,
  [  --with-nanohttp=PREFIX  where nanohttp is installed (required for NGD)],
  [ CPPFLAGS="$CPPFLAGS -I$withval/include/nanohttp-1.0 -I$withval/include" ;
    LDFLAGS="$LDFLAGS -L$withval/lib"]
)
AC_ARG_WITH(csoap,
  [  --with-csoap=PREFIX     where csoap is installed (required for NGD)],
  [ CPPFLAGS="$CPPFLAGS -I$withval/include/libcsoap-1.0 -I$withval/include/nanohttp-1.0 -I$withval/include" ;
    LDFLAGS="$LDFLAGS -L$withval/lib"]
)

fngd_possible=true;
OLDLIBS=$LIBS
LIBS=""

if test x$fngd_possible = xtrue ; then
  fxml2_lib=false
  AC_CHECK_HEADERS([libxml/xpath.h], [
    AC_CHECK_LIB(xml2,xmlXIncludeProcess, [LIBS="-lxml2 $LIBS"; fxml2_lib=true]
     ) ])
  if test x$fxml2_lib = xfalse ; then
    fngd_possible=false;
  fi
fi
if test x$fngd_possible = xtrue ; then
  fnanohttp_lib=false
  AC_CHECK_HEADERS([nanohttp/nanohttp-client.h], [
    AC_CHECK_LIB(nanohttp,httpd_run, [LIBS="-lnanohttp $LIBS";
                 fnanohttp_lib=true]) ])
  if test x$fnanohttp_lib = xfalse ; then
    fngd_possible=false;
  fi
fi

if test x$fngd_possible = xtrue ; then
  fcsoap_lib=false
  AC_CHECK_HEADERS([libcsoap/soap-client.h], [
    AC_CHECK_LIB(csoap,soap_client_init_args, [LIBS="-lcsoap $LIBS";
                 fcsoap_lib=true]) ])
  if test x$fcsoap_lib = xfalse ; then
    fngd_possible=false;
  fi
fi

if test x$fngd_possible = xtrue ; then
  fgdbm_lib=false
  AC_CHECK_HEADERS([gdbm.h], [
    AC_CHECK_LIB(gdbm,gdbm_open, [LIBS="-lgdbm $LIBS";
                 fgdbm_lib=true]) ])
  if test x$fgdbm_lib = xfalse ; then
    fngd_possible=false;
  fi
fi

NGDLIBS=$LIBS
LIBS=$OLDLIBS

AC_PROG_CC
AC_SUBST(NGDLIBS)
AC_SUBST(EXTRABUILTINS)
AC_SUBST(FONTFILE)

echo "fgsl_lib      is $fgsl_lib"
echo "fbz2_lib      is $fbz2_lib"
echo "fzlib_lib     is $fzlib_lib"
echo "fxml2_lib     is $fxml2_lib"
echo "fnanohttp_lib is $fnanohttp_lib"
echo "fcsoap_lib    is $fcsoap_lib"
echo "fgdbm_lib     is $fgdbm_lib"
echo "fngd          is $fngd_possible"
echo "fdl_lib       is $fdl_lib"
echo "ua_optimize   is $ua_optimize"
echo "host          is $host"
echo "build         is $build"
if test "x$EXTRABUILTINS" = x ; then
  echo -n
else
  echo
  echo "*********************************************************************"
  echo
  echo "Warning: To compile in Cygwin, you must use the following commands:"
  echo "cd src"
  echo "make -f winMakefile"
  echo "This will compile static versions of ncd.exe and maketree.exe for you."
  echo "It will be your task to copy these to a better location."
  echo
  echo "*********************************************************************"
  echo
fi

AM_CONFIG_HEADER(src/complearn/aclconfig.h)
AM_CONFIG_HEADER(src/complearn/clconfig.h)
AC_OUTPUT([complearn.pc Doxyfile Makefile src/Makefile])
