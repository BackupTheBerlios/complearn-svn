
bin_PROGRAMS = ncd anycompress anydecompress complearn-config
lib_LTLIBRARIES   = libcomplearn.la libppmdx.la liblzmax.la libart.la

EXELDFLAGS = -lcomplearn

if WANT_RPATH
RPATHFLAGS= -rpath @abs_srcdir@/.libs
else
RPATHFLAGS=
endif

complearn_config_SOURCES = complearn-config.c
complearn_config_LDADD = $(EXELDFLAGS)
complearn_config_LDFLAGS = $(RPATHFLAGS)

ncd_SOURCES = ncd.c
ncd_LDADD = $(EXELDFLAGS)
ncd_LDFLAGS = $(RPATHFLAGS)

anycompress_SOURCES = anycompress.c
anycompress_LDADD = $(EXELDFLAGS)
anycompress_LDFLAGS = $(RPATHFLAGS)

anydecompress_SOURCES = anydecompress.c
anydecompress_LDADD = $(EXELDFLAGS)
anydecompress_LDFLAGS = $(RPATHFLAGS)

GOBCS = \
complearn-compressor-driver.c \
complearn-environment.c \
complearn-external-compressor.c \
complearn-external-file-compressor.c \
complearn-ncd.c \
complearn-rcbzip.c \
complearn-rcbzlib.c \
complearn-rcgzip.c \
complearn-rczlib.c \
complearn-real-compressor-adaptor.c \
complearn-vcblocksort.c

GOBS = \
complearn-compressor-driver.gob \
complearn-environment.gob \
complearn-external-compressor.gob \
complearn-external-file-compressor.gob \
complearn-ncd.gob \
complearn-lzmax.gob \
complearn-ppmdx.gob \
complearn-rcbzip.gob \
complearn-rcbzlib.gob \
complearn-rcgzip.gob \
complearn-rczlib.gob \
complearn-real-compressor-adaptor.gob \
complearn-vcart.gob \
complearn-vcblocksort.gob

GOBSTAMPS = \
complearn-compressor-driver.stamp \
complearn-environment.stamp \
complearn-external-compressor.stamp \
complearn-external-file-compressor.stamp \
complearn-ncd.stamp \
complearn-lzmax.stamp \
complearn-ppmdx.stamp \
complearn-rcbzip.stamp \
complearn-rcbzlib.stamp \
complearn-rcgzip.stamp \
complearn-rczlib.stamp \
complearn-real-compressor-adaptor.stamp \
complearn-vcart.stamp \
complearn-vcblocksort.stamp

GOBHS = \
complearn/complearn-compressor-driver.h \
complearn/complearn-compressor-driver-private.h \
complearn/complearn-environment.h \
complearn/complearn-environment-private.h \
complearn/complearn-external-compressor.h \
complearn/complearn-external-compressor-private.h \
complearn/complearn-external-file-compressor.h \
complearn/complearn-external-file-compressor-private.h \
complearn/complearn-ncd.h \
complearn/complearn-ncd-private.h \
complearn/complearn-lzmax.h \
complearn/complearn-lzmax-private.h \
complearn/complearn-ppmdx.h \
complearn/complearn-ppmdx-private.h \
complearn/complearn-rcbzip.h \
complearn/complearn-rcbzip-private.h \
complearn/complearn-rcbzlib.h \
complearn/complearn-rcbzlib-private.h \
complearn/complearn-rcgzip.h \
complearn/complearn-rcgzip-private.h \
complearn/complearn-rczlib.h \
complearn/complearn-rczlib-private.h \
complearn/complearn-real-compressor-adaptor.h \
complearn/complearn-real-compressor-adaptor-private.h \
complearn/complearn-vcblocksort.h \
complearn/complearn-vcblocksort-private.h

BASESOURCEC = \
cloutput.c \
qsufsort.c \
clnexus.c \
real-compressor.c \
util.c

BASEHEADERS = \
complearn/complearn.h \
complearn/cloutput.h \
complearn/newcomplearn.h \
complearn/aclconfig.h \
complearn/clconfig.h \
complearn/real-compressor.h \
complearn/util.h \
complearn/complearn.h

libcomplearn_la_SOURCES = $(GOBCS) $(BASESOURCEC)
libcomplearn_la_LDFLAGS= -version-info @libcomplearn_version@ @SONAMEOPTS@

libppmdx_la_SOURCES = complearn-ppmdx.c
libppmdx_la_LDFLAGS= -module

liblzmax_la_SOURCES = complearn-lzmax.c
liblzmax_la_LDFLAGS= -module

libart_la_SOURCES = complearn-vcart.c
libart_la_LDFLAGS = -module

MODHS = \
complearn/complearn-ppmdx.h \
complearn/complearn-ppmdx-private.h \
complearn/complearn-lzmax.h \
complearn/complearn-lzmax-private.h \
complearn/complearn-vcart.h \
complearn/complearn-vcart-private.h

MODCS=\
complearn-ppmdx.c \
complearn-lzmax.c \
complearn-vcart.c

lib_includedir=$(includedir)/complearn-1.0/complearn
lib_include_HEADERS= $(BASEHEADERS) $(GOBHS)

$(GOBHS): $(GOBSTAMPS)

$(GOBCS): $(GOBSTAMPS)

BUILT_SOURCES = $(GOBCS) $(GOBSTAMPS) $(MODCS)
MAINTAINERCLEANFILES = $(GOBCS) $(GOBHS) $(GOBSTAMPS)
EXTRA_DIST = $(GOBCS) $(GOBHS) $(MODHS) $(MODCS) $(GOBS)

GOBSOURCES = @GOBSOURCES@

.gob.stamp:
	$(GOB2) --always-private-header --no-touch $< -o @abs_srcdir@/complearn ; mv -f @abs_srcdir@/complearn/*.c . ; touch $@

