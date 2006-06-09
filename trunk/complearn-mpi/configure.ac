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

AC_INIT(complearn-mpi,0.9.3,cilibrar@cilibrar.com)
AC_CONFIG_SRCDIR(src/partree.c)
AM_INIT_AUTOMAKE
AC_CANONICAL_HOST
AM_MAINTAINER_MODE
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
  LDFLAGS="$LDFLAGS -L/usr/local/lib -L/usr/lib -L/lib"
else
  echo -n
fi


AC_CHECK_PROGS(MPICC, mpicc)
  if test x$MPICC = x ; then
    AC_MSG_ERROR(Cannot find mpicc compiler; is MPI installed?)
  else
    CC=$MPICC
    CPP="$MPICC -E"
  fi


# poc: if up_mpi, then CC = MPICC

AC_CHECK_PROGS(PKGCONFIG, pkg-config)
if ! test "x$PKGCONFIG" = "x" ; then
for i in complearn gsl mpi ; do
  if $PKGCONFIG --exists $i ; then
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
  *)
  echo "Building for host: ${host}"
esac

AC_ARG_ENABLE(optimize,
[  --enable-optimize  Enable performance optimizations
],
[case "${enableval}" in
   yes) ua_optimize=true ;;
   no)  ua_optimize=false ;;
    *) AC_MSG_ERROR(bad value "${enableval}" for --enable-optimize) ;;
esac], [ua_optimize=false])

if test x$ua_optimize = xtrue ; then
  CPPFLAGS="$CPPFLAGS -DNDEBUG"
  CFLAGS="$CFLAGS -O3 -DNDEBUG"
  echo "Enabling performance optimizations."
else
  CFLAGS="$CFLAGS -g"
fi

AC_CHECK_LIB(m, pow, [LIBS="$LIBS -lm"])

fgsl_lib=false
AC_CHECK_LIB(gslcblas,cblas_sdot,
  [LIBS="-lgslcblas $LIBS"

AC_CHECK_LIB(gsl,gsl_hypot,
    [ LIBS="-lgsl $LIBS" ; fgsl_lib=true])
])

fcomplearn_lib=false
AC_CHECK_HEADERS([complearn/complearn.h], [
  AC_CHECK_LIB(complearn,clDraNew, [LIBS="-lcomplearn $LIBS"; fcomplearn_lib=true]
   ) ])
if test x$fcomplearn_lib = xfalse ; then
  echo "Sorry, cannot find complearn header or library, exitting."
  exit 1
fi
if test $fgsl_lib = false ; then
  echo "#################################################"
  echo "## Sorry, GSL is required to compile $PACKAGE ##"
  echo "#################################################"
  exit 1
fi

echo "CC             is $CC"
echo "fgsl_lib       is $fgsl_lib"
echo "fcomplearn_lib is $fcomplearn_lib"
echo "ua_optimize    is $ua_optimize"
echo "host           is $host"
echo "build          is $build"

AM_CONFIG_HEADER(src/aclconfig.h)
AC_OUTPUT([Makefile src/Makefile])
