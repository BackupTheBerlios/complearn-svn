#include <stdlib.h>
#include <string.h>
#include <complearn/complearn.h>

#ifdef HAVE_DLFCN_H
#include <dlfcn.h>
#endif

#ifdef HAVE_BZLIB_H
#include <bzlib.h>
#endif

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
static struct BZ2DynamicAdaptorCB bz2sda = {
  BZ2_bzBuffToBuffCompress,
  BZ2_bzBuffToBuffDecompress
};
#else
static struct BZ2DynamicAdaptorCB bz2sda;
#endif

static struct BZ2DynamicAdaptorCB bz2dda;
static int haveTriedDL; /* Singleton */

struct BZ2DynamicAdaptorCB *clGrabBZ2DACB(void) {
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

