
AC_INIT(libcomplearn,0.9.9,cilibrar@cilibrar.com)
AC_CONFIG_SRCDIR(src/ncd.c)
AC_GNU_SOURCE
AM_INIT_AUTOMAKE
AC_CANONICAL_HOST
AC_PROG_LIBTOOL
AM_PROG_CC_C_O
PKG_CHECK_MODULES([CHECK], [check >= 0.9.4])
AM_MAINTAINER_MODE

AC_PATH_PROG(GOB2, gob2, "")
if test x$GOB2 = x ; then
    AC_MSG_ERROR(must have gob2 >= 2.0.0 installed)
fi
AM_PATH_GSL(0.8.0, [
  CFLAGS="$CFLAGS $GSL_CFLAGS"
  LIBS="$LIBS $GSL_LIBS"
  ], [
    AC_MSG_ERROR(must have gsl >= 0.8.0 installed)
] )

AM_PATH_XML2(2.4.0, [
  CPPFLAGS="$CPPFLAGS $XML_CPPFLAGS"
  LIBS="$LIBS $XML_LIBS"
], [
    AC_MSG_ERROR(must have lixbml2 >= 2.4.0 installed)
] )

AM_PATH_GLIB_2_0(2.0.0, [
  CFLAGS="$CFLAGS $GLIB_CFLAGS"
  LIBS="$LIBS $GLIB_LIBS"
  ], [
    AC_MSG_ERROR(must have glib >= 2.0.0 and gobject installed)
] , [ gobject gmodule ] )

CHECK_ZLIB()

if test x$zlib_cv_libz = xno -o x$zlib_cv_zlib_h = xno ; then
    AC_MSG_ERROR(must have libz (zlib) installed)
fi

AC_CHECK_HEADERS([bzlib.h], [
  AC_CHECK_LIB(bz2, BZ2_bzBuffToBuffCompress, [LIBS="$LIBS -lbz2"
                                               fbz2_lib=true],
               [ AC_MSG_ERROR([bzlib.h header found but no bz2 library]) ]) ])

AC_CHECK_HEADERS([malloc.h])
AC_CHECK_HEADERS([errno.h])

PRIVATELIBS="$LIBS"

SONAMEOPTS="-Wl,-soname -Wl,libcomplearn-1.0${shrext_cmds}.0"
libcomplearn_major=`echo $VERSION | $AWK -F. '{print $1}'`
libcomplearn_minor=`echo $VERSION | $AWK -F. '{print $2}'`
libcomplearn_patch=`echo $VERSION | $AWK -F. '{print $3}'`
libcomplearn_release="$libcomplearn_major.$libcomplearn_minor"
libcomplearn_version=`expr $libcomplearn_major + $libcomplearn_minor`":$libcomplearn_patch:$libcomplearn_minor"

AC_ARG_ENABLE(rpath-link,
[  --enable-rpath-link  Enable rpath-link to support custom library development
],
[case "${enableval}" in
   yes) ua_rpath=true ;;
   no) ua_rpath=false ;;
    *) AC_MSG_ERROR(bad value "${enableval}" for --enable-rpath-link) ;;
esac], [ua_rpath=false])
AM_CONDITIONAL([WANT_RPATH], [test x$ua_rpath = xtrue])

SET_SONAME_VERSIONS()

AC_SUBST(PRIVATELIBS)
AC_SUBST(SONAMEOPTS)
AC_SUBST(GOB2)
AM_GNU_GETTEXT([external])
AM_GNU_GETTEXT_VERSION([0.16.1])
AM_CONFIG_HEADER(src/complearn/aclconfig.h)
AM_CONFIG_HEADER(src/complearn/clconfig.h)

AC_OUTPUT([complearn.pc Makefile src/Makefile tests/Makefile src/complearn-config.c  po/Makefile.in])
