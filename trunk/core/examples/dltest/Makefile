ifeq ($(wildcard /usr/include/complearn-1.0),)
INCDIR=../../src
else
INCDIR=/usr/include/complearn-1.0
INC2=$(INCDIR)
endif
ifeq ($(wildcard /usr/lib/libcomplearn.so.?),)
CLLIBPATH=../../src/.libs/libcomplearn.so
else
CLLIBPATH=$(wildcard /usr/lib/libcomplearn.so.?)
endif

CFLAGS=-I$(INCDIR)

all: libart.so.1.0.1

%.o: %.c
	gcc -fPIC -g -c -Wall $< $(CFLAGS)

libart.so.1.0.1: libart.o
	gcc -shared -Wl,-soname,libart.so.1 -o libart.so.1.0.1 libart.o $(CLLIBPATH) -lc -lm

install:
	echo "(no installation)"

distclean: clean


clean:
	rm libart.o libart.so.*

distdir:
	echo "(no distdir)"
