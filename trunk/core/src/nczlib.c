#include <complearn/complearn.h>

#include "newcomp.h"
#include "ncazlib.h"

struct ZlibDynamicAdaptorCB *clGrabZlibDACB(void);

struct ZLibCompressionInstance {
  void *baseClass;
  int level;
};

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
  return sizeof(struct ZLibCompressionInstance);
}

static int fisCompileProblemCB(void)
{
#if HAVE_ZLIB_H
  return 0;
#endif
  if (clGrabZlibDACB() != NULL)
    return 0;
  clSetStaticErrorMessage(fshortNameCB(), "Cannot open zlib library");
  return 1;
}

static double fcompressCB(struct CompressionBase *cb, struct DataBlock *src)
{
  struct ZLibCompressionInstance *gzci = (struct ZLibCompressionInstance *) cb;
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
  struct ZLibCompressionInstance *gzci = (struct ZLibCompressionInstance *) cb;
  gzci->level = 9; // Best compression by default
}

static int fprepareToCompressCB(struct CompressionBase *cb)
{
  const char *clvl = clEnvmapValueForKey(clGetParametersCB(cb), "level");
  struct ZLibCompressionInstance *gzci = (struct ZLibCompressionInstance *) cb;
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

void initBZ2(void);
int main(int argc, char **argv) {
  initGZ();
  initBZ2();
  struct CompressionBase *cb = clNewCompressorCB("bzip2");
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
