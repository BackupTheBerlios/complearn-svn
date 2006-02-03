#include <stdlib.h>
#include <complearn/complearn.h>

#ifdef HAVE_DLFCN_H
#include <dlfcn.h>
#endif

#if HAVE_ZLIB_H
#include <zlib.h>
#endif

/* NOTE:
 *
 * A library is not linkable with C unless it has a header, so we may
 * use #ifdef *_H instead of _RDY everywhere to avoid the new flag.
 *
 * If a user has headers installed but no library, we can rely on a linker
 * to tell them an error message.
 */

static double zlib_compfunc(struct CompAdaptor *ca, struct DataBlock *src);
static void zlib_freecompfunc(struct CompAdaptor *ca);
static char *zlib_shortname(void);
static char *zlib_longname(void);
static int zlib_apiver(void);

/***** Dynamic Adaptor module to support dual-mode static / dynamic loading
 *
 * This system is intended to simplify installation by allowing users to
 * install a dynamic-loading zlib library after static compiletime config
 * without reconfiguring.  This is done through a two-step strategy on
 * compressor module startup:
 *
 * First, if a static symbol has been compiled, link with it and use it.
 * If it hasn't, search using dlopen for the zlib library and load the
 * necessary symbols (compression and decompression) into an adaptor.
 * Expose this adaptor to the rest of the system via a Singleton Facade.
 */

#if HAVE_ZLIB_H
static struct ZlibDynamicAdaptor zlibsda = {
  (int (*)(unsigned char *dbuff,unsigned long *p, unsigned char *src, unsigned long sz, int level) ) compress2,
  (int (*)(unsigned char *dbuff,unsigned long *p, unsigned char *src, unsigned long sz) ) uncompress
};
#else
static struct ZlibDynamicAdaptor zlibsda;
#endif

static struct ZlibDynamicAdaptor zlibdda;
static int haveTriedDL; /* Singleton */

struct ZlibDynamicAdaptor *grabZlibDA(void) {
  if (zlibsda.compress2)
    return &zlibsda;
  if (!haveTriedDL) {
    void *lib_handle;
    haveTriedDL = 1;
#ifdef HAVE_DLFCN_H
    lib_handle = dlopen("libz.so", RTLD_LAZY);
    if (lib_handle) {
      zlibdda.compress2= dlsym(lib_handle,"compress2");
      zlibdda.uncompress= dlsym(lib_handle,"uncompress");
    }
#endif
  }
  return zlibdda.compress2 ? &zlibdda : NULL;
}

/** \brief zlib compression state information
 * \struct ZlibCompInstance
 *
 * This structure holds all zlib compression information.  At this time
 * it only allows for the adjustment of the compression level, which
 * is defaulted to the maximum level of 9.
 */
struct ZlibCompInstance {
  int level; // 0 - 9
};

/** \brief Initializes a ZLIB CompAdaptor instance
 *
 *  builtin_ZLIB() allocates memory to a ZLIB CompAdaptor instance. The
 *  ZLIB CompAdaptor is required for use in computing NCDs with the zlib
 *  compression library.
 *
 *  One option can be set for the ZLIB CompAdaptor: level. By default, level is
 *  set to 9.
 *
 *  builtin_ZLIB() will also read a CompLearn configuration file to override
 *  the option defaults. For details on how to create a configuration file, see
 *  http://www.complearn.org/config.html
 *
 *  \return pointer to newly initialized ZLIB CompAdaptor instance
 */
struct CompAdaptor *builtin_ZLIB(void)
{
	struct CompAdaptor c =
	{
    cptr: NULL,
//    se:   zlib_clsetenv,
    cf:   zlib_compfunc,
    fcf:  zlib_freecompfunc,
    sn:   zlib_shortname,
    ln:   zlib_longname,
    apiv: zlib_apiver
  };
  struct CompAdaptor *ca;
  struct ZlibCompInstance *zci;
  struct ZlibDynamicAdaptor *zlib;
  zlib = grabZlibDA();
  if (!zlib)
    return NULL;
  ca = clCalloc(sizeof(*ca), 1);
  *ca = c;
  ca->cptr = clCalloc(sizeof(struct ZlibCompInstance), 1);
  zci = (struct ZlibCompInstance *) ca->cptr;

  compaInitParameters(ca);

  /* default compressor options */
  compaPushParameter(ca, "zliblevel", "9", PARAMINT);
  compaSetValueForKey(ca, "zliblevel", &zci->level);

  return ca;
}

static double zlib_compfunc(struct CompAdaptor *ca, struct DataBlock *src)
{
  struct ZlibDynamicAdaptor *zlib = grabZlibDA();
	struct ZlibCompInstance *ci = (struct ZlibCompInstance *) ca->cptr;
	int s;

  unsigned char *dbuff;
	unsigned long p;

  if (zlib == NULL) {
    clogError("Cannot do zlib compression: no zlib library available.");
  }

	p = datablockSize(src)*1.001 + 12;
	dbuff = (unsigned char*)clMalloc(p);
	s = (zlib->compress2)(dbuff,&p,datablockData(src),datablockSize(src),ci->level);

	if (s == -5 ) {  /* Z_BUF_ERROR */
		printf ("destLen not big enough!\n");
		exit(1);
	}
	if (s != 0) {    /* Z_OK */
		printf ("Unknown error: zlibBuff returned %d\n",s);
		exit(1);
	}
	free(dbuff);
	return (double) p*8.0;
}

static void zlib_freecompfunc(struct CompAdaptor *ca)
{
  clFreeandclear(ca->cptr);
	clFreeandclear(ca);
}

static char *zlib_shortname(void)
{
	return "zlib";
}

static char *zlib_longname(void)
{
	return "General purpose compression library zlib";
}

static int zlib_apiver(void)
{
	return APIVER_V1;
}
