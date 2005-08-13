AC_INIT(src/ncd.c)
AC_CANONICAL_SYSTEM
AC_ARG_PROGRAM
AM_INIT_AUTOMAKE(libcomplearn , 0.8.17, cilibrar@gmail.com)
AC_PROG_CC
#AC_PROG_LIBTOOL
AC_PROG_RANLIB
AC_CHECK_PROG(MKTEMP, mktemp, mktemp)

AC_CHECK_PROG(SDLCONFIG, sdl-config, missing)

if test "x$SDLCONFIG" = "x" -o "x$SDLCONFIG" = "xmissing"; then
  echo "No sdl-config found in path"
else
  LIBS="$LIBS `$SDLCONFIG --libs`"
  CFLAGS="$CFLAGS `$SDLCONFIG --cflags`"
fi

CURTMPDIR=`mktemp -d`

DEPSTATUSNAME="$CURTMPDIR/depstat.txt"
RECSTEPNAME="$CURTMPDIR/recfile.txt"
REQSTEPNAME="$CURTMPDIR/reqfile.txt"

echo "CompLearn dependency status:" >$DEPSTATUSNAME
echo "CompLearn steps recommended:" >$RECSTEPNAME
echo "CompLearn steps required:" >$REQSTEPNAME

fbzip2_rdy=0;    fbzip2_lib=0;    fbzip2_h=0;
fsdl_rdy=0;      fsdl_lib=0;      fsdlttf_lib=0;
fsdl_h=0;        fsdlttf_h=0;     fsdlthread_h=0;
fglu_h=0;        fglut_h=0;       fwantsdlgl_prog=0;
fgl_rdy=0;       fgl_lib=0;       fgl_h=0;
fzlib_rdy=0;     fzlib_lib=0;     fzlib_h=0;
fgdbm_rdy=0;     fgdbm_lib=0;     fgdbm_h=0;
fcsoap_rdy=0;    fcsoap_lib=0;    fcsoap_h=0;
fnanohttp_rdy=0; fnanohttp_lib=0; fnanohttp_h=0;
fxml2_rdy=0;     fxml2_lib=0;     fxml2_h=0;
fgsl_rdy=0;      fgsl_lib=0;      fgsl_h=0;
fdlfcn_rdy=0;    fdlfcn_lib=0;    fdlfcn_h=0;
fgetopt_rdy=0;   fgetopt_lib=0;   fgetopt_h=0;
fmath_rdy=0;     fmath_lib=0;     fmath_h=0;
fpwd_rdy=0;      futs_rdy=0;      fsdlttf_rdy=0;
fpthread_lib=0;  fglut_lib=0;     fglu_lib=0;
fgtk_lib=0;      fgdkx11_lib=0;   fgtkx11_lib=0;
fgtk_h=0;        fgdk_h=0;        fpango_h=0;
fpango_lib=0;    fglib_lib=0;     fglib_h=0;
fatk_lib=0;      fatk_h=0;        fgtk_rdy=0;

ac_dmalloc=0;
ac_optimize=0;
ac_profile=0;
ac_nogalloc=0;

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

