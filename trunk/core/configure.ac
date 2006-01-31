AC_INIT(complearn,0.9.1,cilibrar@cilibrar.com)
AC_CONFIG_SRCDIR(src/ncd.c)
AM_INIT_AUTOMAKE
AC_PROG_RANLIB
AC_CANONICAL_HOST
AM_CONFIG_HEADER(src/complearn/aclconfig.h)
AM_CONFIG_HEADER(src/complearn/clconfig.h)

AC_ARG_ENABLE(optimize,
[  --enable-optimize  Enable performance optimizations
],
[case "${enableval}" in
   yes) ua_optimize=true ;;
   no)  ua_optimize=false ;;
    *) AC_MSG_ERROR(bad value "${enableval}" for --enable-mpi) ;;
esac], [ua_optimize=false])

if test x$ua_optimize = xtrue ; then
  CPPFLAGS="$CPPFLAGS -DNDEBUG"
  CFLAGS="$CFLAGS -O3"
  echo "Enabling performance optimizations."
else
  CFLAGS="$CFLAGS -g"
fi

# base dependencies
echo "Checking BASE DEPENDENCIES"
AC_ARG_WITH(gsl,
  [  --with-gsl=PREFIX      where gsl is installed (required for NGD)],
  [ CPPFLAGS="$CPPFLAGS -I$withval/include" ;
    LDFLAGS="$LDFLAGS -L$withval/lib"]
)
AC_CHECK_HEADERS([gsl/gsl_matrix.h math.h])
AC_CHECK_LIB(m, pow, [LIBS="$LIBS -lm"])

fgsl_lib=false
AC_CHECK_LIB(gslcblas,cblas_sdot,
  [LIBS="-lgslcblas $LIBS"

AC_CHECK_LIB(gsl,gsl_hypot,
    [ LIBS="-lgsl $LIBS" ; fgsl_lib=true])
])

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
                   [ AC_MSG_ERROR([bzlib.h header found but no bz2 library]) ])
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
  [ CPPFLAGS="$CPPFLAGS -I$withval/include/libcsoap-1.0 -I$withval/include" ;
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
    AC_CHECK_LIB(nanohttp-1.0,log_error, [LIBS="-lnanohttp-1.0 $LIBS";
                 fnanohttp_lib=true]) ])
  if test x$fnanohttp_lib = xfalse ; then
    fngd_possible=false;
  fi
fi

if test x$fngd_possible = xtrue ; then
  fcsoap_lib=false
  AC_CHECK_HEADERS([libcsoap/soap-client.h], [
    AC_CHECK_LIB(csoap-1.0,soap_client_init_args, [LIBS="-lcsoap-1.0 $LIBS";
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

# partree dependencies

AC_ARG_ENABLE(mpi,
[  --enable-mpi  Enable MPI compilation using mpicc
],
[case "${enableval}" in
   yes) ua_mpi=true ;;
   no)  ua_mpi=false ;;
    *) AC_MSG_ERROR(bad value "${enableval}" for --enable-mpi) ;;
esac], [ua_mpi=false])

# pec: CC can be set explicitly, or left unset for mpicc scan

if test x$ua_mpi = xtrue ; then
  if test x$CC = x ; then
    AC_CHECK_PROGS(MPICC, mpicc)
    if test x$MPICC = x ; then
      echo 1>2 "Error, cannot find mpicc"
      exit 1
    else
      CC=$MPICC
    fi
  else
    MPICC=$CC
  fi
else
  AC_PROG_CC
fi

# poc: if up_mpi, then CC = MPICC

# 3d demo dependencies
AC_CHECK_LIB(SDL_ttf,TTF_Init,
    [echo "Enabling SDL_ttf support." ; GFXLIBS="$GFXLIBS -lSDL_ttf" ;
      fsdlttf_lib=true],
    [ fsdlttf_lib=false ]
)

AM_CONDITIONAL(AMCMPI, [test x$ua_mpi = xtrue])
AM_CONDITIONAL(AMCGFX, [test x$fsdlttf_lib = xtrue])

AC_SUBST(NGDLIBS)
AC_SUBST(GFXLIBS)

echo "fgsl_lib      is $fgsl_lib"
echo "fbz2_lib      is $fbz2_lib"
echo "fzlib_lib     is $fzlib_lib"
echo "fxml2_lib     is $fxml2_lib"
echo "fnanohttp_lib is $fnanohttp_lib"
echo "fcsoap_lib    is $fcsoap_lib"
echo "fgdbm_lib     is $fgdbm_lib"
echo "fngd          is $fngd_possible"
echo "fdl_lib       is $fdl_lib"
echo "fsdlttf_lib   is $fsdlttf_lib"
echo "ua_optimize   is $ua_optimize"
echo "ua_mpi        is $ua_mpi"
echo "host          is $host"
echo "target        is $target"
echo "build         is $build"

AC_OUTPUT([Doxyfile Makefile src/Makefile])
