AC_INIT(rtest,1.0,cilibrar@cilibrar.com)
AC_CONFIG_SRCDIR(src/ncd.c)
AM_INIT_AUTOMAKE
AC_CANONICAL_HOST
AC_ARG_ENABLE(mpi,
[  --enable-mpi  Enable MPI compilation using mpicc
],
[case "${enableval}" in
   yes) ua_mpi=true ;;
   no)  ua_mpi=false ;;
    *) AC_MSG_ERROR(bad value "${enableval}" for --enable-mpi) ;;
esac], [ua_mpi=false])

if test x$ua_mpi = xtrue ; then
  AC_CHECK_PROGS(MPICC, mpicc)
  if test x$MPICC = x ; then
    echo 1>2 "Error, cannot find mpicc"
    exit 1
  else
    CC=$MPICC
  fi
fi

AC_PROG_CC
AC_CHECK_LIB(SDL_ttf,TTF_Init,
    [echo "Enabling SDL_ttf support." ; GFXLIBS="$LIBS -lSDL_ttf" ; 
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