AC_ARG_WITH(bzip2,[--with-bzip2=PREFIX   where bzip2 is installed], [ CPPFLAGS="$CPPFLAGS -I$withval/include" ; LDFLAGS="$LDFLAGS -L$withval/lib"])
AC_ARG_WITH(zlib,[--with-zlib=PREFIX   where zlib is installed], [ CPPFLAGS="$CPPFLAGS -I$withval/include" ; LDFLAGS="$LDFLAGS -L$withval/lib"])
AC_ARG_WITH(xml2,[--with-xml2=PREFIX   where xml2 is installed], [ CPPFLAGS="$CPPFLAGS -I$withval/include/libxml2" ; LDFLAGS="$LDFLAGS -L$withval/lib"])
AC_ARG_WITH(csoap,[--with-csoap=PREFIX   where csoap is installed], [ CPPFLAGS="$CPPFLAGS -I$withval/include/libcsoap-1.0 -I$withval/include/nanohttp-1.0" ; LDFLAGS="$LDFLAGS -L$withval/lib"])
AC_ARG_WITH(gtk,[--with-gtk=PREFIX   where gtk+ (version 2.0) is installed], [ CPPFLAGS="$CPPFLAGS -I$withval/include/gtk-2.0 -I$withval/include/glib-2.0 -I$withval/lib/glib-2.0/include -I$withval/include/pango-1.0 -I$withval/lib/gtk-2.0/include -I$withval/include/atk-1.0" ; LDFLAGS="$LDFLAGS -L$withval/lib"])
AC_ARG_WITH(nanohttp,[--with-nanohttp=PREFIX   where nanohttp is installed], [ CPPFLAGS="$CPPFLAGS -I$withval/include/nanohttp-1.0" ; LDFLAGS="$LDFLAGS -L$withval/lib"])
AC_ARG_WITH(gsl,[--with-gsl=PREFIX   where gsl is installed], [ CPPFLAGS="$CPPFLAGS -I$withval/include" ; LDFLAGS="$LDFLAGS -L$withval/lib"])
AC_ARG_WITH(gdbm,[--with-gdbm=PREFIX   where gdbm is installed], [ CPPFLAGS="$CPPFLAGS -I$withval/include" ; LDFLAGS="$LDFLAGS -L$withval/lib"])
AC_ARG_WITH(sdl,[--with-sdl=PREFIX   where SDL is installed], [ CPPFLAGS="$CPPFLAGS -I$withval/include" ; LDFLAGS="$LDFLAGS -L$withval/lib"])
AC_ARG_WITH(sdl_ttf,[--with-sdl_ttf=PREFIX   where SDL_ttf is installed], [ CPPFLAGS="$CPPFLAGS -I$withval/include" ; LDFLAGS="$LDFLAGS -L$withval/lib"])
AC_ARG_WITH(glut,[--with-glut=PREFIX   where SDL_ttf is installed], [ CPPFLAGS="$CPPFLAGS -I$withval/include" ; LDFLAGS="$LDFLAGS -L$withval/lib"])

dnl --enable-nogalloc
AC_ARG_ENABLE(nogalloc,
  AC_HELP_STRING([--enable-nogalloc],
    [Enable nogalloc. Diagnostic use only.]),
  [ac_nogalloc=1])

dnl --enable-profiling
AC_ARG_ENABLE(profiling,
  AC_HELP_STRING([--enable-profiling],
    [Enable profiling. Diagnostic use only.]),
  [ac_profile=1])

dnl --enable-optimize
AC_ARG_ENABLE(optimize,
  AC_HELP_STRING([--enable-optimize],
    [Enable performance optimizations.]),
  [ac_optimize=1])

dnl --enable-dmalloc
AC_ARG_ENABLE(dmalloc,
  AC_HELP_STRING([--enable-dmalloc],
    [Enable dmalloc. Diagnostic use only.]),
  [ac_dmalloc=1])

AC_ARG_ENABLE(sdl,
  [  --enable-sdl            enable SDL support (yes)], , enable_sdl=yes)
if test "x$enable_sdl" == "xyes";
then
  AC_CHECK_LIB(SDL,main,
    echo "Enabling SDL support."
    [LIBS="$LIBS -lSDL" ; fsdl_lib=1])
fi

AC_ARG_ENABLE(sdl_ttf,
  [  --enable-sdl_ttf            enable SDL_ttf support (yes)], , enable_sdl_ttf=yes)
if test "x$enable_sdl_ttf" == "xyes";
then
  AC_CHECK_LIB(SDL_ttf,TTF_Init,
    echo "Enabling SDL_ttf support."
    [LIBS="$LIBS -lSDL_ttf" ; fsdlttf_lib=1])
fi
AC_ARG_ENABLE(xml2,
  [  --enable-xml2            enable xml2 support (yes)], , enable_xml2=yes)
if test "x$enable_xml2" == "xyes";
then
  AC_CHECK_LIB(xml2,xmlXIncludeProcess,
    echo "Enabling xml2 support."
    [LIBS="$LIBS -lxml2" ; fxml2_lib=1])
fi

AC_ARG_ENABLE(nanohttp,
  [  --enable-nanohttp            enable nanohttp support (yes)], , enable_nanohttp=yes)
if test "x$enable_nanohttp" == "xyes";
then
  AC_CHECK_LIB(nanohttp-1.0,log_error,
    echo "Enabling nanohttp support."
    [ LIBS="$LIBS -lnanohttp-1.0" ; fnanohttp_lib=1])
