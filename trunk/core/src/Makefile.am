GENLDFLAGS=-Wl,-export-dynamic
GENLDADD=libcomplearn.a

if WANT_SDLGLPROG
bin_PROGRAMS=ncd runtests svdproj maketree clbinfo sdlgl
else
bin_PROGRAMS=ncd runtests svdproj maketree clbinfo
endif


ncd_SOURCES=ncd.c
ncd_LDADD=$(GENLDADD)
ncd_LDFLAGS=$(GENLDFLAGS)

runtests_SOURCES=runtests.c
runtests_LDADD=$(GENLDADD)
runtests_LDFLAGS=$(GENLDFLAGS)

svdproj_SOURCES=svdproj.c
svdproj_LDADD=$(GENLDADD)
svdproj_LDFLAGS=$(GENLDFLAGS)

maketree_SOURCES=maketree.c
maketree_LDADD=$(GENLDADD)
maketree_LDFLAGS=$(GENLDFLAGS)

clbinfo_SOURCES=clbinfo.c
clbinfo_LDADD=$(GENLDADD)
clbinfo_LDFLAGS=$(GENLDFLAGS)

sdlgl_SOURCES=sdlgl.c textsdl.c
sdlgl_LDADD=$(GENLDADD)
sdlgl_LDFLAGS=$(GENLDFLAGS)
#sdlgl_LDADD=-lSDL -L/usr/lib -L/usr/local/lib -lglu32 -lglut32 -lopengl32 libcomplearn.a -lgsl -lgslcblas -lSDL_ttf -lwinmm

# 87  gcc sdlgl.c -g -Wall -o sdlgl.exe textsdl.c -lSDL -I/usr/include -I/usr/local/include/SDL -L/usr/lib -L/usr/local/lib -lglu32 -lglut32 -lopengl32 -I.. ./libcomplearn.a -lgsl -lgslcblas -lSDL_ttf -lwinmm -I.

lib_LIBRARIES=libcomplearn.a
lib_includedir=$(includedir)/complearn-1.0/complearn
lib_include_HEADERS=\
complearn/complearn.h \
complearn/aclconfig.h \
complearn/uclconfig.h \
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
complearn/dlextcomp.h \
complearn/doublea.h \
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
complearn/textsdl.h \
complearn/transadaptor.h \
complearn/treeadaptor.h \
complearn/treeholder.h \
complearn/treemolder.h \
complearn/treeblaster.h \
complearn/treemaster.h \
complearn/treescore.h \
complearn/clparser.h

libcomplearn_a_SOURCES=\
adjadaptor.c \
adjlist.c \
adjmatrix.c \
builtin.c \
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
doublea.c \
dbe-array.c \
dbe-dir.c \
dbe-factory.c \
dbe-filelist.c \
dbe-singleton.c \
dbe-stringlist.c \
dbe-windowed.c \
dlextcomp.c \
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
clparser.c

