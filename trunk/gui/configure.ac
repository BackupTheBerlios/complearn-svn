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

AC_INIT(complearn-gui,0.9.7,cilibrar@cilibrar.com)
AC_CONFIG_SRCDIR(src/clgui.c)
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
  LDFLAGS="$LDFLAGS -L/usr/local/lib -L/usr/lib -L/lib -L/usr/X11R6/lib"
else
  echo -n
fi

fcomplearn_lib=false
fgsl_lib=false

AC_PROG_CC()
AC_CHECK_PROGS(PKGCONFIG, pkg-config)
if ! test "x$PKGCONFIG" = "x" ; then
echo "Consulting pkg-config database"
for i in complearn gsl glib-2.0 gdk-pixbuf-xlib-2.0 cairo pango gdk-x11-2.0 gdk-2.0 gtk+-2.0 gtk+-x11-2.0 ; do
  if $PKGCONFIG --exists $i ; then
    if test x$i = xcomplearn ; then fcomplearn_lib=true ; fi
    if test x$i = xgsl ; then fgsl_lib=true ; fi
    CPPFLAGS="$CPPFLAGS `pkg-config $i --cflags`"
    LDFLAGS="$LDFLAGS `pkg-config $i --libs`"
  else
    echo -n
  fi
done
echo "CPPFLAGS=$CPPFLAGS"
echo "LDFLAGS=$LDFLAGS"
echo "Done with pkg-config database"
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
esac], [ua_optimize=true])

if test x$ua_optimize = xtrue ; then
  CPPFLAGS="$CPPFLAGS -DNDEBUG"
  CFLAGS="$CFLAGS -O3 -DNDEBUG"
  echo "Enabling performance optimizations."
else
  CFLAGS="$CFLAGS -g"
fi

AC_CHECK_LIB(m, pow, [LIBS="$LIBS -lm"])

if test x$fgsl_lib = xfalse ; then
AC_CHECK_LIB(gslcblas,cblas_sdot,
  [LIBS="-lgslcblas $LIBS"

AC_CHECK_LIB(gsl,gsl_hypot,
    [ LIBS="-lgsl $LIBS" ; fgsl_lib=true])
])
fi

echo "fcomplearn_lib is $fcomplearn_lib"
if test x$fcomplearn_lib = xfalse ; then
AC_CHECK_HEADERS([complearn/complearn.h], [
  AC_CHECK_LIB(complearn,clDraNew, [LIBS="-lcomplearn $LIBS"; fcomplearn_lib=true]
   ) ])
fi

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