fi

AC_ARG_ENABLE(gtk,
  [  --enable-gtk            enable gtk+ support (yes)], , enable_gtk=yes)
if test "x$enable_gtk" == "xyes";
then
#  AC_CHECK_LIB(gtk,main,
#    echo "Enabling gtk+ support."
#    [LIBS="-lgtk $LIBS" ; fgtk_lib=1])
  AC_CHECK_LIB(gtk-x11-2.0,main,
    echo "Enabling gtk-x11-2.0 support."
    [LIBS="$LIBS -lgtk-x11-2.0" ; fgtkx11_lib=1])
  AC_CHECK_LIB(gdk-x11-2.0,main,
    echo "Enabling gdk-x11-2.0 support."
    [LIBS="$LIBS -lgdk-x11-2.0" ; fgdkx11_lib=1])
  AC_CHECK_LIB(glib-2.0,g_main_loop_new,
    echo "Enabling glib support support."
    [LIBS="$LIBS -lglib-2.0" ; fglib_lib=1])
  AC_CHECK_LIB(pango-1.0,pango_item_new,
    echo "Enabling pango support support."
    [LIBS="$LIBS -lpango-1.0" ; fpango_lib=1])
  AC_CHECK_LIB(atk-1.0,atk_action_do_action,
    echo "Enabling atk support support."
    [LIBS="$LIBS -latk-1.0" ; fatk_lib=1])
fi

AC_ARG_ENABLE(csoap,
  [  --enable-csoap            enable csoap support (yes)], , enable_csoap=yes)
if test "x$enable_csoap" == "xyes";
then
  AC_CHECK_LIB(csoap-1.0,soap_client_init_args,
    echo "Enabling csoap support."
    [LIBS="-lcsoap-1.0 $LIBS" ; fcsoap_lib=1])
fi

AC_ARG_ENABLE(gsl,
  [  --enable-gsl            enable gsl support (yes)], , enable_gsl=yes)
if test "x$enable_gsl" == "xyes";
then
  AC_CHECK_LIB(gslcblas,cblas_srot,
    [ LIBS="$LIBS -lgslcblas" ;
    AC_CHECK_LIB(gsl,gsl_eigen_symm_alloc,
      echo "Enabling GSL support."
      [ LIBS="$LIBS -lgsl -lgslcblas" ; fgsl_lib=1]) ]
  )
fi

AC_ARG_ENABLE(bzip2,
  [  --enable-bzip2            enable bzip2 (yes)], , enable_bzip2=yes)
if test "x$enable_bzip2" == "xyes";
then
  AC_CHECK_LIB(bz2,BZ2_bzBuffToBuffCompress,
    echo "Enabling bzip2 support."
    LIBS="$LIBS -lbz2" [fbzip2_lib=1],
    [fbzip2_lib=0] )
fi

AC_ARG_ENABLE(zlib,
  [  --enable-zlib            enable zlib (yes)], , enable_zlib=yes)
if test "x$enable_zlib" == "xyes";
then
  AC_CHECK_LIB(z,compress2,
    echo "Enabling zlib support."
    [LIBS="$LIBS -lz" ; fzlib_lib=1])
fi

AC_ARG_ENABLE(gdbm,
  [  --enable-gdbm            enable gdbm (yes)], , enable_gdbm=yes)
if test "x$enable_gdbm" == "xyes";
then
  AC_CHECK_LIB(gdbm,gdbm_open,
    echo "Enabling gdbm support."
    [LIBS="$LIBS -lgdbm" ; fgdbm_lib=1])
fi

AC_ARG_ENABLE(glut,
  [  --enable-glut            enable glut (yes)], , enable_glut=yes)
if test "x$enable_gdbm" == "xyes";
then
  AC_CHECK_LIB(glut32, main,
      [LIBS="$LIBS -lglut32" ; fglut_lib=1])

  if test ${fglut_lib} -eq 0; then
    AC_CHECK_LIB(glut, main,
      [LIBS="$LIBS -lglut" ; fglut_lib=1])
  fi
fi

AC_CHECK_LIB([dl], [dlopen], [LIBS="$LIBS -ldl"] [fdlfcn_lib=1])
AC_CHECK_LIB([c],[getopt_long], [fgetopt_lib=1])

