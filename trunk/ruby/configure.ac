AC_INIT(src/complearn4r.c)
AC_CANONICAL_SYSTEM
AC_ARG_PROGRAM
AM_INIT_AUTOMAKE(libcomplearn-ruby1.8 , 0.5.3, cilibrar@gmail.com)
AC_PROG_CC
#AC_PROG_LIBTOOL
AC_PROG_RANLIB
AC_CHECK_PROG(MKTEMP, mktemp, mktemp)

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

if test ${fusrruby} -eq 0; then
  rubypath="ruby"
else
  rubypath=`which ruby`
fi


export CFLAGS
export LDFLAGS

( cd src ; $rubypath extconf.rb --with-complearn-lib=$cllib --with-complearn-include=$clinclude
cat <<EOF >>Makefile

distdir:
	

EOF
)

AC_CONFIG_FILES([Makefile])

AC_OUTPUT