# 3d demo dependencies
  AC_ARG_WITH(gtk,
    [  --with-gtk=PREFIX       where gtk+ (version 2.0) is installed],
    [ CPPFLAGS="$CPPFLAGS -I$withval/include/gtk-2.0 -I$withval/include/glib-2.0 -I$withval/lib/glib-2.0/include -I$withval/include/pango-1.0 -I$withval/lib/gtk-2.0/include -I$withval/include/atk-1.0" ; LDFLAGS="$LDFLAGS -L$withval/lib"]
  )

  fgfx_possible=true;

  OLDLIBS=$LIBS

  if test x$fgfx_possible = xtrue ; then
    fsdl_lib=false
    if test x$fsdl_lib = xfalse ; then
      AC_CHECK_PROGS(SDLCONFIG, sdl-config)
      if ! test "x$SDLCONFIG" = "x" ; then
        CPPFLAGS="$CPPFLAGS `sdl-config --cflags`"
        LDFLAGS="$LDFLAGS `sdl-config --libs`"
        fsdl_lib=true
      fi
    fi
    if test x$fsdl_lib = xfalse ; then
    AC_CHECK_HEADERS([SDL/SDL.h], [
      AC_CHECK_LIB(SDL,SDL_Init, [LIBS="-lSDL $LIBS"; fsdl_lib=true]
       ) ])
    fi
    if test x$fsdl_lib = xfalse ; then
      fgfx_possible=false;
      echo "Error, cannot use SDL library"
    fi
  fi
  if test x$fgfx_possible = xtrue ; then
    fsdlttf_lib=false
    AC_CHECK_HEADERS([SDL/SDL_ttf.h], [
      AC_CHECK_LIB(SDL_ttf,TTF_Init, [LIBS="-lSDL_ttf $LIBS";
                   fsdlttf_lib=true]) ])
    if test x$fsdlttf_lib = xfalse ; then
      fgfx_possible=false;
      echo "Error, cannot use SDL_ttf library"
      tail -100 config.log
      exit 1
    fi
  fi

  if test x$fgfx_possible = xtrue ; then
    fglu_lib=false
    AC_CHECK_HEADERS([GL/glu.h], [
      AC_CHECK_LIB(GLU,gluCylinder, [LIBS="-lGLU $LIBS";
                   fglu_lib=true]) ])
    AC_CHECK_HEADERS([OpenGL/glu.h], [
      AC_CHECK_LIB(GLU,gluCylinder, [LIBS="-lGLU $LIBS";
                   fglu_lib=true]) ])
    if test x$fglu_lib = xfalse ; then
      fgfx_possible=false;
      echo "Error, cannot use glut library" ; exit 1
    fi
  fi

  if test x$fgfx_possible = xtrue ; then
    fglut_lib=false
    AC_CHECK_HEADERS([GL/glut.h], [
      AC_CHECK_LIB(glut,glutInit, [LIBS="-lglut $LIBS";
                   fglut_lib=true]) ])
    AC_CHECK_HEADERS([OpenGL/glut.h], [
      AC_CHECK_LIB(glut,glutInit, [LIBS="-lglut $LIBS";
                   fglut_lib=true]) ])
    if test x$fglut_lib = xfalse ; then
      fgfx_possible=false;
      echo "Error, cannot use glut library" ; exit 1
    fi
  fi

  if test x$fgfx_possible = xtrue ; then
    fopengl_lib=false
    AC_CHECK_HEADERS([GL/gl.h], [
      AC_CHECK_LIB(GL,glColorTable, [LIBS="-lGL $LIBS";
                   fopengl_lib=true]) ])
    AC_CHECK_HEADERS([OpenGL/gl.h], [
      AC_CHECK_LIB(GL,glColorTable, [LIBS="-lGL $LIBS";
                   fopengl_lib=true]) ])
    if test x$fopengl_lib = xfalse ; then
      fgfx_possible=false;
      echo "Error, cannot use glut library" ; exit 1
    fi
  fi

  if test x$fgfx_possible = xtrue ; then
    fgtk_lib=false
    AC_CHECK_HEADERS([gtk/gtk.h], [
      AC_CHECK_LIB(gtk-x11-2.0,gtk_main, [LIBS="$LIBS -lgtk-x11-2.0";
                   fgtk_lib=true]) ])
    if test x$fgtk_lib = xfalse ; then
      fgfx_possible=false;
      echo "Error, cannot use glut library" ; exit 1
    fi
  fi

  if test x$fgfx_possible = xtrue ; then
    fgdk_lib=false
    AC_CHECK_HEADERS([gdk/gdk.h], [
      AC_CHECK_LIB(gdk-x11-2.0,main, [LIBS="$LIBS -lgdk-x11-2.0";
                   fgdk_lib=true]) ])
    if test x$fgdk_lib = xfalse ; then
      fgfx_possible=false;
      echo "Error, cannot use glut library" ; exit 1
    fi
  fi

  if test x$fgfx_possible = xtrue ; then
    fpango_lib=false
    AC_CHECK_HEADERS([pango/pango.h], [
      AC_CHECK_LIB(pango-1.0,pango_item_new, [LIBS="$LIBS -lpango-1.0";
                   fpango_lib=true]) ])
    if test x$fpango_lib = xfalse ; then
      fgfx_possible=false;
      echo "Error, cannot use glut library" ; exit 1
    fi
  fi

  if test x$fgfx_possible = xtrue ; then
    fatk_lib=false
    AC_CHECK_HEADERS([atk/atk.h], [
      AC_CHECK_LIB(atk-1.0,atk_action_do_action, [LIBS="$LIBS -latk-1.0";
                   fatk_lib=true]) ])
    if test x$fatk_lib = xfalse ; then
      fgfx_possible=false;
      echo "Error, cannot use glut library" ; exit 1
    fi
  fi

  GFXLIBS=$LIBS
  LIBS=$OLDLIBS

AM_CONDITIONAL(AMCGFX, [test x$fgfx_possible = xtrue])

AC_SUBST(FONTFILE)
AC_SUBST(GFXLIBS)

echo "fgsl_lib      is $fgsl_lib"
echo "fsdl_lib      is $fsdl_lib"
echo "fsdlttf_lib   is $fsdlttf_lib"
echo "fopengl_lib   is $fopengl_lib"
echo "fglu_lib      is $fglu_lib"
echo "fglut_lib     is $fglut_lib"
echo "fgtk_lib      is $fgtk_lib"
echo "fgdk_lib      is $fgdk_lib"
echo "fpango_lib    is $fpango_lib"
echo "fatk_lib      is $fatk_lib"
echo "fgfx          is $fgfx_possible"
echo "ua_optimize   is $ua_optimize"
echo "host          is $host"
echo "build         is $build"

AM_CONFIG_HEADER(src/aclconfig.h)
AC_OUTPUT([Makefile src/Makefile])