AC_STRUCT_TIMEZONE
AC_CHECK_FUNC(gettimeofday,
  [AC_DEFINE(HAVE_GETTIMEOFDAY, 1,
  [Define if the gettimeofday function is available])])
AC_CHECK_HEADER([libxml/xpath.h], [fxml2_h=1])
AC_CHECK_HEADER([gdbm.h], [fgdbm_h=1])
AC_CHECK_HEADER([gsl/gsl_matrix.h], [fgsl_h=1])
AC_CHECK_HEADER([dlfcn.h], [fdlfcn_h=1])
AC_CHECK_HEADER([getopt.h], [fgetopt_h=1])
AC_CHECK_HEADER([zlib.h], [fzlib_h=1])
AC_CHECK_HEADER([bzlib.h], [fbzip2_h=1])
AC_CHECK_HEADER([nanohttp/nanohttp-client.h], [fnanohttp_h=1])
AC_CHECK_HEADER([libcsoap/soap-client.h], [fcsoap_h=1])
AC_CHECK_HEADER([math.h], [fmath_h=1])
AC_CHECK_HEADER([pwd.h], [fpwd_h=1])
AC_CHECK_HEADER([sys/utsname.h], [futs_h=1])
AC_CHECK_HEADER([SDL/SDL.h], [fsdl_h=1])
AC_CHECK_HEADER([SDL/SDL_ttf.h], [fsdlttf_h=1])
AC_CHECK_HEADER([SDL/SDL_thread.h], [fsdlthread_h=1])
AC_CHECK_HEADER([GL/gl.h], [fgl_h=1])
AC_CHECK_HEADER([GL/glu.h], [fglu_h=1])
AC_CHECK_HEADER([GL/glut.h], [fglut_h=1])
AC_CHECK_HEADER([gtk/gtk.h], [fgtk_h=1])
AC_CHECK_HEADER([gdk/gdk.h], [fgdk_h=1])
AC_CHECK_HEADER([glib.h], [fglib_h=1])
AC_CHECK_HEADER([pango/pango.h], [fpango_h=1])
AC_CHECK_HEADER([atk/atk.h], [fatk_h=1])

CPPFLAGS="$CPPFLAGS -I/usr/local/include"
CFLAGS="$CFLAGS -I/usr/local/include"
LDFLAGS="$LDFLAGS -I/usr/local/lib"

eval "eval TOP_SRC_DIR=${top_srcdir}"
AC_SUBST(TOP_SRC_DIR)
eval "eval PREFIX_DIR=${prefix}"
AC_SUBST(PREFIX_DIR)
eval "eval EXEC_PREFIX_DIR=${exec_prefix}"
AC_SUBST(EXEC_PREFIX_DIR)
eval "eval BIN_DIR=$bindir"
AC_SUBST(BIN_DIR)
eval "eval SBIN_DIR=$sbindir"
AC_SUBST(SBIN_DIR)
eval "eval LIBEXEC_DIR=$libexecdir"
AC_SUBST(LIBEXEC_DIR)
eval "eval DATA_DIR=$datadir"
AC_SUBST(DATA_DIR)
eval "eval SYSCONF_DIR=$sysconfdir"
AC_SUBST(SYSCONF_DIR)
eval "eval SHAREDSTATE_DIR=$sharedstatedir"
AC_SUBST(SHAREDSTATE_DIR)
eval "eval LOCALSTATE_DIR=$localstatedir"
AC_SUBST(LOCALSTATE_DIR)
eval "eval LIB_DIR=$libdir"
AC_SUBST(LIB_DIR)
eval "eval INCLUDE_DIR=$includedir"
AC_SUBST(INCLUDE_DIR)
eval "eval OLDINCLUDE_DIR=$oldincludedir"
AC_SUBST(OLDINCLUDE_DIR)
eval "eval INFO_DIR=$infodir"
AC_SUBST(INFO_DIR)
eval "eval MAN_DIR=$mandir"
AC_SUBST(MAN_DIR)

if test ${ac_nogalloc} -eq 1; then
   CFLAGS="$CFLAGS -DNOGALLOC"
   AC_MSG_NOTICE([Enable profiling])
fi

