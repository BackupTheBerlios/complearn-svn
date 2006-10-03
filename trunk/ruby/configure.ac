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

AC_INIT(src/complearn4r.c)
AC_CANONICAL_SYSTEM
AC_ARG_PROGRAM
AM_INIT_AUTOMAKE(libcomplearn-ruby, 0.9.7, cilibrar@cilibrar.com)
AC_PROG_CC
#AC_PROG_LIBTOOL
AC_PROG_RANLIB
AC_CHECK_PROG(MKTEMP, mktemp, mktemp)
AC_CHECK_PROG(RUBY, ruby, ruby)

if test -z "$RUBY"; then
  AC_CHECK_PROG(RUBY, ruby1.8, ruby1.8)
fi

if test -z "$RUBY"; then
  echo "Cannot find ruby nor ruby1.8, exitting..."
  exit 1
fi

AC_CHECK_PROGS(PKGCONFIG, pkg-config)
if ! test "x$PKGCONFIG" = "x" ; then
for i in complearn gsl ; do
  if $PKGCONFIG --exists $i ; then
    CPPFLAGS="$CPPFLAGS `pkg-config $i --cflags`"
    LDFLAGS="$LDFLAGS `pkg-config $i --libs`"
  else
    echo -n
  fi
done
fi

CURTMPDIR=`mktemp -d`

case "${target}" in
  *-*-cygwin*)
  echo "Building for Cygwin : ${target}"
  CFLAGS="$CFLAGS -DWINCHOICE -I /usr/local/include -I /usr/local/include/SDL"
  CPPFLAGS="$CFLAGS"
  LDFLAGS="-L/lib -L/usr/local/lib"
#  LIBS="$LIBS -lgdi32 -lkernel32 -luser32"
;;
  *-*-mingw32*)
  echo "Building for cross-compilation to MinGW : ${target}"
  btarget="i586-mingw32msvc"
  CC="${btarget}-gcc"
  CPP="${btarget}-cpp"
  LD="${btarget}-ld"
  AR="${btarget}-ar"
  # TODO: add the LIBS: -lwsock32 -luser32 (and maybe -lgcc) stuff
  RANLIB="${btarget}-ranlib"
  unset btarget
;;
  *-*-linux*)
  echo "Building for Linux"
  CPPFLAGS="$CPPFLAGS -DLINUX"
  LDFLAGS="-L/lib -L/usr/local/lib -L/usr/lib"
  AC_DEFINE(LINUX, 1, "Linux")
;;
*)
  echo "Building for target: ${target}"
esac

if test "x$CC" == "xgcc";
then
  CFLAGS="$CFLAGS -fno-strict-aliasing"
fi

fcomplearn_lib=0;      fcomplearn_h=0 ; fusrruby=0

AC_ARG_WITH(complearn,[  --with-complearn=PREFIX   where complearn is installed], [ CPPFLAGS="$CPPFLAGS -I$withval/include -I$withval/include/complearn-1.0" ; LDFLAGS="$LDFLAGS -L$withval/lib" ; clinclude="$withval/include/complearn-1.0" ; cllib="$withval/lib"])

AC_ARG_WITH(rubycmd,[  --with-rubycmd=CMD        path to ruby command], [ usrruby="$withval" ; fusrruby=1] )

AC_CHECK_LIB(complearn, main,
  echo "Found libcomplearn."
  [LIBS="$LIBS -lcomplearn" ; fcomplearn_lib=1])

AC_CHECK_HEADER([complearn/complearn.h], [fcomplearn_h=1 ; echo "Found complearn/complearn.h"])

if test ${fcomplearn_h} -eq 0; then
  echo "Error, cannot find complearn header."
  exit 1
fi

if test ${fcomplearn_lib} -eq 0; then
  echo "Error, cannot find complearn library."
  exit 1
fi

export CFLAGS
export CPPFLAGS
export LDFLAGS

( cd src ; $RUBY extconf.rb --with-complearn-lib=$cllib --with-complearn-include=$clinclude
cat <<EOF >>Makefile

distdir:
	

EOF
)

AC_CONFIG_FILES([Makefile])

AC_OUTPUT

