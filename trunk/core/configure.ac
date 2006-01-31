AC_INIT(complearn,0.8.21,cilibrar@cilibrar.com)
AC_CONFIG_SRCDIR(src/ncd.c)
AM_INIT_AUTOMAKE
AC_PROG_RANLIB
AC_CANONICAL_HOST
AM_CONFIG_HEADER(src/complearn/aclconfig.h)

# base dependencies
echo "Checking BASE DEPENDENCIES"
AC_CHECK_HEADERS([gsl/gsl_matrix.h math.h])
AC_CHECK_HEADERS([dlfcn.h])
AC_CHECK_HEADERS([bzlib.h], [
  AC_CHECK_LIB(bz2, BZ2_blockSort, [LIBS="$LIBS -lbz2"],
                     [ AC_ERROR([bzlib.h header found but no bz2 library]) ])
]
)

AC_CHECK_LIB(m, pow, [LIBS="$LIBS -lm"])

fdl_lib=false
AC_CHECK_LIB(dl, dlopen, [LIBS="$LIBS -ldl" ; fdl_lib=true])

fgsl_lib=false
AC_CHECK_LIB(gslcblas,cblas_sdot,
  [LIBS="$LIBS -lgslcblas"

AC_CHECK_LIB(gsl,gsl_hypot,
    [ LIBS="$LIBS -lgsl" ; fgsl_lib=true])
])

if test $fgsl_lib = false ; then
  echo "#################################################"
  echo "## Sorry, GSL is required to compile $PACKAGE ##"
  echo "#################################################"
  exit 1
fi

# ngd dependencies
AC_CHECK_HEADERS([])

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

AC_CHECK_HEADER([gsl/gsl_matrix.h])

AC_CHECK_LIB(SDL_ttf,TTF_Init,
    [echo "Enabling SDL_ttf support." ; GFXLIBS="$GFXLIBS -lSDL_ttf" ;
      fsdlttf_lib=true],
    [ fsdlttf_lib=false ]
)

AM_CONDITIONAL(AMCMPI, [test x$ua_mpi = xtrue])
AM_CONDITIONAL(AMCGFX, [test x$fsdlttf_lib = xtrue])

AC_SUBST(GFXLIBS)

echo "fsdlttf_lib is $fsdlttf_lib"
echo "ua_mpi      is $ua_mpi"
echo "host        is $host"
echo "target      is $target"
echo "build       is $build"

AC_OUTPUT([Makefile src/Makefile])