if test ${ac_profile} -eq 1; then
   CFLAGS="$CFLAGS -pg"
   AC_MSG_NOTICE([Enable profiling])
fi

if test ${ac_dmalloc} -eq 1; then
   LIBS="$LIBS -ldmalloc -L/usr/lib"
   AC_CHECK_LIB(dmalloc,malloc,
    echo "Enabling libdmalloc support."
    AC_DEFINE(HAVE_DMALLOC, 1, has dmalloc)
    LIBS="$LIBS -ldmalloc",
    AC_MSG_ERROR(dmalloc not found; cannot enable debug malloc) )
fi

if test ${ac_optimize} -eq 1; then
  CPPFLAGS="$CPPFLAGS -DNDEBUG"
  CFLAGS="$CFLAGS -O2"
  echo "Enabling performance optimizations."
else
  CFLAGS="$CFLAGS -g"
fi

AC_CHECK_LIB(glu32, main,
    [LIBS="$LIBS -lglu32" ; fglu_lib=1])

AC_CHECK_LIB(pthread, main,
    [LIBS="$LIBS -lpthread" ; fpthread_lib=1])

AC_CHECK_LIB(GL, main,
    [LIBS="$LIBS -lGL" ; fgl_lib=1])

AC_CHECK_LIB(opengl32, main,
    [LIBS="$LIBS -lopengl32" ; fgl_lib=1])

AC_CHECK_LIB(GLU, main,
    [LIBS="$LIBS -lGLU" ; fglu_lib=1])

AC_CHECK_LIB(m,main,
    echo "Enabling math library (-lm) support."
    [LIBS="$LIBS -lm" ; fmath_lib=1])

#AC_MSG_NOTICE([Enable dmalloc])
CFLAGS="$CFLAGS -Wall"

if test ${fgl_lib} -eq 1 -a ${fglu_lib} -eq 1 ; then
  if test ${fglut_lib} -eq 1 -a ${fgl_h} -eq 1 ; then
    if test ${fglu_h} -eq 1 -a ${fglut_h} -eq 1 ; then
       fgl_rdy=1 ; AC_DEFINE(GL_RDY, 1, GL ready)
    fi
  fi
fi

if test ${fsdlttf_lib} -eq 1 -a ${fsdlttf_h} -eq 1; then
    fsdlttf_rdy=1 ; AC_DEFINE(SDLTTF_RDY, 1, sdl_ttf ready)
fi

if test ${fsdl_lib} -eq 1 -a ${fsdlttf_lib} -eq 1 ; then
  if test ${fsdl_h} -eq 1 -a ${fsdlttf_h} -eq 1 ; then
    if test ${fsdlthread_h} -eq 1 ; then
      fsdl_rdy=1; AC_DEFINE(SDL_RDY, 1, SDL ready)
    fi
  fi
fi

if test ${fsdl_rdy} -eq 1 -a ${fgl_rdy} -eq 1 ; then
  fwantsdlgl_prog=1
fi

AM_CONDITIONAL(WANT_SDLGLPROG, test $fwantsdlgl_prog -eq 1)

if test ${fbzip2_lib} -eq 1 -a ${fbzip2_h} -eq 1; then
    fbzip2_rdy=1 ; AC_DEFINE(BZIP2_RDY, 1, bzip2 ready)
fi

if test ${fzlib_lib} -eq 1 -a ${fzlib_h} -eq 1; then
    fzlib_rdy=1 ; AC_DEFINE(ZLIB_RDY, 1, has zlib library)
fi

if test ${fgsl_lib} -eq 1 -a ${fgsl_h} -eq 1; then
    fgsl_rdy=1 ; AC_DEFINE(GSL_RDY, 1, has gsl library)
fi

