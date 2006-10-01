#include <stdio.h>
#include <complearn/complearn.h>

#ifdef HAVE_DLFCN_H
#include <dlfcn.h>
#endif

#if HAVE_ZLIB_H
#include <zlib.h>
#endif


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

static struct ZlibDynamicAdaptorCB zlibdda;
static int haveTriedDL; /* Singleton */

#if HAVE_ZLIB_H
static struct ZlibDynamicAdaptorCB zlibsda = {
  (int (*)(unsigned char *dbuff,unsigned long *p, unsigned char *src, unsigned long sz, int level) ) compress2,
  (int (*)(unsigned char *dbuff,unsigned long *p, unsigned char *src, unsigned long sz) ) uncompress
};
#else
static struct ZlibDynamicAdaptorCB zlibsda;
#endif


struct ZlibDynamicAdaptorCB *clGrabZlibDACB(void) {
  if (zlibsda.compress2)
    return &zlibsda;
  if (!haveTriedDL) {
    void *lib_handle;
    haveTriedDL = 1;
#if HAVE_DLFCN_H
    lib_handle = dlopen("libz.so", RTLD_LAZY);
    if (lib_handle) {
      zlibdda.compress2= dlsym(lib_handle,"compress2");
      zlibdda.uncompress= dlsym(lib_handle,"uncompress");
    }
#endif
  }
  return zlibdda.compress2 ? &zlibdda : NULL;
}

struct DataBlock *clCompressZLibDB(struct DataBlock *src)
{
  struct DataBlock *res;
  struct ZlibDynamicAdaptorCB *zlib = clGrabZlibDACB();
  int s;

  unsigned char *dbuff;
  unsigned long p;

  if (zlib == NULL) {
    return clDatablockClonePtr(src);
  }

  p = clDatablockSize(src)*1.2 + 12;
  dbuff = (unsigned char*)clMalloc(p);
  s = (zlib->compress2)(dbuff,&p,clDatablockData(src),clDatablockSize(src),9);
  if (s != 0) {    /* Z_OK */
    clLogError("Unknown error: zlibBuff returned %d\n",s);
  }
  res = clDatablockNewFromBlock(dbuff, p);
  free(dbuff);
  return res;
}
