/*
* Copyright (c) 2006 Rudi Cilibrasi, Rulers of the RHouse
* All rights reserved.     cilibrar@cilibrar.com
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*
*     * Redistributions of source code must retain the above copyright
*       notice, this list of conditions and the following disclaimer.
*     * Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
*     * Neither the name of the RHouse nor the
*       names of its contributors may be used to endorse or promote products
*       derived from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE RULERS AND CONTRIBUTORS "AS IS" AND ANY
* EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL THE RULERS AND CONTRIBUTORS BE LIABLE FOR ANY
* DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#include <stdlib.h>
#include <string.h>
#include <complearn/complearn.h>

#ifdef HAVE_DLFCN_H
#include <dlfcn.h>
#endif

#ifdef HAVE_BZLIB_H
#include <bzlib.h>
#endif

/* NOTE:
 *
 * A library is not linkable with C unless it has a header, so we may
 * use #ifdef *_H instead of _RDY everywhere to avoid the new flag.
 *
 * If a user has headers installed but no library, we can rely on a linker
 * to tell them an error message.
 */

static double bz2a_compclFunc(struct CompAdaptor *ca, struct DataBlock *src);
static void bz2a_freecompclFunc(struct CompAdaptor *ca);
static char *bz2a_shortname(void);
static char *bz2a_longname(void);
static int bz2a_apiver(void);

/***** Dynamic Adaptor module to support dual-mode static / dynamic loading
 *
 * This system is intended to simplify installation by allowing users to
 * install a dynamic-loading bz2 library after static compiletime config
 * without reconfiguring.  This is done through a two-step strategy on
 * compressor module startup:
 *
 * First, if a static symbol has been compiled, link with it and use it.
 * If it hasn't, search using dlopen for the bz2 library and load the
 * necessary symbols (compression and decompression) into an adaptor.
 * Expose this adaptor to the rest of the system via a Singleton Facade.
 */

#if HAVE_BZLIB_H
static struct BZ2DynamicAdaptor bz2sda = {
  BZ2_bzBuffToBuffCompress,
  BZ2_bzBuffToBuffDecompress
};
#else
static struct BZ2DynamicAdaptor bz2sda;
#endif

static struct BZ2DynamicAdaptor bz2dda;
static int haveTriedDL; /* Singleton */

struct BZ2DynamicAdaptor *clGrabBZ2DA(void) {
  if (bz2sda.buftobufcompress)
    return &bz2sda;
  if (!haveTriedDL) {
    void *lib_handle;
    haveTriedDL = 1;
#ifdef HAVE_DLFCN_H
    lib_handle = dlopen("libbz2.so", RTLD_LAZY);
    if (lib_handle) {
      bz2dda.buftobufcompress= dlsym(lib_handle,"BZ2_bzBuffToBuffCompress");
      bz2dda.buftobufdecompress= dlsym(lib_handle,"BZ2_bzBuffToBuffDecompress");
//      dlclose(lib_handle);
    }
#endif
  }
  return bz2dda.buftobufcompress ? &bz2dda : NULL;
}

/* bzip2 compression interface */

/** \brief The BZIP2 compression adaptor instance
 *  \struct BZ2CompInstance
 *
 *  This structure holds all the persistent configuration information
 *  governing how the block-sorting compressor behaves.  Several parameters
 *  that effect the behavior are listed as fields below.
 */
struct BZ2CompInstance {
  int blocksize;  /*!< the size of each block, in multiples of 100K; this must
                       be between 1 and 9 inclusive.  The default, 9, is the
                       largest block, measuring just under one megabyte. */
  int workfactor; /*!< a performance-tuning parameter that can effect
                       compression speed drastically for certain types of
                       highly repetitive low-entropy files.  This parameter
                       should now affect results, but may effect the time it
                       takes to reach them. */
  int verbosity;  /*!< a bzip-only verbosity flag, either 0 or 1, to suppress
                       or encourage the printing of status or other messages.*/
};

/** \brief Initializes a BZIP2 CompAdaptor instance
 *
 *  clBuiltin_BZIP() allocates memory to a BZIP2 CompAdaptor instance. The
 *  BZIP2 CompAdaptor is required for use in computing NCDs with the bzip2
 *  compression library.
 *
 *  There are 3 options that can be set for the bzip2 CompAdaptor: blocksize,
 *  workfactor, and verbosity.  By default, blocksize is set to 9, workfactor
 *  is set to 30, and verbosity is set to 0.
 *
 *  clBuiltin_BZIP() will also read a CompLearn configuration file to override
 *  the option defaults. For details on how to create a configuration file, see
 *  http://www.complearn.org/config.html
 *
 *  \return pointer to newly initialized BZIP2 CompAdaptor instance
 */
struct CompAdaptor *clBuiltin_BZIP(void)
{
	struct CompAdaptor c =
	{
    cptr: NULL,
//    se:   bz2a_clsetenv,
    cf:   bz2a_compclFunc,
    fcf:  bz2a_freecompclFunc,
    sn:   bz2a_shortname,
    ln:   bz2a_longname,
    apiv: bz2a_apiver,
  };
  struct CompAdaptor *ca;
  struct BZ2CompInstance *bzci;
  struct BZ2DynamicAdaptor *bzlib;
  bzlib = clGrabBZ2DA();
  if (!bzlib)
    return NULL;
  ca = clCalloc(sizeof(*ca), 1);
  *ca = c;
  ca->cptr = clCalloc(sizeof(struct BZ2CompInstance), 1);
  bzci = (struct BZ2CompInstance *) ca->cptr;
  clCompaInitParameters(ca);

  /* default compressor options */
  clCompaPushParameter(ca, "blocksize", "9", PARAMINT);
  clCompaPushParameter(ca, "workfactor", "30" , PARAMINT);
  clCompaPushParameter(ca, "bzverbosity", "0", PARAMINT);

  clCompaSetValueForKey(ca, "blocksize", &bzci->blocksize);
  clCompaSetValueForKey(ca, "workfactor", &bzci->workfactor);
  clCompaSetValueForKey(ca, "bzverbosity", &bzci->verbosity);

  return ca;
}

static double bz2a_compclFunc(struct CompAdaptor *ca, struct DataBlock *src)
{
  struct BZ2DynamicAdaptor *bzlib = clGrabBZ2DA();
	struct BZ2CompInstance *bzci = (struct BZ2CompInstance *) ca->cptr;
	int s;

  unsigned char *dbuff;
	int p;

  if (bzlib == NULL) {
    clogError("Cannot do bzip2 compression: no bzip2 library available.");
  }
	p = clDatablockSize(src)*1.02+600;
	dbuff = (unsigned char*)clMalloc(p);
	s = (bzlib->buftobufcompress)((char *) dbuff,(unsigned int *) &p,(char *) clDatablockData(src),clDatablockSize(src),
			bzci->blocksize, bzci->verbosity, bzci->workfactor);
	if (s != 0) {
		printf ("error code %d\n", s);
		exit(1);
	}
	free(dbuff);
	return (double) p*8.0;
}

static void bz2a_freecompclFunc(struct CompAdaptor *ca)
{
  clFreeandclear(ca->cptr);
  clFreeandclear(ca);
}

static char *bz2a_shortname(void)
{
	return "bzip";
}

static char *bz2a_longname(void)
{
	return "Block-sorting compressor bzip2";
}

static int bz2a_apiver(void)
{
	return APIVER_V1;
}

