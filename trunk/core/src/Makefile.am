BASICPROGRAMS = ncd reordertree improvetree maketree svdproj clbinfo
MPIPROGRAMS = partree
GRAPHICSPROGRAMS = clfun
GFXLIBS = @GFXLIBS@

GENCPPFLAGS = -I complearn
CLLDFLAGS = libcomplearn.a

AM_CPPFLAGS = $(GENCPPFLAGS)

if AMCMPI
LEV2PROGRAMS = $(BASICPROGRAMS) $(MPIPROGRAMS)
else
LEV2PROGRAMS = $(BASICPROGRAMS)
endif

# TODO: Should not include NGDLIBS?? /alcruz

if AMCGFX
LEV3PROGRAMS = $(LEV2PROGRAMS) clfun

clfun_SOURCES = clfun.c textsdl.c
clfun_LDADD = $(CLLDFLAGS) $(NGDLIBS) $(GFXLIBS)
else
LEV3PROGRAMS = $(LEV2PROGRAMS)
endif


bin_PROGRAMS = $(LEV3PROGRAMS)

maketree_SOURCES = maketree.c
maketree_LDADD = $(CLLDFLAGS)

improvetree_SOURCES = improvetree.c
improvetree_LDADD = $(CLLDFLAGS)

svdproj_SOURCES = svdproj.c
svdproj_LDADD = $(CLLDFLAGS)

clbinfo_SOURCES = clbinfo.c
clbinfo_LDADD = $(CLLDFLAGS)

ncd_SOURCES = ncd.c
ncd_LDADD = $(CLLDFLAGS) $(NGDLIBS)

reordertree_SOURCES = reordertree.c
reordertree_LDADD = $(CLLDFLAGS)

partree_SOURCES = partree.c
partree_LDADD = $(CLLDFLAGS)

#include target.mk

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
complearn/textsdl.h \
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