if test ${fgsl_rdy} -eq 0; then
  AC_MSG_ERROR(Can not find gsl and gslcblas libraries on your system. CompLearn will not compile. Download from http://ftp.gnu.org/gnu/gsl/gsl-1.6.tar.gz or apt-get install libgsl0-dev . Already installed?: specify prefix with --with-gsl=PREFIX)
fi

if test ${fgdbm_lib} -eq 1 -a ${fgdbm_h} -eq 1; then
    fgdbm_rdy=1 ; AC_DEFINE(GDBM_RDY, 1, has gdbm library)
fi

if test ${fxml2_lib} -eq 1 -a ${fxml2_h} -eq 1; then
    fxml2_rdy=1 ; AC_DEFINE(XML2_RDY, 1, has xml2 library)
fi

if test ${fnanohttp_lib} -eq 1 -a ${fnanohttp_h} -eq 1; then
    fnanohttp_rdy=1 ; AC_DEFINE(NANOHTTP_RDY, 1, has nanohttp library)
fi

if test ${fcsoap_lib} -eq 1 -a ${fcsoap_h} -eq 1; then
    fcsoap_rdy=1 ; AC_DEFINE(CSOAP_RDY, 1, has csoap library)
fi

if test ${fdlfcn_lib} -eq 1 -a ${fdlfcn_h} -eq 1; then
    fdlfcn_rdy=1 ; AC_DEFINE(DLFCN_RDY, 1, has dynamic library link library)
fi

if test ${fgetopt_lib} -eq 1 -a ${fgetopt_h} -eq 1; then
    fgetopt_rdy=1 ; AC_DEFINE(GETOPT_RDY, 1, has getopt library)
fi

if test ${fmath_lib} -eq 1 -a ${fmath_h} -eq 1; then
    fmath_rdy=1 ; AC_DEFINE(MATH_RDY, 1, has math library)
fi

if test ${fpwd_h} -eq 1; then
    fpwd_rdy=1 ; AC_DEFINE(PWD_RDY, 1, has pwd.h)
fi

if test ${futs_h} -eq 1; then
    futs_rdy=1 ; AC_DEFINE(UTS_RDY, 1, has sys/utsname.h)
fi

if test ${fgtkx11_lib} -eq 1 -a ${fgtk_h} -eq 1; then
  if test ${fgdkx11_lib} -eq 1 -a ${fgdk_h} -eq 1; then
    if test ${fglib_lib} -eq 1 -a ${fglib_h} -eq 1; then
      if test ${fpango_lib} -eq 1 -a ${fpango_h} -eq 1; then
        if test ${fatk_lib} -eq 1 -a ${fatk_h} -eq 1; then
          fgtk_rdy=1 ; AC_DEFINE(GTK_RDY, 1, has gtk library)
        fi
      fi
    fi
  fi
fi

AM_CONFIG_HEADER(src/complearn/aclconfig.h)
AC_CONFIG_FILES([Doxyfile Makefile src/Makefile examples/Makefile examples/dltest/Makefile src/complearn/clconfig.h])
AC_OUTPUT

if test ${fxml2_rdy} -eq 0; then
  echo 'Warning: xml2 not found, csoap dependency
    Download from http://packages.debian.org/unstable/libdevel/libxml2-dev
    or apt-get install libxml2-dev
    Already installed?: specify prefix with --with-xml2=PREFIX' >>$RECSTEPNAME
  echo "xml2               NO" >>$DEPSTATUSNAME
else
  echo "xml2              YES" >>$DEPSTATUSNAME
fi

if test ${fnanohttp_rdy} -eq 0; then
  echo 'Warning: nanohttp not found; included with csoap bundle
    Download from http://sourceforge.net/project/showfiles.php?group_id=74977
    Already installed?: specify prefix with --with-nanohttp=PREFIX' >>$RECSTEPNAME
  echo "nanohttp           NO" >>$DEPSTATUSNAME
else
  echo "nanohttp          YES" >>$DEPSTATUSNAME
fi

if test ${fcsoap_rdy} -eq 0; then
  echo 'Warning: csoap not found; Google and soap adaptor dependency
    Download from http://sourceforge.net/project/showfiles.php?group_id=74977
    Already installed?: specify prefix with  --with-csoap=PREFIX' >>$RECSTEPNAME
  echo "csoap              NO (NGD disabled)" >>$DEPSTATUSNAME
else
  echo "csoap             YES" >>$DEPSTATUSNAME
fi

if test ${fgdbm_rdy} -eq 0; then
  echo 'Warning: gdbm library not found; google caching dependency
  Download from ftp://ftp.gnu.org/gnu/gdbm/ or
  apt-get install libgdbm-dev
  Already installed?: specify prefix with --with-gdbm=PREFIX' >>$RECSTEPNAME
  echo "gdbm               NO" >>$DEPSTATUSNAME
else
  echo "gdbm              YES" >>$DEPSTATUSNAME
fi

if test ${fzlib_rdy} -eq 0; then
  echo 'Warning: zlib library not found; builtin compression library
    Download from http://www.zlib.net/ or apt-get install zlib1g-dev
    Already installed?: specify prefix with --with-zlib=PREFIX' >>$RECSTEPNAME
  echo "zlib               NO" >>$DEPSTATUSNAME
else
  echo "zlib              YES" >>$DEPSTATUSNAME
fi

if test ${fbzip2_rdy} -eq 0; then
  echo 'Warning: bzip2 library not found; builtin compression library
    Download from http://www.bzip.org/downloads.html or
    apt-get install libbz2-dev
    Already installed?: specify prefix with --with-bzip2=PREFIX' >>$RECSTEPNAME
  echo "bzip2              NO" >>$DEPSTATUSNAME
else
  echo "bzip2             YES" >>$DEPSTATUSNAME
fi

if test ${fgsl_rdy} -eq 0; then
  echo 'Warning: gsl and gslcblas libraries not found; matrix support
    Download from http://ftp.gnu.org/gnu/gsl/ or
    apt-get install libgsl0-dev
    Already installed?: specify prefix with --with-gsl=PREFIX' >>$REQSTEPNAME
  echo "gsl                NO (required; CompLearn will not compile)" >>$DEPSTATUSNAME
else
  echo "gsl               YES" >>$DEPSTATUSNAME
fi

if test ${fdlfcn_rdy} -eq 0; then
  echo "dynamic linking    NO" >>$DEPSTATUSNAME
else
  echo "dynamic linking   YES" >>$DEPSTATUSNAME
fi

if test ${fgetopt_rdy} -eq 0; then
  echo 'Warning: getopt_long library not found; long options support
    Download from http://ftp.gnu.org/gnu/glibc/glibc-2.3.tar.gz or
    apt-get install libc6-dev' >> $REQSTEPNAME
  echo "getopt_long        NO" >>$DEPSTATUSNAME
else
  echo "getopt_long       YES" >>$DEPSTATUSNAME
fi

if test ${fsdl_lib} -eq 0; then
  echo 'Warning: SDL library not found; 3D GUI sdlgl disabled
    Download SDL from http://www.libsdl.org/
    apt-get install libsdl1.2-dev' >> $RECSTEPNAME
  echo "SDL library        NO" >>$DEPSTATUSNAME
else
  echo "SDL library       YES" >>$DEPSTATUSNAME
fi

if test ${fsdlttf_rdy} -eq 0; then
  echo 'Warning: SDL_ttf library not found; 3D GUI sdlgl disabled
    Download from http://www.libsdl.org/projects/SDL_ttf/
    apt-get install libsdl-ttf2.0-dev' >> $RECSTEPNAME
  echo "SDL_ttf library    NO" >>$DEPSTATUSNAME
else
  echo "SDL_ttf library   YES" >>$DEPSTATUSNAME
fi

if test ${fmath_rdy} -eq 0; then
  echo "math library       NO" >>$DEPSTATUSNAME
else
  echo "math library      YES" >>$DEPSTATUSNAME
fi

if test ${fpthread_lib} -eq 0; then
  echo "pthread library    NO" >>$DEPSTATUSNAME
else
  echo "pthread library   YES" >>$DEPSTATUSNAME
fi

if test ${fgtk_rdy} -eq 0; then
  echo "gtk library        NO" >>$DEPSTATUSNAME
else
  echo "gtk library       YES" >>$DEPSTATUSNAME
fi

#if test ${fgdkx11_lib} -eq 0; then
#  echo "gdk library        NO" >>$DEPSTATUSNAME
#else
#  echo "gdk library       YES" >>$DEPSTATUSNAME
#fi

if test ${fgl_rdy} -eq 0; then
  echo "GL lib             NO" >>$DEPSTATUSNAME
else
  echo "GL lib            YES" >>$DEPSTATUSNAME
fi

if test ${fwantsdlgl_prog} -eq 0; then
  echo "3d GUI             NO" >>$DEPSTATUSNAME
else
  echo "3d GUI            YES" >>$DEPSTATUSNAME
fi

cat $RECSTEPNAME
cat $REQSTEPNAME
cat $DEPSTATUSNAME
