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

BASICPROGRAMS = ncd reordertree improvetree maketree svdproj clbinfo splittree

GENCPPFLAGS = -I complearn -I .
CLLDFLAGS = .libs/libcomplearn.so $(NGDLIBS)

AM_CPPFLAGS = $(GENCPPFLAGS)

LEV2PROGRAMS = $(BASICPROGRAMS)

LEV3PROGRAMS = $(LEV2PROGRAMS) $(NGDLIBS)

bin_PROGRAMS = $(LEV3PROGRAMS)
check_PROGRAMS = runtests

EXTRA_PROGRAMS=

runtests_SOURCES = runtests.c
runtests_LDADD = $(CLLDFLAGS) $(NGDLIBS)

maketree_SOURCES = maketree.c
maketree_LDADD = $(CLLDFLAGS)

improvetree_SOURCES = improvetree.c
improvetree_LDADD = $(CLLDFLAGS)

svdproj_SOURCES = svdproj.c
svdproj_LDADD = $(CLLDFLAGS)

clbinfo_SOURCES = clbinfo.c
clbinfo_LDADD = $(CLLDFLAGS)

splittree_SOURCES = splittree.c
splittree_LDADD = $(CLLDFLAGS)

ncd_SOURCES = ncd.c
ncd_LDADD = $(CLLDFLAGS) $(NGDLIBS)

reordertree_SOURCES = reordertree.c
reordertree_LDADD = $(CLLDFLAGS)

#include target.mk

#lib_LIBRARIES=libcomplearn.a
lib_includedir=$(includedir)/complearn-1.0/complearn
lib_include_HEADERS=\
complearn/dlextcomp.h \
complearn/complearn.h \
complearn/aclconfig.h \
complearn/uclconfig.h \
complearn/ncazlib.h \
complearn/ncabz2.h \
complearn/ncbase.h \
complearn/nccbi.h \
complearn/ncblocksort.h \
complearn/adjadaptor.h \
complearn/adjlist.h \
complearn/adjmatrix.h \
complearn/builtin.h \
complearn/clconfig.h \
complearn/clerror.h \
complearn/clnodeset.h \
complearn/cloutput.h \
complearn/unrootedbinary.h \
complearn/cltypes.h \
complearn/compadaptor.h \
complearn/datablockenum.h \
complearn/datablock.h \
complearn/datetime.h \
complearn/dbe-array.h \
complearn/dbe-dir.h \
complearn/dbe-factory.h \
complearn/dbe-filelist.h \
complearn/dbe-singleton.h \
complearn/dbe-stringlist.h \
complearn/dbe-windowed.h \
complearn/dotparser.h \
complearn/dra.h \
complearn/environment.h \
complearn/envmap.h \
complearn/clalloc.h \
complearn/gdbmhelper.h \
complearn/googlecache.h \
complearn/google.h \
complearn/incrdm.h \
complearn/labelmaker.h \
complearn/labelperm.h \
complearn/matfact.h \
complearn/mutate.h \
complearn/ncd-calc.h \
complearn/paramlist.h \
complearn/pathkeeper.h \
complearn/quartet.h \
complearn/quartetobs.h \
complearn/rootedbinary.h \
complearn/springball.h \
complearn/stamp-h1 \
complearn/stringstack.h \
complearn/tagfile.h \
complearn/tagtypes.h \
complearn/tastack.h \
complearn/transadaptor.h \
complearn/treeadaptor.h \
complearn/treeholder.h \
complearn/treemolder.h \
complearn/treeblaster.h \
complearn/treemaster.h \
complearn/treescore.h \
complearn/treeparser.h \
complearn/clog.h \
complearn/clwrap.h \
complearn/clparser.h

libcomplearn_a_SOURCES=\
adjadaptor.c \
adjlist.c \
adjmatrix.c \
builtin.c \
ncbase.c \
ncazlib.c \
ncabz2.c \
builtinca-bzlib.c \
builtinca-google.c \
builtinca-realcomp.c \
builtinca-soap.c \
builtinca-virtcomp.c \
builtinca-blocksort.c \
qsufsort.c \
builtinca-zlib.c \
builtinta-unbzip.c \
builtinta-ungz.c \
builtinta-unzlib.c \
clnodeset.c \
cloutput.c \
compadaptor.c \
unrootedbinary.c \
datablock.c \
datetime.c \
dra.c \
dbe-array.c \
dbe-dir.c \
dbe-factory.c \
dbe-filelist.c \
dbe-singleton.c \
dbe-stringlist.c \
dbe-windowed.c \
dlextcomp.c \
dotparser.c \
envmap.c \
environment.c \
gdbmhelper.c \
clalloc.c \
google.c \
googlecache.c \
incrdm.c \
labelmaker.c \
labelperm.c \
matfact.c \
mutate.c \
ncd-calc.c \
paramlist.c \
pathkeeper.c \
quartet.c \
rootedbinary.c \
springball.c \
stringstack.c \
tastack.c \
tagfile.c \
treeadaptor.c \
treeblaster.c \
treeholder.c \
treemolder.c \
treemaster.c \
treescore.c \
treeparser.c \
clog.c \
clwrap.c \
clparser.c

lib_LTLIBRARIES= libcomplearn.la
libcomplearn_la_SOURCES= $(libcomplearn_a_SOURCES)
libcomplearn_la_LIBADD= $(NGDLIBS)

clean:
	rm -rf *.o *.lo $(BASICPROGRAMS) .libs

distclean:
	rm -rf Makefile *.o .libs libcomple*
