#include <complearn/complearn.h>

#ifdef HAVE_DLFCN_H
#include <dlfcn.h>
#endif

#if HAVE_ZLIB_H
#include <zlib.h>
#endif

#include "newcomp.h"

struct GZipCompressionInstance {
  void *baseClass;
  int level;
};

struct ZlibDynamicAdaptorCB {
  int (*compress2)(unsigned char *dbuff,unsigned long *p,
    unsigned char *src, unsigned long sz, int level);
  int (*uncompress)(unsigned char *dbuff,unsigned long *p, unsigned char *src, unsigned long sz);
};

#if HAVE_ZLIB_H
static struct ZlibDynamicAdaptorCB zlibsda = {
  (int (*)(unsigned char *dbuff,unsigned long *p, unsigned char *src, unsigned long sz, int level) ) compress2,
  (int (*)(unsigned char *dbuff,unsigned long *p, unsigned char *src, unsigned long sz) ) uncompress
};
#else
static struct ZlibDynamicAdaptorCB zlibsda;
#endif

static struct ZlibDynamicAdaptorCB *clGrabZlibDACB(void);

static int fgetWindowSizeCB(void)
{
  return 32768;
}

static const char *fshortNameCB(void)
{
  return "gzip";
}

static int fallocSizeCB(void)
{
  return sizeof(struct GZipCompressionInstance);
}

static double fcompressCB(struct CompressionBase *cb, struct DataBlock *src)
{
  struct GZipCompressionInstance *gzci = (struct GZipCompressionInstance *) cb;
  struct ZlibDynamicAdaptorCB *zlib = clGrabZlibDACB();
  int s;

  unsigned char *dbuff;
  unsigned long p;

  if (zlib == NULL) {
    clogError("Cannot do zlib compression: no zlib library available.");
  }

  p = clDatablockSize(src)*1.2 + 12;
  dbuff = (unsigned char*)clMalloc(p);
  s = (zlib->compress2)(dbuff,&p,clDatablockData(src),clDatablockSize(src),
                        gzci->level);

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

static int fspecificInitCB(struct CompressionBase *cb)
{
  struct GZipCompressionInstance *gzci = (struct GZipCompressionInstance *) cb;
  gzci->level = 9; // Best compression by default
}

static int fprepareToCompressCB(struct CompressionBase *cb)
{
  const char *clvl = clEnvmapValueForKey(clGetParametersCB(cb), "level");
  struct GZipCompressionInstance *gzci = (struct GZipCompressionInstance *) cb;
  if (clvl != NULL)
    gzci->level = atoi(clvl);
  if (gzci->level < 1)
    gzci->level = 1;
  if (gzci->level > 9)
    gzci->level = 9;
  printf("In gzip-specific prepare function, with level param %d\n", gzci->level);
}

static struct CompressionBaseAdaptor cba = {
  specificInitCB : fspecificInitCB,
  prepareToCompressCB : fprepareToCompressCB,
  compressCB : fcompressCB,
  getWindowSizeCB : fgetWindowSizeCB,
  shortNameCB : fshortNameCB,
  allocSizeCB : fallocSizeCB
};

static void initGZ(void)
{
//#define REGTYPEFORNAME(name, typ, xcba) clRegisterCB(#name, sizeof(typ), &xcba)
  clRegisterCB(&cba);
}

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

static struct ZlibDynamicAdaptorCB *clGrabZlibDACB(void) {
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

int main(int argc, char **argv) {
  initGZ();
  struct CompressionBase *cb = clNewCompressorCB("gzip");
  clSetParameterCB(cb, "level", "4", 0);
  printf("Using parameters %s\n", clGetParamStringCB(cb));
  struct DataBlock *db;
  db = clStringToDataBlockPtr("liiiiiiiissssssssssaaaaaaaa");
  printf("%f\n", clCompressCB(cb, db));
  db = clStringToDataBlockPtr("liiiiiiiissssssssssaaaaaaaaaxaaaaaaaaaaaa");
  printf("%f (with 3 more)\n", clCompressCB(cb, db));
  clFreeCB(cb);
  printCompressors();
  return 0;
}
